#include "secure_aes_gcm.h"

#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

#define SALT_SIZE  16
#define NONCE_SIZE 12   // recommandé pour GCM
#define KEY_SIZE   32   // 256 bits
#define TAG_SIZE   16   // 128 bits
#define PBKDF2_ITERATIONS 100000

static bool derive_key(const char *pass, const unsigned char *salt,
                       unsigned char *outKey, size_t keyLen)
{
    // Utilise PBKDF2-HMAC-SHA256
    if (!PKCS5_PBKDF2_HMAC(pass, strlen(pass),
                           salt, SALT_SIZE,
                           PBKDF2_ITERATIONS,
                           EVP_sha256(),
                           (int)keyLen, outKey)) {
        return false;
    }
    return true;
}

void secure_memzero(void *ptr, size_t len)
{
    if (ptr && len > 0) {
        OPENSSL_cleanse(ptr, len);
    }
}

bool secure_aes_gcm_encrypt_file(const char *inFile, const char *outFile, const char *pass)
{
    bool ret = false;
    FILE *fIn = NULL, *fOut = NULL;
    EVP_CIPHER_CTX *ctx = NULL;

    unsigned char salt[SALT_SIZE];
    unsigned char nonce[NONCE_SIZE];
    unsigned char key[KEY_SIZE];
    unsigned char bufferIn[4096];
    unsigned char bufferOut[4096 + EVP_MAX_BLOCK_LENGTH];
    int outLen = 0;

    // 1) Ouvrir les fichiers
    fIn = fopen(inFile, "rb");
    if (!fIn) {
        perror("fopen inFile");
        goto cleanup;
    }
    fOut = fopen(outFile, "wb");
    if (!fOut) {
        perror("fopen outFile");
        goto cleanup;
    }

    // 2) Générer salt et nonce aléatoires
    if (1 != RAND_bytes(salt, SALT_SIZE)) {
        fprintf(stderr, "Erreur RAND_bytes salt\n");
        goto cleanup;
    }
    if (1 != RAND_bytes(nonce, NONCE_SIZE)) {
        fprintf(stderr, "Erreur RAND_bytes nonce\n");
        goto cleanup;
    }

    // 3) Écrire salt + nonce en clair
    if (fwrite(salt, 1, SALT_SIZE, fOut) != SALT_SIZE) {
        fprintf(stderr, "Erreur fwrite salt\n");
        goto cleanup;
    }
    if (fwrite(nonce, 1, NONCE_SIZE, fOut) != NONCE_SIZE) {
        fprintf(stderr, "Erreur fwrite nonce\n");
        goto cleanup;
    }

    // 4) Dériver clé via PBKDF2
    if (!derive_key(pass, salt, key, KEY_SIZE)) {
        fprintf(stderr, "Erreur derive_key\n");
        goto cleanup;
    }

    // 5) Créer le contexte AES-GCM
    ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Erreur EVP_CIPHER_CTX_new\n");
        goto cleanup;
    }
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) {
        fprintf(stderr, "Erreur EVP_EncryptInit_ex (AES-256-GCM)\n");
        goto cleanup;
    }

    // Ajuster la taille du nonce (12 octets)
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, NONCE_SIZE, NULL)) {
        fprintf(stderr, "Erreur EVP_CIPHER_CTX_ctrl (set IV len)\n");
        goto cleanup;
    }

    // Initialiser la clé + nonce
    if (1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, nonce)) {
        fprintf(stderr, "Erreur EVP_EncryptInit_ex (key, nonce)\n");
        goto cleanup;
    }

    // 6) Lire bloc par bloc, chiffrer en GCM
    while (!feof(fIn)) {
        int bytesRead = (int)fread(bufferIn, 1, sizeof(bufferIn), fIn);
        if (ferror(fIn)) {
            fprintf(stderr, "Erreur fread\n");
            goto cleanup;
        }
        if (bytesRead > 0) {
            if (1 != EVP_EncryptUpdate(ctx, bufferOut, &outLen, bufferIn, bytesRead)) {
                fprintf(stderr, "Erreur EVP_EncryptUpdate\n");
                goto cleanup;
            }
            // Écrire la portion chiffrée
            if (fwrite(bufferOut, 1, outLen, fOut) != (size_t)outLen) {
                fprintf(stderr, "Erreur fwrite ciphertext\n");
                goto cleanup;
            }
        }
    }

    // Finaliser
    if (1 != EVP_EncryptFinal_ex(ctx, bufferOut, &outLen)) {
        fprintf(stderr, "Erreur EVP_EncryptFinal_ex\n");
        goto cleanup;
    }
    if (outLen > 0) {
        if (fwrite(bufferOut, 1, outLen, fOut) != (size_t)outLen) {
            fprintf(stderr, "Erreur fwrite final ciphertext\n");
            goto cleanup;
        }
    }

    // 7) Récupérer le tag GCM (16 octets) et l’écrire en fin de fichier
    unsigned char tag[TAG_SIZE];
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_SIZE, tag)) {
        fprintf(stderr, "Erreur EVP_CTRL_GCM_GET_TAG\n");
        goto cleanup;
    }
    if (fwrite(tag, 1, TAG_SIZE, fOut) != TAG_SIZE) {
        fprintf(stderr, "Erreur fwrite tag\n");
        goto cleanup;
    }

    ret = true;

cleanup:
    if (fIn) fclose(fIn);
    if (fOut) fclose(fOut);
    if (ctx) EVP_CIPHER_CTX_free(ctx);
    secure_memzero(key, KEY_SIZE);
    return ret;
}

bool secure_aes_gcm_decrypt_file(const char *inFile, const char *outFile, const char *pass)
{
    bool ret = false;
    FILE *fIn = NULL, *fOut = NULL;
    EVP_CIPHER_CTX *ctx = NULL;

    unsigned char salt[SALT_SIZE];
    unsigned char nonce[NONCE_SIZE];
    unsigned char key[KEY_SIZE];
    unsigned char tag[TAG_SIZE];

    // 1) Ouvrir
    fIn = fopen(inFile, "rb");
    if (!fIn) {
        perror("fopen inFile");
        goto cleanup;
    }
    fOut = fopen(outFile, "wb");
    if (!fOut) {
        perror("fopen outFile");
        goto cleanup;
    }

    // 2) Lire SALT + NONCE
    if (fread(salt, 1, SALT_SIZE, fIn) != SALT_SIZE) {
        fprintf(stderr, "Erreur lecture salt (fichier invalide?)\n");
        goto cleanup;
    }
    if (fread(nonce, 1, NONCE_SIZE, fIn) != NONCE_SIZE) {
        fprintf(stderr, "Erreur lecture nonce (fichier invalide?)\n");
        goto cleanup;
    }

    // On va avoir besoin de lire la fin pour extraire le tag
    // => Mesurer la taille totale, calculer la taille ciphertext = total - (16 salt + 12 nonce + 16 tag).
    fseek(fIn, 0, SEEK_END);
    long fileSize = ftell(fIn);
    long cipherSize = fileSize - (SALT_SIZE + NONCE_SIZE + TAG_SIZE);
    if (cipherSize < 0) {
        fprintf(stderr, "Fichier trop petit/invalide pour GCM.\n");
        goto cleanup;
    }
    // Revenir à la position après SALT + NONCE
    fseek(fIn, SALT_SIZE + NONCE_SIZE, SEEK_SET);

    // 3) Dériver la clé
    if (!derive_key(pass, salt, key, KEY_SIZE)) {
        fprintf(stderr, "Erreur derive_key\n");
        goto cleanup;
    }

    // 4) Initialiser le contexte AES-GCM
    ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Erreur EVP_CIPHER_CTX_new\n");
        goto cleanup;
    }
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) {
        fprintf(stderr, "Erreur EVP_DecryptInit_ex\n");
        goto cleanup;
    }
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, NONCE_SIZE, NULL)) {
        fprintf(stderr, "Erreur EVP_CTRL_GCM_SET_IVLEN\n");
        goto cleanup;
    }
    if (1 != EVP_DecryptInit_ex(ctx, NULL, NULL, key, nonce)) {
        fprintf(stderr, "Erreur EVP_DecryptInit_ex (key,nonce)\n");
        goto cleanup;
    }

    // 5) Lire le ciphertext (cipherSize octets), en laissant les 16 octets finaux pour le tag
    // On va décrypter au fur et à mesure
    unsigned char bufferIn[4096];
    unsigned char bufferOut[4096 + EVP_MAX_BLOCK_LENGTH];
    long bytesToRead = cipherSize;
    int outLen = 0;

    while (bytesToRead > 0) {
        size_t chunk = (bytesToRead > (long)sizeof(bufferIn)) ? sizeof(bufferIn) : (size_t)bytesToRead;
        size_t readSize = fread(bufferIn, 1, chunk, fIn);
        if (readSize != chunk) {
            fprintf(stderr, "Erreur lecture ciphertext\n");
            goto cleanup;
        }
        bytesToRead -= chunk;

        if (1 != EVP_DecryptUpdate(ctx, bufferOut, &outLen, bufferIn, (int)readSize)) {
            fprintf(stderr, "Erreur EVP_DecryptUpdate (tag invalide? passphrase fausse?)\n");
            goto cleanup;
        }
        if (outLen > 0) {
            if (fwrite(bufferOut, 1, outLen, fOut) != (size_t)outLen) {
                fprintf(stderr, "Erreur fwrite (décryptage)\n");
                goto cleanup;
            }
        }
    }

    // 6) Lire le tag final (16 octets)
    if (fread(tag, 1, TAG_SIZE, fIn) != TAG_SIZE) {
        fprintf(stderr, "Erreur lecture tag\n");
        goto cleanup;
    }

    // Informer OpenSSL du tag à vérifier
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_SIZE, tag)) {
        fprintf(stderr, "Erreur EVP_CTRL_GCM_SET_TAG\n");
        goto cleanup;
    }

    // 7) Finaliser -> si tag incorrect, la fonction retourne une erreur
    if (1 != EVP_DecryptFinal_ex(ctx, bufferOut, &outLen)) {
        fprintf(stderr, "Tag invalide ou passphrase incorrecte.\n");
        goto cleanup;
    }
    if (outLen > 0) {
        fwrite(bufferOut, 1, outLen, fOut);
    }

    ret = true;

cleanup:
    if (fIn) fclose(fIn);
    if (fOut) fclose(fOut);
    if (ctx) EVP_CIPHER_CTX_free(ctx);
    secure_memzero(key, KEY_SIZE);
    return ret;
}

#ifndef SECURE_AES_GCM_H
#define SECURE_AES_GCM_H

#include <stdbool.h>
#include <stddef.h>

/**
 * Chiffre un fichier en AES-256-GCM, dérive la clé via PBKDF2.
 * Format du fichier chiffré :
 *   [16 bytes SALT | 12 bytes NONCE | ciphertext... | 16 bytes TAG]
 *
 * @param inFile  : fichier source clair
 * @param outFile : fichier résultat chiffré
 * @param pass    : passphrase (servant à dériver la clé)
 * @return true si succès, false si erreur
 */
bool secure_aes_gcm_encrypt_file(const char *inFile, const char *outFile, const char *pass);

/**
 * Déchiffre un fichier en AES-256-GCM, dérive la clé via PBKDF2.
 * Attend le format :
 *   [16 bytes SALT | 12 bytes NONCE | ciphertext... | 16 bytes TAG]
 *
 * @param inFile  : fichier chiffré
 * @param outFile : fichier résultat clair
 * @param pass    : passphrase
 * @return true si succès, false si erreur (tag invalide, passphrase fausse, etc.)
 */
bool secure_aes_gcm_decrypt_file(const char *inFile, const char *outFile, const char *pass);

/**
 * Nettoyage mémoire sécurisé (ex: OPENSSL_cleanse).
 * @param ptr : buffer
 * @param len : taille en octets
 */
void secure_memzero(void *ptr, size_t len);

#endif

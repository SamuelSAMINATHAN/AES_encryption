#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/secure_aes_gcm.h"

// Usage: ./encryptor-cli [encrypt|decrypt] <inFile> <outFile> <passphrase>
int main(int argc, char *argv[])
{
    if (argc != 5) {
        fprintf(stderr, "Usage: %s [encrypt|decrypt] <in> <out> <pass>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *mode = argv[1];
    const char *inFile = argv[2];
    const char *outFile = argv[3];
    const char *pass = argv[4];

    if (strcmp(mode, "encrypt") == 0) {
        if (secure_aes_gcm_encrypt_file(inFile, outFile, pass)) {
            printf("Chiffrement OK\n");
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "Chiffrement échoué\n");
            return EXIT_FAILURE;
        }
    } else if (strcmp(mode, "decrypt") == 0) {
        if (secure_aes_gcm_decrypt_file(inFile, outFile, pass)) {
            printf("Déchiffrement OK\n");
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "Déchiffrement échoué (tag invalide?)\n");
            return EXIT_FAILURE;
        }
    } else {
        fprintf(stderr, "Mode inconnu: %s\n", mode);
        return EXIT_FAILURE;
    }
}

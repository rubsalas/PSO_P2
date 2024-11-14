#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi_utils.h"
#include "word_counter.h"
#include "crypto_utils.h"

int main(int argc, char** argv) {
    int rank, size;
    mpi_initialize(&argc, &argv, &rank, &size);

    WordCount* globalWordCounts = NULL;
    int globalCountSize = 0;

    if (rank == 0) {
        distribute_text("texts/quijote.txt", size);
    } else {
        receive_and_count_fragment(rank);
    }

    if (rank == 0) {
        consolidate_counts(size, &globalWordCounts, &globalCountSize);

        // Encontrar la palabra más repetida
        qsort(globalWordCounts, globalCountSize, sizeof(WordCount), compareWordCount);
        printf("Palabra más repetida: %s, Frecuencia: %d\n", globalWordCounts[0].word, globalWordCounts[0].frequency);

        // Encriptar y desencriptar el resultado
        char textToEncrypt[128];
        snprintf(textToEncrypt, sizeof(textToEncrypt), "%s: %d", globalWordCounts[0].word, globalWordCounts[0].frequency);

        unsigned char key[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        unsigned char iv[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        unsigned char encryptedText[128];
        unsigned char decryptedText[128];

        int encryptedTextLength = encrypt((unsigned char*)textToEncrypt, strlen(textToEncrypt), key, iv, encryptedText);
        
        printf("Texto encriptado: ");
        for (int i = 0; i < encryptedTextLength; ++i) {
            printf("%02x", encryptedText[i]);
        }
        printf("\n");

        int decryptedTextLength = decrypt(encryptedText, encryptedTextLength, key, iv, decryptedText);
        decryptedText[decryptedTextLength] = '\0';
        printf("Texto desencriptado: %s\n", decryptedText);

        free(globalWordCounts);
    }

    mpi_finalize();
    return 0;
}

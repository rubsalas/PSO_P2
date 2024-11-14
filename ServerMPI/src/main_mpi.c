#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "word_counter.h"
#include "crypto_utils.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtener el rango del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtener el numero total de procesos

    if (size < 2) {
        fprintf(stderr, "Este programa requiere al menos 2 procesos.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Variables para el conteo de palabras
    WordCount* globalWordCounts = NULL;
    int globalCountSize = 0;

    // Nodo maestro lee y divide el texto
    if (rank == 0) {
        // Leer archivo de texto
        FILE *file = fopen("texts/quijote.txt", "r");
        if (file == NULL) {
            fprintf(stderr, "No se puede abrir el archivo\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fseek(file, 0, SEEK_END);
        long fileLength = ftell(file);
        rewind(file);

        char *text = (char*)malloc(fileLength + 1);
        fread(text, 1, fileLength, file);
        text[fileLength] = '\0';
        fclose(file);

        // Dividir el texto en fragmentos y enviarlos a los nodos
        long fragmentSize = fileLength / (size - 1); // Excluir al maestro
        for (int i = 1; i < size; i++) {
            long start = (i - 1) * fragmentSize;
            long end = (i == size - 1) ? fileLength : i * fragmentSize;

            // Ajustar fragmento para finalizar en un delimitador
            while (end < fileLength && text[end] != ' ' && text[end] != '\n') end++;
            long adjustedFragmentSize = end - start;

            // Enviar fragmento al nodo trabajador
            MPI_Send(&adjustedFragmentSize, 1, MPI_LONG, i, 0, MPI_COMM_WORLD);
            MPI_Send(text + start, adjustedFragmentSize, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
        free(text);
    } else {
        // Nodo trabajador recibe el fragmento
        long adjustedFragmentSize;
        MPI_Recv(&adjustedFragmentSize, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        char* fragment = (char*)malloc((adjustedFragmentSize + 1) * sizeof(char));
        MPI_Recv(fragment, adjustedFragmentSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        fragment[adjustedFragmentSize] = '\0';

        // Procesar fragmento para contar palabras
        WordCount* localWordCounts;ó
        int localCountSize;
        countWordsAndFrequencies(fragment, adjustedFragmentSize, &localWordCounts, &localCountSize);

        // Enviar conteo de palabras al maestro
        MPI_Send(&localCountSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(localWordCounts, localCountSize * sizeof(WordCount), MPI_BYTE, 0, 0, MPI_COMM_WORLD);

        free(fragment);
        free(localWordCounts);
    }

    // Consolidar resultados en el nodo maestro
    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            int localCountSize;
            MPI_Recv(&localCountSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            WordCount* localWordCounts = (WordCount*)malloc(localCountSize * sizeof(WordCount));
            MPI_Recv(localWordCounts, localCountSize * sizeof(WordCount), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Consolidar conteos locales en el conteo global
            for (int j = 0; j < localCountSize; j++) {
                int found = 0;
                for (int k = 0; k < globalCountSize; k++) {
                    if (strcmp(globalWordCounts[k].word, localWordCounts[j].word) == 0) {
                        globalWordCounts[k].frequency += localWordCounts[j].frequency;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    globalCountSize++;
                    globalWordCounts = realloc(globalWordCounts, globalCountSize * sizeof(WordCount));
                    globalWordCounts[globalCountSize - 1] = localWordCounts[j];
                }
            }
            free(localWordCounts);
        }

        // Encontrar la palabra mas repetida
        qsort(globalWordCounts, globalCountSize, sizeof(WordCount), compareWordCount);
        printf("Palabra mas repetida: %s, Frecuencia: %d\n", globalWordCounts[0].word, globalWordCounts[0].frequency);

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

    MPI_Finalize();
    return 0;
}

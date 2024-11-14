#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi_utils.h"
#include "word_counter.h"

// Función: mpi_initialize
// -----------------------
// Inicializa MPI y obtiene el rango y tamaño de los procesos.
// Si hay menos de dos procesos, el programa muestra un error y termina.
//
// Parámetros:
//    argc - Número de argumentos de la línea de comandos
//    argv - Arreglo de argumentos de la línea de comandos
//    rank - Puntero donde se almacenará el rango del proceso actual
//    size - Puntero donde se almacenará el número total de procesos
void mpi_initialize(int *argc, char ***argv, int *rank, int *size) {
    MPI_Init(argc, argv);                     // Inicializa MPI
    MPI_Comm_rank(MPI_COMM_WORLD, rank);      // Obtiene el rango del proceso actual
    MPI_Comm_size(MPI_COMM_WORLD, size);      // Obtiene el número total de procesos

    // Verifica que haya al menos dos procesos (un maestro y al menos un trabajador)
    if (*size < 2) {
        fprintf(stderr, "Este programa requiere al menos 2 procesos.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);         // Termina el programa si no hay suficientes procesos
    }
}

// Función: distribute_text
// ------------------------
// El proceso maestro lee un archivo de texto y lo divide en fragmentos,
// luego envía cada fragmento a un proceso trabajador.
//
// Parámetros:
//    file_path - Ruta del archivo de texto que se va a distribuir
//    size - Número total de procesos (incluye al maestro)
void distribute_text(const char *file_path, int size) {
    FILE *file = fopen(file_path, "r");       // Abre el archivo en modo lectura
    if (file == NULL) {
        fprintf(stderr, "No se puede abrir el archivo\n");
        MPI_Abort(MPI_COMM_WORLD, 1);         // Termina el programa si no se puede abrir el archivo
    }

    fseek(file, 0, SEEK_END);                 // Mueve el cursor al final del archivo
    long fileLength = ftell(file);            // Obtiene el tamaño del archivo
    rewind(file);                             // Regresa el cursor al inicio del archivo

    char *text = (char*)malloc(fileLength + 1); // Reserva memoria para el contenido del archivo
    fread(text, 1, fileLength, file);           // Lee el contenido del archivo
    text[fileLength] = '\0';                    // Añade el terminador de cadena
    fclose(file);                               // Cierra el archivo

    // Calcula el tamaño del fragmento a enviar a cada trabajador
    long fragmentSize = fileLength / (size - 1); // Excluye al maestro
    for (int i = 1; i < size; i++) {
        long start = (i - 1) * fragmentSize;
        long end = (i == size - 1) ? fileLength : i * fragmentSize;

        // Ajusta el fragmento para terminar en un delimitador
        while (end < fileLength && text[end] != ' ' && text[end] != '\n') end++;
        long adjustedFragmentSize = end - start;

        // Envía el tamaño del fragmento y el fragmento al proceso trabajador
        MPI_Send(&adjustedFragmentSize, 1, MPI_LONG, i, 0, MPI_COMM_WORLD);
        MPI_Send(text + start, adjustedFragmentSize, MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }
    free(text);                                // Libera la memoria del texto completo
}

// Función: receive_and_count_fragment
// -----------------------------------
// Recibe un fragmento de texto en un proceso trabajador y cuenta las palabras en él.
// Luego envía el conteo de palabras de vuelta al proceso maestro.
//
// Parámetros:
//    rank - Rango del proceso trabajador que recibe el fragmento
void receive_and_count_fragment(int rank) {
    long adjustedFragmentSize;
    
    // Recibe el tamaño del fragmento desde el proceso maestro
    MPI_Recv(&adjustedFragmentSize, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    char* fragment = (char*)malloc((adjustedFragmentSize + 1) * sizeof(char));
    
    // Recibe el fragmento de texto y lo almacena en 'fragment'
    MPI_Recv(fragment, adjustedFragmentSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    fragment[adjustedFragmentSize] = '\0';     // Añade el terminador de cadena

    // Cuenta las palabras en el fragmento recibido
    WordCount* localWordCounts;
    int localCountSize;
    countWordsAndFrequencies(fragment, adjustedFragmentSize, &localWordCounts, &localCountSize);

    // Envía el conteo de palabras y su tamaño al proceso maestro
    MPI_Send(&localCountSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Send(localWordCounts, localCountSize * sizeof(WordCount), MPI_BYTE, 0, 0, MPI_COMM_WORLD);

    free(fragment);                            // Libera la memoria del fragmento
    free(localWordCounts);                     // Libera la memoria del conteo de palabras
}

// Función: consolidate_counts
// ---------------------------
// Consolida los conteos de palabras recibidos de todos los procesos trabajadores en el proceso maestro.
// Si una palabra ya existe en el conteo global, suma su frecuencia; si no, la añade al conteo.
//
// Parámetros:
//    size - Número total de procesos (incluye al maestro)
//    globalWordCounts - Puntero al arreglo de conteo global de palabras
//    globalCountSize - Puntero al número total de palabras únicas en el conteo global
void consolidate_counts(int size, WordCount **globalWordCounts, int *globalCountSize) {
    for (int i = 1; i < size; i++) {
        int localCountSize;
        
        // Recibe el tamaño del conteo local desde el proceso trabajador
        MPI_Recv(&localCountSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Reserva memoria para recibir el conteo local de palabras
        WordCount* localWordCounts = (WordCount*)malloc(localCountSize * sizeof(WordCount));
        MPI_Recv(localWordCounts, localCountSize * sizeof(WordCount), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Consolidar conteos locales en el conteo global
        for (int j = 0; j < localCountSize; j++) {
            int found = 0;
            for (int k = 0; k < *globalCountSize; k++) {
                // Si la palabra ya está en el conteo global, suma la frecuencia
                if (strcmp((*globalWordCounts)[k].word, localWordCounts[j].word) == 0) {
                    (*globalWordCounts)[k].frequency += localWordCounts[j].frequency;
                    found = 1;
                    break;
                }
            }
            // Si la palabra no está en el conteo global, la añade
            if (!found) {
                (*globalCountSize)++;
                *globalWordCounts = realloc(*globalWordCounts, (*globalCountSize) * sizeof(WordCount));
                (*globalWordCounts)[*globalCountSize - 1] = localWordCounts[j];
            }
        }
        free(localWordCounts);                 // Libera la memoria del conteo local
    }
}

// Función: mpi_finalize
// ---------------------
// Finaliza el entorno de MPI, asegurando que todos los procesos terminen correctamente.
void mpi_finalize() {
    MPI_Finalize();
}

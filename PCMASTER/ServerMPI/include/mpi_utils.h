#ifndef MPI_UTILS_H
#define MPI_UTILS_H

#include <mpi.h>
#include "word_counter.h"

// Función: mpi_initialize
// -----------------------
// Inicializa el entorno de MPI, obteniendo el rango del proceso actual y el número total de procesos.
// 
// Parámetros:
//    argc - Puntero al número de argumentos
//    argv - Puntero al arreglo de argumentos
//    rank - Puntero a la variable donde se almacenará el rango del proceso
//    size - Puntero a la variable donde se almacenará el número total de procesos
void mpi_initialize(int *argc, char ***argv, int *rank, int *size);

// Función: distribute_text
// ------------------------
// Distribuye el contenido de un archivo de texto desde el proceso maestro a los procesos trabajadores.
// 
// Parámetros:
//    file_path - Ruta del archivo de texto que se va a distribuir
//    size - Número total de procesos (incluyendo el maestro)
void distribute_text(const char *file_path, int size);

// Función: receive_and_count_fragment
// -----------------------------------
// Recibe un fragmento de texto en un proceso trabajador y cuenta las palabras en él.
// 
// Parámetros:
//    rank - Rango del proceso trabajador que recibe el fragmento de texto
void receive_and_count_fragment(int rank);

// Función: consolidate_counts
// ---------------------------
// Consolida los conteos de palabras de todos los procesos trabajadores en el proceso maestro.
// 
// Parámetros:
//    size - Número total de procesos (incluyendo el maestro)
//    globalWordCounts - Puntero a un arreglo que almacenará el conteo de palabras global
//    globalCountSize - Puntero a la variable que almacenará el número total de palabras únicas contadas
void consolidate_counts(int size, WordCount **globalWordCounts, int *globalCountSize);

// Función: mpi_finalize
// ---------------------
// Finaliza el entorno de MPI, asegurando que todos los procesos terminen correctamente.
void mpi_finalize();

#endif // MPI_UTILS_H

#ifndef WORD_COUNTER_H
#define WORD_COUNTER_H

#include <openssl/evp.h>
#include <openssl/err.h>

// Estructura para almacenar una palabra y su frecuencia
typedef struct {
    char word[100];  // Palabra con una longitud maxima de 100 caracteres
    int frequency;   // Frecuencia de la palabra
} WordCount;

/*
 * Funcion: countWordsAndFrequencies
 * ----------------------
 * Cuenta las palabras en un fragmento de texto y almacena sus frecuencias en un array.
 * 
 * Parametros: 
 *    text - El texto a procesar
 *    length - Longitud del texto
 *    wordCounts - Puntero a un array de estructuras WordCount
 *    countSize - Puntero a un entero que representa el numero de palabras unicas encontradas
 * 
 * Retorna:
 *    Entero que representa el numero total de palabras contadas en el texto
 */
int countWordsAndFrequencies(const char* text, int length, WordCount** wordCounts, int* countSize);

/*
 * Funcion: compareWordCount
 * ----------------------
 * Funcion de comparacion para ordenar las estructuras WordCount por frecuencia en orden descendente.
 * 
 * Parametros:
 *    a - Puntero a la primera estructura WordCount
 *    b - Puntero a la segunda estructura WordCount
 * 
 * Retorna:
 *    Un entero menor, igual o mayor a cero si la frecuencia de a es menor, igual o mayor
 *    que la frecuencia de b, respectivamente.
 */
int compareWordCount(const void* a, const void* b);

#endif // WORD_COUNTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "word_counter.h"

/*
 * Funcion: countWordsAndFrequencies
 * ----------------------
 * Cuenta las palabras en un fragmento de texto y almacena sus frecuencias en un
 * array dinamico. Redimensiona el array segun sea necesario para acomodar palabras unicas.
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
int countWordsAndFrequencies(const char* text, int length, WordCount** wordCounts, int* countSize) {
    char tempWord[100];           // Almacenamiento temporal para cada palabra
    int wordLength = 0;           // Longitud actual de la palabra que se esta procesando
    int capacity = 10;            // Capacidad inicial para el array wordCounts
    *wordCounts = (WordCount*)malloc(capacity * sizeof(WordCount));
    *countSize = 0;

    for (int i = 0; i < length; ++i) {
        if (isalpha(text[i])) {   // Si el caracter es alfabetico
            if (wordLength < 99) {
                tempWord[wordLength++] = tolower(text[i]); // Convertir a minuscula y añadir a tempWord
            }
        } else {                  // Si encontramos un caracter no alfabetico
            if (wordLength > 0) { // Se ha formado una palabra completa
                tempWord[wordLength] = '\0';
                int found = 0;
                
                // Verificar si la palabra ya esta en wordCounts
                for (int j = 0; j < *countSize; ++j) {
                    if (strcmp((*wordCounts)[j].word, tempWord) == 0) {
                        (*wordCounts)[j].frequency++;
                        found = 1;
                        break;
                    }
                }
                
                // Si la palabra no se encuentra, añadirla a wordCounts
                if (!found) {
                    if (*countSize >= capacity) {
                        capacity *= 2; // Duplicar la capacidad si es necesario
                        *wordCounts = (WordCount*)realloc(*wordCounts, capacity * sizeof(WordCount));
                    }
                    strcpy((*wordCounts)[*countSize].word, tempWord);
                    (*wordCounts)[*countSize].frequency = 1;
                    (*countSize)++;
                }
                wordLength = 0; // Reiniciar la longitud de la palabra para la siguiente palabra
            }
        }
    }

    // Procesar la ultima palabra si el texto no termina con un delimitador
    if (wordLength > 0) {
        tempWord[wordLength] = '\0';
        int found = 0;
        
        for (int j = 0; j < *countSize; ++j) {
            if (strcmp((*wordCounts)[j].word, tempWord) == 0) {
                (*wordCounts)[j].frequency++;
                found = 1;
                break;
            }
        }
        
        if (!found) {
            if (*countSize >= capacity) {
                capacity *= 2;
                *wordCounts = (WordCount*)realloc(*wordCounts, capacity * sizeof(WordCount));
            }
            strcpy((*wordCounts)[*countSize].word, tempWord);
            (*wordCounts)[*countSize].frequency = 1;
            (*countSize)++;
        }
    }

    return *countSize;
}

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
int compareWordCount(const void* a, const void* b) {
    return ((WordCount*)b)->frequency - ((WordCount*)a)->frequency;
}

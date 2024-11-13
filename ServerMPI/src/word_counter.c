#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "word_counter.h"

int contarPalabrasYFrecuencias(const char* texto, int longitud, ConteoPalabra** conteoPalabras, int* tamañoConteo) {
    char palabraTemporal[100];
    int longitudPalabra = 0;
    int capacidad = 10;
    *conteoPalabras = (ConteoPalabra*)malloc(capacidad * sizeof(ConteoPalabra));
    *tamañoConteo = 0;

    for (int i = 0; i < longitud; ++i) {
        if (isalpha(texto[i])) {
            if (longitudPalabra < 99) {
                palabraTemporal[longitudPalabra++] = tolower(texto[i]);
            }
        } else {
            if (longitudPalabra > 0) {
                palabraTemporal[longitudPalabra] = '\0';
                int encontrado = 0;
                for (int j = 0; j < *tamañoConteo; ++j) {
                    if (strcmp((*conteoPalabras)[j].palabra, palabraTemporal) == 0) {
                        (*conteoPalabras)[j].frecuencia++;
                        encontrado = 1;
                        break;
                    }
                }
                if (!encontrado) {
                    if (*tamañoConteo >= capacidad) {
                        capacidad *= 2;
                        *conteoPalabras = (ConteoPalabra*)realloc(*conteoPalabras, capacidad * sizeof(ConteoPalabra));
                    }
                    strcpy((*conteoPalabras)[*tamañoConteo].palabra, palabraTemporal);
                    (*conteoPalabras)[*tamañoConteo].frecuencia = 1;
                    (*tamañoConteo)++;
                }
                longitudPalabra = 0;
            }
        }
    }

    if (longitudPalabra > 0) {
        palabraTemporal[longitudPalabra] = '\0';
        int encontrado = 0;
        for (int j = 0; j < *tamañoConteo; ++j) {
            if (strcmp((*conteoPalabras)[j].palabra, palabraTemporal) == 0) {
                (*conteoPalabras)[j].frecuencia++;
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) {
            if (*tamañoConteo >= capacidad) {
                capacidad *= 2;
                *conteoPalabras = (ConteoPalabra*)realloc(*conteoPalabras, capacidad * sizeof(ConteoPalabra));
            }
            strcpy((*conteoPalabras)[*tamañoConteo].palabra, palabraTemporal);
            (*conteoPalabras)[*tamañoConteo].frecuencia = 1;
            (*tamañoConteo)++;
        }
    }

    return *tamañoConteo;
}

int compararConteoPalabra(const void* a, const void* b) {
    return ((ConteoPalabra*)b)->frecuencia - ((ConteoPalabra*)a)->frecuencia;
}


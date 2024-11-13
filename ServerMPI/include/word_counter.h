#ifndef WORD_COUNTER_H
#define WORD_COUNTER_H

#include <openssl/evp.h>
#include <openssl/err.h>

typedef struct {
    char palabra[100];
    int frecuencia;
} ConteoPalabra;

int contarPalabrasYFrecuencias(const char* texto, int longitud, ConteoPalabra** conteoPalabras, int* tamañoConteo);
int compararConteoPalabra(const void* a, const void* b);

#endif // WORD_COUNTER_H


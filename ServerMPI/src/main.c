#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "word_counter.h"
#include "crypto_utils.h"

int main() {
    // Abrir archivo de texto
    FILE *archivo = fopen("texts/quijote.txt", "r");
    if (archivo == NULL) {
        fprintf(stderr, "No se puede abrir el archivo\n");
        return 1;
    }

    fseek(archivo, 0, SEEK_END);
    long longitudArchivo = ftell(archivo);
    rewind(archivo);

    char *texto = (char*)malloc(longitudArchivo + 1);
    fread(texto, 1, longitudArchivo, archivo);
    texto[longitudArchivo] = '\0';
    fclose(archivo);

    // Contar palabras y frecuencias
    ConteoPalabra *conteoPalabras;
    int tamañoConteo;
    contarPalabrasYFrecuencias(texto, longitudArchivo, &conteoPalabras, &tamañoConteo);

    qsort(conteoPalabras, tamañoConteo, sizeof(ConteoPalabra), compararConteoPalabra);

    // Mostrar palabra más repetida y su frecuencia
    printf("Palabra más repetida: %s, Frecuencia: %d\n", conteoPalabras[0].palabra, conteoPalabras[0].frecuencia);

    // Construir el string para encriptar
    char textoParaEncriptar[128];
    snprintf(textoParaEncriptar, sizeof(textoParaEncriptar), "%s: %d", conteoPalabras[0].palabra, conteoPalabras[0].frecuencia);

    // Imprimir el texto que será encriptado
    printf("Texto para encriptar: %s\n", textoParaEncriptar);

    // Configurar claves y vectores de inicialización
    unsigned char clave[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    unsigned char iv[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    unsigned char textoCifrado[128];
    unsigned char textoDescifrado[128];

    // Encriptar el texto
    int longitudTextoCifrado = encriptar((unsigned char*)textoParaEncriptar, strlen(textoParaEncriptar), clave, iv, textoCifrado);

    // Imprimir el texto encriptado en formato hexadecimal
    printf("Texto encriptado: ");
    for (int i = 0; i < longitudTextoCifrado; ++i) {
        printf("%02x", textoCifrado[i]);
    }
    printf("\n");

    // Desencriptar el texto
    int longitudTextoDescifrado = desencriptar(textoCifrado, longitudTextoCifrado, clave, iv, textoDescifrado);
    textoDescifrado[longitudTextoDescifrado] = '\0';  // Asegurarse de que el texto descifrado esté terminado en nulo

    // Imprimir el texto desencriptado
    printf("Texto desencriptado: %s\n", textoDescifrado);

    free(texto);
    free(conteoPalabras);

    return 0;
}


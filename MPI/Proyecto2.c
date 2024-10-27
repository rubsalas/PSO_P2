#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <openssl/evp.h>
#include <openssl/err.h>
// #include <mpi.h>
// #include "Driver/driver_lib.h"

// Estructura para almacenar palabras y sus frecuencias
typedef struct {
    char palabra[100];
    int frecuencia;
} ConteoPalabra;

void manejarErrores(void) {
    ERR_print_errors_fp(stderr);
    abort();
}

// Función para contar palabras en una porción de texto y almacenar sus frecuencias
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

    // Contar la última palabra si el texto no termina con un delimitador
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
}

int compararConteoPalabra(const void* a, const void* b) {
    return ((ConteoPalabra*)b)->frecuencia - ((ConteoPalabra*)a)->frecuencia;
}

// Función para encriptar datos
int encriptar(unsigned char *textoPlano, int longitudTexto, unsigned char *clave, unsigned char *iv, unsigned char *textoCifrado) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int longitudTextoCifrado;

    if (!(ctx = EVP_CIPHER_CTX_new())) manejarErrores();
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, clave, iv)) manejarErrores(); // Cambiado a AES-128
    if (1 != EVP_EncryptUpdate(ctx, textoCifrado, &len, textoPlano, longitudTexto)) manejarErrores();
    longitudTextoCifrado = len;
    if (1 != EVP_EncryptFinal_ex(ctx, textoCifrado + len, &len)) manejarErrores ();
    longitudTextoCifrado += len;
    EVP_CIPHER_CTX_free(ctx);
    return longitudTextoCifrado;
}

// Función para desencriptar datos
int desencriptar(unsigned char *textoCifrado, int longitudTextoCifrado, unsigned char *clave, unsigned char *iv, unsigned char *textoPlano) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int longitudTextoPlano;

    if (!(ctx = EVP_CIPHER_CTX_new())) manejarErrores();
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, clave, iv)) manejarErrores(); // Cambiado a AES-128
    if (1 != EVP_DecryptUpdate(ctx, textoPlano, &len, textoCifrado, longitudTextoCifrado)) manejarErrores();
    longitudTextoPlano = len;
    if (1 != EVP_DecryptFinal_ex(ctx, textoPlano + len, &len)) manejarErrores();
    longitudTextoPlano += len;
    EVP_CIPHER_CTX_free(ctx);
    return longitudTextoPlano;
}

// Función para inicializar el driver de dispositivo
void inicializarDriver() {
    // Inicializar el driver de dispositivo para la pantalla LCD y el buzzer
    // printf("Inicializando driver de dispositivo...\n");
    // // wiringPiSetup();
    // // pinMode(LCD_RS, OUTPUT);
    // // pinMode(LCD_EN, OUTPUT);
    // // pinMode(LCD_D4, OUTPUT);
    // // pinMode(LCD_D5, OUTPUT);
    // // pinMode(LCD_D6, OUTPUT);
    // // pinMode(LCD_D7, OUTPUT);
    // // pinMode(BUZZER_PIN, OUTPUT);
}

// Función para mostrar texto en la pantalla LCD
void mostrarTextoEnPantallaLCD(const char* texto) {
    // printf("Mostrando texto en pantalla LCD: %s\n", texto);
    // // lcdClear();
    // // lcdPrint(texto);
}

// Función para emitir un sonido con el buzzer
void emitirSonidoConBuzzer() {
    // printf("Emitiendo sonido con buzzer...\n");
    // // digitalWrite(BUZZER_PIN, HIGH);
    // // delay(1000);
    // // digitalWrite(BUZZER_PIN, LOW);
}

int main() {
    // MPI_Init(NULL, NULL); // Inicializar MPI
    // int rank, size;
    // MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Abrir archivo de texto
    FILE *archivo;
    archivo = fopen("quijote.txt", "r");
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

    // Encriptar resultados
    unsigned char clave[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    unsigned char iv[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    unsigned char textoCifrado[longitudArchivo];
    int longitudTextoCifrado = encriptar((unsigned char*)texto, longitudArchivo, clave, iv, textoCifrado);

    // Mostrar texto cifrado
    // printf("Texto cifrado: ");
    // for (int i = 0; i < longitudTextoCifrado; ++i) {
    //     printf("%02x", textoCifrado[i]);
    // }
    // printf("\n");

    // // Enviar resultados a nodo maestro mediante MPI
    // if (rank != 0) {
    //     MPI_Send(conteoPalabras, tamañoConteo, MPI_INT, 0, 0, MPI_COMM_WORLD);
    // } else {
    //     ConteoPalabra *conteoPalabrasRecibidos;
    //     int tamañoConteoRecibido;
    //     for (int i = 1; i < size; ++i) {
    //         MPI_Recv(&tamañoConteoRecibido, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //         conteoPalabrasRecibidos = (ConteoPalabra*)malloc(tamañoConteoRecibido * sizeof(ConteoPalabra));
    //         MPI_Recv(conteoPalabrasRecibidos, tamañoConteoRecibido, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //         // Desencriptar resultados
    //         unsigned char textoPlanoRecibido[longitudArchivo];
    //         int longitudTextoPlanoRecibido = desencriptar(textoCifrado, longitudTextoCifrado, clave, iv, textoPlanoRecibido);
    //         // Consolidar resultados y encontrar palabra más repetida
    //         // ...
    //         free(conteoPalabrasRecibidos);
    //     }
    // }

    // // Inicializar el driver de dispositivo
    // inicializarDriver();

    // // Mostrar resultados en pantalla LCD
    // mostrarTextoEnPantallaLCD("La palabra más repetida es: ...");

    // // Emitir sonido con buzzer
    // emitirSonidoConBuzzer();

    // // Enviar palabra más repetida al driver
    // int fd = open_serial_port("/dev/ttyACM0");
    // int bytes_written = send_word(fd, ("%s\n", conteoPalabras[0].palabra));

    // close_serial_port(fd);

    free(texto);
    free(conteoPalabras);

    // MPI_Finalize(); // Finalizar MPI

    return 0;
}

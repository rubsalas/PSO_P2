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
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtener el número total de procesos

    if (size < 2) {
        fprintf(stderr, "Este programa requiere al menos 2 procesos.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Nodo maestro lee y divide el texto
    ConteoPalabra* conteoPalabrasGlobal = NULL;
    int tamañoConteoGlobal = 0;
    if (rank == 0) {
        // Leer archivo de texto
        FILE *archivo = fopen("texts/quijote.txt", "r");
        if (archivo == NULL) {
            fprintf(stderr, "No se puede abrir el archivo\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fseek(archivo, 0, SEEK_END);
        long longitudArchivo = ftell(archivo);
        rewind(archivo);

        char *texto = (char*)malloc(longitudArchivo + 1);
        fread(texto, 1, longitudArchivo, archivo);
        texto[longitudArchivo] = '\0';
        fclose(archivo);

        // Dividir el texto en fragmentos y enviarlos a los nodos
        long fragmentoTamaño = longitudArchivo / (size - 1); // Excluir al maestro
        for (int i = 1; i < size; i++) {
            long inicio = (i - 1) * fragmentoTamaño;
            long fin = (i == size - 1) ? longitudArchivo : i * fragmentoTamaño;

            // Ajustar fragmento para finalizar en un delimitador
            while (fin < longitudArchivo && texto[fin] != ' ' && texto[fin] != '\n') fin++;
            long tamañoFragmento = fin - inicio;

            // Enviar fragmento al nodo trabajador
            MPI_Send(&tamañoFragmento, 1, MPI_LONG, i, 0, MPI_COMM_WORLD);
            MPI_Send(texto + inicio, tamañoFragmento, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
        free(texto);
    } else {
        // Nodo trabajador recibe el fragmento
        long tamañoFragmento;
        MPI_Recv(&tamañoFragmento, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        char* fragmento = (char*)malloc((tamañoFragmento + 1) * sizeof(char));
        MPI_Recv(fragmento, tamañoFragmento, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        fragmento[tamañoFragmento] = '\0';

        // Procesar fragmento para contar palabras
        ConteoPalabra* conteoPalabrasLocal;
        int tamañoConteoLocal;
        contarPalabrasYFrecuencias(fragmento, tamañoFragmento, &conteoPalabrasLocal, &tamañoConteoLocal);

        // Enviar conteo de palabras al maestro
        MPI_Send(&tamañoConteoLocal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(conteoPalabrasLocal, tamañoConteoLocal * sizeof(ConteoPalabra), MPI_BYTE, 0, 0, MPI_COMM_WORLD);

        free(fragmento);
        free(conteoPalabrasLocal);
    }

    // Consolidar resultados en el nodo maestro
    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            int tamañoConteoLocal;
            MPI_Recv(&tamañoConteoLocal, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            ConteoPalabra* conteoPalabrasLocal = (ConteoPalabra*)malloc(tamañoConteoLocal * sizeof(ConteoPalabra));
            MPI_Recv(conteoPalabrasLocal, tamañoConteoLocal * sizeof(ConteoPalabra), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Consolidar conteos locales en el conteo global
            for (int j = 0; j < tamañoConteoLocal; j++) {
                int encontrado = 0;
                for (int k = 0; k < tamañoConteoGlobal; k++) {
                    if (strcmp(conteoPalabrasGlobal[k].palabra, conteoPalabrasLocal[j].palabra) == 0) {
                        conteoPalabrasGlobal[k].frecuencia += conteoPalabrasLocal[j].frecuencia;
                        encontrado = 1;
                        break;
                    }
                }
                if (!encontrado) {
                    tamañoConteoGlobal++;
                    conteoPalabrasGlobal = realloc(conteoPalabrasGlobal, tamañoConteoGlobal * sizeof(ConteoPalabra));
                    conteoPalabrasGlobal[tamañoConteoGlobal - 1] = conteoPalabrasLocal[j];
                }
            }
            free(conteoPalabrasLocal);
        }

        // Encontrar la palabra más repetida
        qsort(conteoPalabrasGlobal, tamañoConteoGlobal, sizeof(ConteoPalabra), compararConteoPalabra);
        printf("Palabra más repetida: %s, Frecuencia: %d\n", conteoPalabrasGlobal[0].palabra, conteoPalabrasGlobal[0].frecuencia);

        // Encriptar y desencriptar el resultado
        char textoParaEncriptar[128];
        snprintf(textoParaEncriptar, sizeof(textoParaEncriptar), "%s: %d", conteoPalabrasGlobal[0].palabra, conteoPalabrasGlobal[0].frecuencia);

        unsigned char clave[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        unsigned char iv[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        unsigned char textoCifrado[128];
        unsigned char textoDescifrado[128];

        int longitudTextoCifrado = encriptar((unsigned char*)textoParaEncriptar, strlen(textoParaEncriptar), clave, iv, textoCifrado);
        
        printf("Texto encriptado: ");
        for (int i = 0; i < longitudTextoCifrado; ++i) {
            printf("%02x", textoCifrado[i]);
        }
        printf("\n");

        int longitudTextoDescifrado = desencriptar(textoCifrado, longitudTextoCifrado, clave, iv, textoDescifrado);
        textoDescifrado[longitudTextoDescifrado] = '\0';
        printf("Texto desencriptado: %s\n", textoDescifrado);

        free(conteoPalabrasGlobal);
    }

    MPI_Finalize();
    return 0;
}


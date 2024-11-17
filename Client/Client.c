#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lcd_buzzer.h>
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <dirent.h> // Para listar archivos en el directorio

#define MAX 1024
#define PORT 8080
#define SA struct sockaddr

// Función para listar los archivos de texto disponibles en el directorio actual
void listar_archivos() {
    DIR *dir;
    struct dirent *ent;
    printf("\n--- Archivos disponibles para enviar ---\n");
    if ((dir = opendir(".")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            // Mostrar solo archivos con extensión ".txt"
            if (strstr(ent->d_name, ".txt") != NULL) {
                printf(" - %s\n", ent->d_name);
            }
        }
        closedir(dir);
    } else {
        perror("No se pudo abrir el directorio");
    }
}

// Función para enviar un archivo al servidor
void enviar_archivo(int sockfd, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    char buffer[MAX];
    size_t bytes_read;

    // Leer y enviar el archivo en bloques
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (write(sockfd, buffer, bytes_read) != bytes_read) {
            perror("Error al enviar datos");
            fclose(file);
            return;
        }
    }

    // Enviar el indicador "EOF" en un mensaje separado
    strcpy(buffer, "EOF");
    write(sockfd, buffer, strlen(buffer));

    printf("Archivo '%s' enviado correctamente.\n", filename);
    fclose(file);
}

// Función para recibir la respuesta del servidor
void recibir_respuesta(int sockfd) {
    char buffer[MAX];
    int bytes_read = read(sockfd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
    buffer[bytes_read] = '\0';
    char lcd[32];
	const char *final = " ";
	lcd[0]= '\0';
    strcat(lcd, buffer);  // Concatenar mensaje
    strcat(lcd, final);
	lcd_write(lcd);
    play_melody();
    beep_buzzer(3);
        printf("Respuesta del servidor:\n%s\n", buffer);
    } else {
        printf("No se recibió respuesta del servidor.\n");
    }
}

int main() {
    if (lcd_buzzer_init() < 0) {
        return -1;
    }
    int sockfd;
    struct sockaddr_in servaddr;

    // Crear el socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket creado exitosamente.\n");

    // Configuración del servidor
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("192.168.100.7");
    servaddr.sin_port = htons(PORT);

    // Conectarse al servidor
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0) {
        perror("Error al conectar al servidor");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Conectado al servidor.\n");

    while (1) {
        listar_archivos();
        printf("\nEscriba el nombre del archivo a enviar o 'salir' para cerrar la conexión: ");

        char input[MAX];
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0'; // Eliminar el salto de línea

        if (strcmp(input, "salir") == 0) {
	    lcd_buzzer_close();
            printf("Cerrando cliente.\n");
            break;
        }

        // Enviar archivo si existe
        if (access(input, F_OK) == 0) { // Verificar si el archivo existe
            enviar_archivo(sockfd, input);
            recibir_respuesta(sockfd);
        } else {
            printf("El archivo '%s' no existe. Intente de nuevo.\n", input);
        }
    }

    // Cerrar el socket
    close(sockfd);
    printf("Conexión cerrada.\n");
    return 0;
}


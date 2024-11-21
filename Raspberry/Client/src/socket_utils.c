#include "socket_utils.h"
#include <arpa/inet.h>  // Para estructuras y funciones de sockets
#include <string.h>     // Para bzero y manejo de cadenas
#include <unistd.h>     // Para read, write y close

/*
 * Función: socket_connect
 * ----------------------
 * Crea un socket y establece una conexión con un servidor especificado
 * mediante la dirección IP y el puerto proporcionados.
 * 
 * Parámetros:
 *    ip - Dirección IP del servidor en formato de cadena
 *    port - Número de puerto del servidor
 * 
 * Retorna:
 *    Descriptor del socket (un valor entero positivo) si tiene éxito,
 *    -1 si ocurre un error al crear o conectar el socket.
 */
int socket_connect(const char *ip, int port) {
    int sockfd;
    struct sockaddr_in servaddr;

    // Crear el socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error al crear el socket"); // Imprimir mensaje de error
        return -1;
    }

    // Configurar los parámetros del servidor
    bzero(&servaddr, sizeof(servaddr));             // Inicializar estructura con ceros
    servaddr.sin_family = AF_INET;                 // Familia de direcciones IPv4
    servaddr.sin_addr.s_addr = inet_addr(ip);      // Convertir IP a formato binario
    servaddr.sin_port = htons(port);               // Convertir puerto a formato de red

    // Intentar conectarse al servidor
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        perror("Error al conectar al servidor");   // Imprimir mensaje de error
        close(sockfd);                             // Cerrar el socket en caso de error
        return -1;
    }

    return sockfd; // Retorna el descriptor del socket si la conexión es exitosa
}

/*
 * Función: socket_send
 * ----------------------
 * Envía datos al servidor a través del socket especificado.
 * 
 * Parámetros:
 *    sockfd - Descriptor del socket por donde se enviarán los datos
 *    data - Puntero a los datos que se desean enviar
 *    size - Tamaño en bytes de los datos a enviar
 * 
 * Retorna:
 *    Número de bytes enviados si tiene éxito,
 *    -1 si ocurre un error durante el envío.
 */
int socket_send(int sockfd, const char *data, size_t size) {
    return write(sockfd, data, size); // Enviar datos al servidor
}

/*
 * Función: socket_receive
 * ----------------------
 * Recibe datos desde el servidor a través del socket especificado.
 * 
 * Parámetros:
 *    sockfd - Descriptor del socket desde donde se recibirán los datos
 *    buffer - Puntero al buffer donde se almacenarán los datos recibidos
 *    size - Tamaño del buffer en bytes
 * 
 * Retorna:
 *    Número de bytes recibidos si tiene éxito,
 *    -1 si ocurre un error durante la recepción.
 * 
 * Nota:
 *    Esta función asegura que la cadena recibida esté terminada en '\0'
 *    para facilitar su manejo como cadena en C.
 */
int socket_receive(int sockfd, char *buffer, size_t size) {
    int bytes_read = read(sockfd, buffer, size - 1); // Leer datos del servidor
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0'; // Agregar terminador nulo para manejar como cadena
    }
    return bytes_read; // Retorna la cantidad de bytes leídos
}

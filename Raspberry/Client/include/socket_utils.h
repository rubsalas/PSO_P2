#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <stdio.h>

/*
 * Función: socket_connect
 * ----------------------
 * Crea un socket y se conecta al servidor especificado mediante la dirección IP
 * y el puerto proporcionados.
 * 
 * Parámetros:
 *    ip - Puntero a una cadena de caracteres que representa la dirección IP del servidor
 *    port - Entero que especifica el puerto del servidor al que se conectará
 * 
 * Retorna:
 *    Descriptor del socket (un valor entero positivo) si tiene éxito,
 *    -1 si ocurre un error al crear o conectar el socket.
 */
int socket_connect(const char *ip, int port);

/*
 * Función: socket_send
 * ----------------------
 * Envía datos al servidor a través del socket especificado. 
 * 
 * Parámetros:
 *    sockfd - Descriptor del socket a través del cual se enviarán los datos
 *    data - Puntero a los datos que se desean enviar
 *    size - Tamaño en bytes de los datos a enviar
 * 
 * Retorna:
 *    Número de bytes enviados si tiene éxito,
 *    -1 si ocurre un error durante el envío.
 */
int socket_send(int sockfd, const char *data, size_t size);

/*
 * Función: socket_receive
 * ----------------------
 * Recibe datos del servidor a través del socket especificado. 
 * 
 * Parámetros:
 *    sockfd - Descriptor del socket desde el cual se recibirán los datos
 *    buffer - Puntero al buffer donde se almacenarán los datos recibidos
 *    size - Tamaño del buffer en bytes
 * 
 * Retorna:
 *    Número de bytes recibidos si tiene éxito,
 *    -1 si ocurre un error durante la recepción.
 */
int socket_receive(int sockfd, char *buffer, size_t size);

#endif // SOCKET_UTILS_H

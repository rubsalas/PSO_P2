#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <openssl/evp.h>
#include <openssl/err.h>

/*
 * Funcion: handleCryptoErrors
 * ----------------------
 * Maneja los errores que ocurren en las operaciones de cifrado o descifrado
 * utilizando la biblioteca OpenSSL. Imprime los errores en la salida estandar de error
 * y termina el programa.
 */
void handleCryptoErrors(void);

/*
 * Funcion: encrypt
 * ----------------------
 * Cifra un texto plano utilizando el algoritmo AES-128 en modo CBC.
 * 
 * Parametros:
 *    plainText - Puntero al texto que se desea cifrar
 *    textLength - Longitud del texto plano
 *    key - Puntero a la clave de cifrado de 128 bits (16 bytes)
 *    iv - Puntero al vector de inicializacion (IV) de 128 bits (16 bytes)
 *    cipherText - Puntero donde se almacenara el texto cifrado
 * 
 * Retorna:
 *    La longitud del texto cifrado.
 */
int encrypt(unsigned char *plainText, int textLength, unsigned char *key, unsigned char *iv, unsigned char *cipherText);

/*
 * Funcion: decrypt
 * ----------------------
 * Descifra un texto cifrado utilizando el algoritmo AES-128 en modo CBC.
 * 
 * Parametros:
 *    cipherText - Puntero al texto cifrado que se desea descifrar
 *    cipherTextLength - Longitud del texto cifrado
 *    key - Puntero a la clave de cifrado de 128 bits (16 bytes)
 *    iv - Puntero al vector de inicializacion (IV) de 128 bits (16 bytes)
 *    plainText - Puntero donde se almacenara el texto descifrado
 * 
 * Retorna:
 *    La longitud del texto descifrado.
 */
int decrypt(unsigned char *cipherText, int cipherTextLength, unsigned char *key, unsigned char *iv, unsigned char *plainText);

#endif // CRYPTO_UTILS_H

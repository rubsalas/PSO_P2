#include <stdio.h>
#include <stdlib.h>
#include "crypto_utils.h"

/*
 * Funcion: handleCryptoErrors
 * ----------------------
 * Maneja los errores que ocurren en las operaciones de cifrado o descifrado
 * utilizando la biblioteca OpenSSL. Imprime los errores en la salida estandar de error
 * y termina el programa.
 */
void handleCryptoErrors(void) {
    ERR_print_errors_fp(stderr);
    abort();
}

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
int encrypt(unsigned char *plainText, int textLength, unsigned char *key, unsigned char *iv, unsigned char *cipherText) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int cipherTextLength;

    // Inicializa el contexto de cifrado
    if (!(ctx = EVP_CIPHER_CTX_new())) handleCryptoErrors();
    
    // Configura el contexto para utilizar AES-128-CBC
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv)) handleCryptoErrors();
    
    // Realiza el cifrado de los datos en bloques
    if (1 != EVP_EncryptUpdate(ctx, cipherText, &len, plainText, textLength)) handleCryptoErrors();
    cipherTextLength = len;
    
    // Finaliza el cifrado de los datos
    if (1 != EVP_EncryptFinal_ex(ctx, cipherText + len, &len)) handleCryptoErrors();
    cipherTextLength += len;
    
    // Libera el contexto de cifrado
    EVP_CIPHER_CTX_free(ctx);
    return cipherTextLength;
}

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
int decrypt(unsigned char *cipherText, int cipherTextLength, unsigned char *key, unsigned char *iv, unsigned char *plainText) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plainTextLength;

    // Inicializa el contexto de descifrado
    if (!(ctx = EVP_CIPHER_CTX_new())) handleCryptoErrors();
    
    // Configura el contexto para utilizar AES-128-CBC
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv)) handleCryptoErrors();
    
    // Realiza el descifrado de los datos en bloques
    if (1 != EVP_DecryptUpdate(ctx, plainText, &len, cipherText, cipherTextLength)) handleCryptoErrors();
    plainTextLength = len;
    
    // Finaliza el descifrado de los datos
    if (1 != EVP_DecryptFinal_ex(ctx, plainText + len, &len)) handleCryptoErrors();
    plainTextLength += len;
    
    // Libera el contexto de descifrado
    EVP_CIPHER_CTX_free(ctx);
    return plainTextLength;
}

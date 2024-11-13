#include <stdio.h>
#include <stdlib.h>
#include "crypto_utils.h"

void manejarErroresCrypto(void) {
    ERR_print_errors_fp(stderr);
    abort();
}

int encriptar(unsigned char *textoPlano, int longitudTexto, unsigned char *clave, unsigned char *iv, unsigned char *textoCifrado) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int longitudTextoCifrado;

    if (!(ctx = EVP_CIPHER_CTX_new())) manejarErroresCrypto();
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, clave, iv)) manejarErroresCrypto();
    if (1 != EVP_EncryptUpdate(ctx, textoCifrado, &len, textoPlano, longitudTexto)) manejarErroresCrypto();
    longitudTextoCifrado = len;
    if (1 != EVP_EncryptFinal_ex(ctx, textoCifrado + len, &len)) manejarErroresCrypto();
    longitudTextoCifrado += len;
    EVP_CIPHER_CTX_free(ctx);
    return longitudTextoCifrado;
}

int desencriptar(unsigned char *textoCifrado, int longitudTextoCifrado, unsigned char *clave, unsigned char *iv, unsigned char *textoPlano) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int longitudTextoPlano;

    if (!(ctx = EVP_CIPHER_CTX_new())) manejarErroresCrypto();
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, clave, iv)) manejarErroresCrypto();
    if (1 != EVP_DecryptUpdate(ctx, textoPlano, &len, textoCifrado, longitudTextoCifrado)) manejarErroresCrypto();
    longitudTextoPlano = len;
    if (1 != EVP_DecryptFinal_ex(ctx, textoPlano + len, &len)) manejarErroresCrypto();
    longitudTextoPlano += len;
    EVP_CIPHER_CTX_free(ctx);
    return longitudTextoPlano;
}


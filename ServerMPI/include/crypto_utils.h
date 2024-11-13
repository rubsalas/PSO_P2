#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <openssl/evp.h>
#include <openssl/err.h>

void manejarErroresCrypto(void);
int encriptar(unsigned char *textoPlano, int longitudTexto, unsigned char *clave, unsigned char *iv, unsigned char *textoCifrado);
int desencriptar(unsigned char *textoCifrado, int longitudTextoCifrado, unsigned char *clave, unsigned char *iv, unsigned char *textoPlano);

#endif // CRYPTO_UTILS_H


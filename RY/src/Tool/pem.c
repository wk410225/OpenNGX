#include <stdio.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/err.h>

int main() {
    // 初始化 OpenSSL 库
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    FILE *pem_file = fopen("public_key.pem", "r");
    if (!pem_file) {
        perror("Unable to open PEM file");
        return 1;
    }

    // 读取 PEM 公钥
    RSA *rsa = PEM_read_RSA_PUBKEY(pem_file, NULL, NULL, NULL);
    if (!rsa) {
        printf("Error loading public key: %s\n", ERR_error_string(ERR_get_error(), NULL));
        fclose(pem_file);
        return 1;
    }

    fclose(pem_file);

    // 打开 DER 格式文件进行写入
    FILE *der_file = fopen("public_key.der", "wb");
    if (!der_file) {
        perror("Unable to open DER file");
        RSA_free(rsa);
        return 1;
    }

    // 将公钥写入 DER 文件
    int len = i2d_RSA_PUBKEY(rsa, NULL);
    if (len <= 0) {
        printf("Error encoding public key: %s\n", ERR_error_string(ERR_get_error(), NULL));
        fclose(der_file);
        RSA_free(rsa);
        return 1;
    }

    unsigned char *der_data = malloc(len);
    unsigned char *p = der_data;
    if (i2d_RSA_PUBKEY(rsa, &p) != len) {
        printf("Error encoding public key to DER format\n");
        fclose(der_file);
        RSA_free(rsa);
        free(der_data);
        return 1;
    }

    // 写入 DER 文件
    fwrite(der_data, 1, len, der_file);

    // 清理资源
    fclose(der_file);
    RSA_free(rsa);
    free(der_data);

    printf("PEM to DER conversion successful.\n");
    return 0;
}


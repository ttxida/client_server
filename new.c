#include <openssl/ssl.h>
#include <openssl/bio.h>

int main() {
    SSL_library_init();
    SSL_CTX *ctx = SSL_CTX_new(TLSv1_2_method());
    SSL_CTX_load_verify_locations(ctx, "ca-certs.pem", NULL);
    SSL *ssl = SSL_new(ctx);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr("example.com");
    dest.sin_port = htons(443);

    connect(sockfd, (struct sockaddr *)&dest, sizeof(dest));
    SSL_set_fd(ssl, sockfd);

    SSL_connect(ssl);

    char message[] = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
    SSL_write(ssl, message, strlen(message));

    char buffer[1024];
    SSL_read(ssl, buffer, sizeof(buffer));

    printf("Received: %s\n", buffer);

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);

    return 0;
}
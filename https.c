#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define MAX_BUF_SIZE 1024

int main() {
    const char *host = "iot.ghtk.vn";
    const char *port = "443";
    const char *path = "";
    const char *token = "";
//    const char *data = "your_data_here";

    SSL_library_init();
    SSL_CTX *ctx = SSL_CTX_new(TLSv1_2_client_method());

    if (ctx == NULL) {
        fprintf(stderr, "Failed to create SSL context\n");
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        fprintf(stderr, "Failed to create socket\n");
        return 1;
    }

    struct hostent *server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr, "Failed to resolve host name\n");
        return 1;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        fprintf(stderr, "Failed to connect to server\n");
        return 1;
    }

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);
    if (SSL_connect(ssl) != 1) {
        fprintf(stderr, "Failed to establish SSL connection\n");
        return 1;
    }

    char request[MAX_BUF_SIZE] = {0};
    snprintf(request, sizeof(request), "GET %s HTTP/1.1\r\nHost: %s\r\nAuthorization: Bearer %s\r\n\r\n", path, host, token);
    SSL_write(ssl, request, strlen(request));

    char response[4096];
    int len = SSL_read(ssl, response, sizeof(response) - 1);
    response[len] = '\0';
    printf("%s", response);


//    SSL_write(ssl, request, strlen(request));
//    char buffer[1024];
//    int bytes = 0;
//    while ((bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
//        buffer[bytes] = '\0';
//        printf("%s", buffer);
//    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);

    return 0;
}

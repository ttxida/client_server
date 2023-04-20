#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int main(int argc, char *argv[]) {
    char *hostname = "";
    int port = 443;

    SSL_library_init();
    SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *server = gethostbyname(hostname);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);
    SSL_connect(ssl);

    char *request = "GET / HTTP/1.1\r\nHost: \r\n\r\n";
    SSL_write(ssl, request, strlen(request));

    char buffer[1024];
    int bytes = 0;
    while ((bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);

    return 0;
}

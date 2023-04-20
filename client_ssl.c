#include <stdio.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int main()
{
    SSL_CTX *ctx;
    SSL *ssl;
    int sockfd;
    char buffer[1024];

    // Initialize OpenSSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    // Create a new SSL context
    ctx = SSL_CTX_new(TLS_client_method());

    // Create a new socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Connect to the server
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(443);
    inet_pton(AF_INET, "", &server_address.sin_addr);
    connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address));

    // Connect the SSL object with the socket
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);
    SSL_connect(ssl);

    // Send a request to the server with x-access token
    const char* request = "GET / HTTP/1.1\r\nHost: \r\nX-Access-Token: "token"\r\n\r\n";
    SSL_write(ssl, request, strlen(request));

    // Receive the response from the server
    SSL_read(ssl, buffer, sizeof(buffer));

    // Close the SSL connection and socket
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(sockfd);

    // Print the response
    printf("%s", buffer);

    return 0;
}

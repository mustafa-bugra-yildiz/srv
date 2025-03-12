#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 3000

int
read_file(const char *path, char *buf, size_t size)
{
    FILE *file = fopen(path, "r");
    if (!file) {
        perror("fopen");
        return -1;
    }
    memset(buf, 0, size);
    fread(buf, 1, size, file);
    fclose(file);
    return 0;
}

int
respond(int client, char *payload)
{
    char buf[1024 * 1024];
    int code;

    memset(buf, 0, sizeof(buf));
    sprintf(
        buf,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "Content-Length: %lu\r\n"
        "\r\n"
        "\r\n"
        "%s",
        strlen(payload) + 1,
        payload
    );

    code = write(client, buf, strlen(buf));
    if (code < 0) {
        perror("write");
        return -1;
    }

    return 0;
}

int
main()
{
    struct sockaddr_in addr;
    int sock, exitcode, client_sock;
    char res[1024 * 1024];

    exitcode = 0;
    sock = -1;
    client_sock = -1;

    if (read_file("src/res.html", res, sizeof(res)) < 0) {
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }
    puts("socket created");

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exitcode = 1;
        goto cleanup;
    }
    printf("bound :%d\n", PORT);

    if (listen(sock, 1) < 0) {
        perror("listen");
        exitcode = 1;
        goto cleanup;
    }
    puts("listening");

    while (1) {
        client_sock = accept(sock, NULL, NULL);
        if (client_sock < 0) {
            perror("accept");
            exitcode = 1;
            goto cleanup;
        }
        puts("accepted");

        respond(client_sock, res);
        puts("responded");

        close(client_sock);
        client_sock = -1;
    }


cleanup:
    if (client_sock >= 0) {
        close(client_sock);
    }
    if (sock >= 0) {
        close(sock);
    }
    return exitcode;
}

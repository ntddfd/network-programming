#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8001);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    listen(listener, 5);

    int client = accept(listener, NULL, NULL);
    if (client < 0) {
        perror("accept() failed");
        exit(EXIT_FAILURE);
    }

    FILE *f = fopen("greetings.txt", "rb");
    char buf[2048];
    int len = fread(buf, 1, sizeof(buf), f);
    send(client, buf, len, 0);

    memset(buf, 0, sizeof(buf));
    fclose(f);

    f = fopen("server.txt", "wb");

    long size;
    recv(client, &size, sizeof(size), 0);

    while (1) {
        len = recv(client, buf, sizeof(buf), 0);
        if (len <= 0)
            break;
        fwrite(buf, 1, len, f);
    }

    fclose(f);
    close(client);
    close(listener);

    return 0;
}
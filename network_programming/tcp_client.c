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
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client < 0) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8001);

    if (connect(client, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("connect() failed");
        exit(EXIT_FAILURE);
    }

    char buf[256];
    int len = recv(client, buf, sizeof(buf), 0);
    for (int i = 0; i < len; ++i) printf("%c", buf[i]);
    printf("\n");

    printf("Nhap du lieu: ");
    fgets(buf, sizeof(buf), stdin);
    send(client, buf, strlen(buf), 0);

    close(client);

    return 0;
}
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

    FILE *f = fopen("sv_client.txt", "wb");

    printf("MSSV: ");
    fgets(buf, sizeof(buf), stdin);
    fwrite(buf, 1, strlen(buf), f);
    printf("Ho va ten: ");
    fgets(buf, sizeof(buf), stdin);
    fwrite(buf, 1, strlen(buf), f);
    printf("Ngay sinh: ");
    fgets(buf, sizeof(buf), stdin);
    fwrite(buf, 1, strlen(buf), f);
    printf("CPA: ");
    fgets(buf, sizeof(buf), stdin);
    fwrite(buf, 1, strlen(buf), f);

    fclose(f);
    f = fopen("sv_client.txt", "rb");
    char buf_send[2048];
    len = fread(buf_send, 1, sizeof(buf_send), f);
    send(client, buf_send, strlen(buf_send), 0);

    close(client);

    return 0;
}
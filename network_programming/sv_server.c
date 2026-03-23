#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr, cli;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8001);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    listen(listener, 5);
    int clilen = sizeof(cli);

    int client = accept(listener, (struct sockaddr *)&cli, &clilen);
    if (client < 0) {
        perror("accept() failed");
        exit(EXIT_FAILURE);
    }

    char buf[2048];
    FILE *f = fopen("server_log.txt", "wb");

    long size;
    recv(client, &size, sizeof(size), 0);
    fwrite(inet_ntoa(cli.sin_addr), 1, clilen, f);

    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char* time = asctime(timeinfo);
    fwrite(time, 1, strlen(time), f);

    while (1) {
        int len = recv(client, buf, sizeof(buf), 0);
        if (len <= 0)
            break;
        for (int i = 0; i < len; i++) printf("%c", buf[i]);
        printf("\n");
        fwrite(buf, 1, len, f);
    }

    fclose(f);
    close(client);
    close(listener);

    return 0;
}
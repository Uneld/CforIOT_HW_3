#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

#define SOP 0x7E
#define EOP 0x7E

void stuff_bytes(char *data, int len, char *stuffed_data)
{
    int i, j = 0;
    stuffed_data[j++] = SOP;
    for (i = 0; i < len; i++)
    {
        if (data[i] == SOP || data[i] == EOP || data[i] == 0x7D)
        {
            stuffed_data[j++] = 0x7D;
            stuffed_data[j++] = data[i] ^ 0x20;
        }
        else
        {
            stuffed_data[j++] = data[i];
        }
    }
    stuffed_data[j++] = EOP;
    stuffed_data[j] = '\0';
}

void unstuff_bytes(char *data, int len, char *unstuffed_data)
{
    int i, j = 0;
    for (i = 0; i < len; i++)
    {
        if (data[i] == 0x7D)
        {
            i++;
            unstuffed_data[j++] = data[i] ^ 0x20;
        }
        else
        {
            unstuffed_data[j++] = data[i];
        }
    }
    unstuffed_data[j] = '\0';
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024], stuffed_buffer[1024], unstuffed_buffer[1024];

    char message[1024];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        return 1;
    }

    if (listen(server_socket, 1) < 0)
    {
        perror("listen");
        return 1;
    }

    printf("Server started. Wait clients\n");

    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
    if (client_socket < 0)
    {
        perror("accept");
        return 1;
    }

    printf("Client connected .\n");

    while (1)
    {
        int len = recv(client_socket, buffer, 1024, 0);
        if (len < 0)
        {
            perror("recv");
            return 1;
        }

        unstuff_bytes(buffer, len, unstuffed_buffer);

        printf("Recieved message: %s\n", unstuffed_buffer);

        if (unstuffed_buffer[1] == '#')
        {
            break;
        }

        printf("Enter message: ");
        fgets(message, 1024, stdin);

        int message_len = strlen(message);
        stuff_bytes((char *)message, message_len, stuffed_buffer);

        send(client_socket, stuffed_buffer, message_len + 2, 0);

        if (message[0] == '#')
        {
            break;
        }
    }
    printf("\n\n=> Connection terminated with IP %s\n", inet_ntoa(server_addr.sin_addr));
    close(client_socket);
    close(server_socket);
    printf("\nGoodbye...\n");
    return 0;
}

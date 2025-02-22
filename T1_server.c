/*!
 * Simple chat program (server side).cpp - http://github.com/hassanyf
 * Version - 2.0.1
 *
 * Copyright (c) 2016 Hassan M. Yousuf
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    /* ---------- ИНИЦИАЛИЗАЦИЯ ПЕРЕМЕННЫХ СЕРВЕРА---------- */

    /*
       1. client/server - два файл-дескриптора.
       Эти две перемнные хранят значение сокетов,
       которые вернула система при подключении.
       2. portNum нужен для хранения номера порта, на котором происходит соединение.
       3. isExit - булевая переменная, признак завершения программы.
       4. Сервер считывает сообщение из сокет соединения в буффер обмена для приема/отправки данных к/от сервера.

    */
    int server;           // файл-дескриптор сервера
    int client;           // файл-дескриптор клиента
    int portNum = 1500;   // номера порта, на котором происходит соединение (0 до 65535)
    bool isExit = false;  // булевая переменная, признак завершения программы.
    int bufsize = 1024;   // размер буффера
    char buffer[bufsize]; // буффер обмена для приема/отправки данных к/от сервера
    /*
           5. A sockaddr_in - структура, содержащая интернет адрес, с которым будет установлено соединение.
           Эта структура уже определена в netinet/in.h, поэтому нет необходимости
           заново ее задавать.
            DEFINITION:
            struct sockaddr_in
            {
              short   sin_family;
              u_short sin_port;
              struct  in_addr sin_addr;
              char    sin_zero[8];
            };
            6. serv_addr будет содержать адрес сервера
            7. socklen_t - длиной по крайней мере 32 бита
    */
    struct sockaddr_in server_addr;
    socklen_t size;

    /* ---------- УСТАНОВКА СОКЕТ СОЕДИНЕНИЯ ----------*/
    /* --------------- socket() функция ------------------*/

    /*
       Socket() функция создает новый сокет.
       На вход получает 3 аргумента,
           a. AF_INET: доменный адрес сокета.
           b. SOCK_STREAM: тип сокета. Потоковый сокет, в котором сообщение
           читается последовательным потоком(TCP).
           c. Третий это аргумент протокола: всегда должен быть 0.
           Операционная система выберет самый подходящий протокол.
           Функция возвращает файл-дескриптор целое число - соединение, которое используется для всех ссылок
           на этот сокет. Если произошла ошибка соединения, то возвращается -1.
   */

    server = socket(AF_INET, SOCK_STREAM, 0);
    printf("SERVER\n");

    if (server < 0)
    {
        printf("Error establishing socket...\n");
        exit(1);
    }

    printf("=> Socket server has been created...\n");

    /*
        Переменная serv_addr - структура sockaddr_in.
        sin_family содержит код для адресной семьи.
        Всегда должна быть установлена AF_INET.
        INADDR_ANY содержит IP-адрес хоста. Для сервера - это
        IP-адрес компьютера, на котором работает сервер.
        htons() функция переводит номер порта из порядка байтов хоста
        в номер порта в порядке байтов сети.

    */

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(portNum);

    /* ---------- ПРИВЯЗКА СОКЕТА ---------- */
    /* ---------------- bind() ---------------- */

    /*
        bind() функция привязывает сокет к адресу, то есть в нашем случае
        к адресу сервера и номеру порта, на котором сервер будет работать.
        Для этого нужны три аргумента: файл дескриптор сокета, указатель на структуру
        типа sockaddr (должен указывать на правильный тип)
        *
    */

    if ((bind(server, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0)
    {
        printf("=> Error binding connection, the socket has already been established...\n");
        return -1;
    }

    size = sizeof(server_addr);
    printf("=> Looking for clients...\n");

    /* ------------- ПРОСЛУШИВАНИЕ СОКЕТА ------------- */
    /* ---------------- listen() ---------------- */

    /*
        Функция listen позволяет прослушивать сокета  для подключения.
        Программа будет в состоянии простоя, если подключений не обнаружится.
        Первый аргумент - это файл дескриптор сокета, второй - количество клиентов,
        то есть количество подключений, которое сервер может обработать, пока процесс
        обрабатывает определенное подключение. Максимальное число клиентов во многих системах равняется 5.
    */

    listen(server, 1);

    /* ------------- ПОДКЛЮЧЕНИЕ КЛИЕНТОВ  ------------- */
    /* ----------------- accept() ------------------- */

    /*
        Система accept() вызывает процесс блокировки пока клиент подключается к серверу.
        К тому же, она пробуждает процесс, когда подключение с клиентом было успешно установлено.
        Она возвращает новый файл дескриптор, и вся связь по этому подключению должна
        осуществляться, используя новый файл дескриптор. Второй аргумент - указатель на
        адрес клиента, третий - размер структуры.
    */

    int clientCount = 1;
    client = accept(server, (struct sockaddr *)&server_addr, &size);

    // первая проверка действительности подключения
    if (client < 0)
        printf("=> Error on accepting...");

    // Основной цикл
    while (client > 0)
    {
        printf("=> Connected with the client %d, you are good to go...\n", clientCount);
        printf("\n=> Enter # to end the connection\n");
        //.....
        strcpy(buffer, "=> Server connected...\n");
        send(client, buffer, bufsize, 0);

        /*
            Примечание: мы перейдем в этот момент только после того, как
            клиент успешно подключится к нашему серверу.
            Произойдет чтение сокета. Заметим, что read() будет
            блокировать до момента наличия чего-то для чтения
            Чтение будет происходить maximum 1024 символов в пакете
        */
        do
        {
            printf("Client: ");
            do
            {
                recv(client, buffer, bufsize, 0);
                printf("%s ", buffer);
                if (buffer[0] == '#')
                {
                    isExit = true;
                }
            } while (buffer[0] != '.' && buffer[0] != '#');

            printf("\nServer: ");
            do
            {
                scanf("%s", buffer);
                send(client, buffer, bufsize, 0);
                if (buffer[0] == '#')
                {
                    isExit = true;
                }
            } while (buffer[0] != '.' && buffer[0] != '#');

        } while (!isExit);

        /*
            Как только соединение будет установлено, оба конца связи могут
            писать и читать соединение. Все, написанное клиентом, будет прочитано сервером,
            а все, написанное сервером, будет прочитано клиентом.
        */

        /* ---------------- ЗАВЕРШЕНИЕ СОЕДИНЕНИЯ ------------- */
        /* ----------------- close() --------------- */

        /*
            Как только сервер нажмет # для завершения соединения,
            цикл завершится и закроет серверное сокет соединение
            и клиентсткое соединение.
        */

        // inet_ntoa переводит данные пакета в IP-адрес, который был взят у клиента
        printf("\n\n=> Connection terminated with IP %s\n", inet_ntoa(server_addr.sin_addr));
        close(client);
        close(server);
        printf("\nGoodbye...");
        isExit = false;
        //~ exit(1);
        return 0;
    }
    return 0;
}

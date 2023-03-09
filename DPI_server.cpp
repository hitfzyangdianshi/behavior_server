#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

#include <winsock2.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS  

#pragma comment(lib,"ws2_32.lib")


using namespace std;



char* receive_data(int port) {
    int result = 0;

    SOCKET server_ = INVALID_SOCKET;  

    WSADATA data_;

    result = WSAStartup(MAKEWORD(2, 2), &data_);//inital
    if (result != 0)
    {
        cout << "WSAStartup() init error " << GetLastError() << std::endl;
        return NULL;
    }

    server_ = socket(AF_INET, SOCK_STREAM, 0);

    SOCKADDR_IN addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(port);
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//ip port 

    result = bind(server_, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));

    if (result != 0)
    {
        cout << "bind() error" << result;
        return NULL;
    }

    result = listen(server_, 10);
    if (result != 0)
    {
        cout << "listen error" << result;
        return NULL;
    }

    SOCKADDR_IN addrClient;
    int len = sizeof(SOCKADDR);

    cout << "wait new connect......" << std::endl;
    SOCKET socketConn = accept(server_, (SOCKADDR*)&addrClient, &len);

    if (socketConn == SOCKET_ERROR)
    {
        cout << " accept error" << WSAGetLastError();
        return NULL;
    }



    char* f;

    f = inet_ntoa(addrClient.sin_addr);
    cout << "accept client ip:" << f << endl;

    char recvBuff[9999];
    memset(recvBuff, '\0', sizeof(char) * 9999);

    recv(socketConn, recvBuff, 9999, 0);

    cout << "recv from client:\n" << recvBuff << endl;;

    ofstream  output("reveive output.txt");
    output << recvBuff << endl;
    output.close();


    closesocket(server_);
    WSACleanup();


    return recvBuff;

}


bool send_data(const char* addr, int port, char* data_buffer) {
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        cout << "WSAStartup error";
        return false;
    }

    SOCKADDR_IN addrServer;
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(port);
    addrServer.sin_addr.S_un.S_addr = inet_addr(addr);

    SOCKET socketClient = socket(AF_INET, SOCK_STREAM, 0);

    if (socketClient == INVALID_SOCKET)
    {
        cout << "socket create error";
        return false;
    }


    if (connect(socketClient, (struct sockaddr*)&addrServer, sizeof(addrServer)) == INVALID_SOCKET)
    {
        cout << "connect error";
        return false;
    }


    int a;
    send(socketClient, data_buffer, strlen(data_buffer), 0);

    printf("data sent[%d]: %s\n", strlen(data_buffer), data_buffer);


    closesocket(socketClient);
    WSACleanup();

    return true;
}


char composite_number[] = "7778509999111111111111111110000000999231112332199999999999999999999999999999999999999999999999999992";

int main(int argc, char** argv) {

#ifdef LOOP_SENDING
    while (1) {
#endif // LOOP_SENDING
        send_data(argv[1], 4321, composite_number);


        char* received_data;
        received_data = receive_data(4321);

        FILE* read_output = fopen("reveive output.txt", "r");
        FILE* usage_out = fopen("usage_out.txt", "a");


        char received_line[200];
        int vmsize, rss;
        double cpu;
        long extime;
        for (int i = 1; i <= 26; i++) {
            if (i >= 1 && i <= 21) {
                fscanf(read_output, "%s", received_line);
            }
            else if (i == 22) {
                fscanf(read_output, "%s %d", received_line, &vmsize);
            }
            else if (i == 23) {
                fscanf(read_output, "%s %d", received_line, &rss);
            }
            else if (i == 24) {
                fscanf(read_output, "%s %lf", received_line, &cpu);
            }
            else if (i == 25) {
                fscanf(read_output, "%s %ld", received_line, &extime);
            }
            else {
                break;
            }
        }

        fprintf(usage_out, "%f,%d,%d,%ld\n", cpu, vmsize, rss, extime);

#ifdef LOOP_SENDING
        Sleep(5000);
    }
#endif // LOOP_SENDING  
    return 0;
}
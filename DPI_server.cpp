#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS  


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

#include <openssl/ssl.h>
#include <openssl/err.h>


#pragma comment(lib, "D:\\OpenSSL_noshared\\lib\\libssl.lib")
#pragma comment(lib, "D:\\OpenSSL_noshared\\lib\\libcrypto.lib")

#include <winsock2.h>



#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Crypt32.lib")



using namespace std;


#if 0
char* receive_data(int port) {
    int result = 0;

    SOCKET server_ = INVALID_SOCKET; //socket 对象

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
    memset(recvBuff, '\0', sizeof(char)* 9999);

    recv(socketConn, recvBuff, 9999, 0);

    cout << "recv from client:\n" << recvBuff << endl;;

    ofstream  output("reveive output.txt");
    output << recvBuff << endl;
    output.close();


    closesocket(server_);
    WSACleanup();


    return recvBuff;

}


int send_data(const char* addr, int port, char* data_buffer) {
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
#endif


int split(char dst[][100000], char* str, const char* spl)
{
    int n = 0;
    char* result = NULL;
    result = strtok(str, spl);
    while (result != NULL)
    {
        strcpy(dst[n++], result);
        result = strtok(NULL, spl);
    }
    return n;
}



char composite_number[] = "777850999911111111111111111111111122222222222222222222229999999999999999999999999999999900000000000000000000099999999999999999999999999999999999999999999999999999992";
char client_ip_address[] = "192.168.33.131";
int port = 4321;


int main(int argc, char** argv) {

    char buf_recieve[141000];


    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        perror( "WSAStartup error");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SOCKADDR_IN addrServer;



    SSL_CTX* ctx;
    SSL* ssl;

    ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);

    if (SSL_CTX_load_verify_locations(ctx, "keys/cacert.pem", NULL) != 1) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_set_default_verify_paths(ctx) != 1) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_certificate_file(ctx, "keys/servercert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "keys/serverkey.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_check_private_key(ctx) != 1) {
        perror("SSL_CTX_check_private_key failed");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);


    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(port);
    addrServer.sin_addr.S_un.S_addr = inet_addr(client_ip_address);


    SOCKET socketClient = socket(AF_INET, SOCK_STREAM, 0);

    if (socketClient == INVALID_SOCKET || socketClient == SOCKET_ERROR)  {
        perror("socket create error");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    ssl = SSL_new(ctx);
    if (ssl == NULL) {
        perror("SSL_new error \n");
    }


    SSL_set_fd(ssl, socketClient);


    int connect_result;
    connect_result = connect(socketClient, (struct sockaddr*)&addrServer, sizeof(addrServer));
    if (connect_result < 0) {
        printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
        exit(EXIT_FAILURE);
    }

    if (connect_result == 0) {
        /* Run the OpenSSL handshake */
        int result_ssl = SSL_connect(ssl);

        /* Exchange some data if the connection succeeded */
        if (result_ssl == 1) {
            SSL_write(ssl, composite_number,strlen(composite_number));
            printf("send composite_number: %s\n", composite_number);
            SSL_read(ssl, buf_recieve, 141000);
            printf("Received message from server: \n%s\n", buf_recieve);
            fprintf(fopen("reveive output.txt", "w"), "%s\n", buf_recieve);
        }
    }


    

#if 0

    char* received_data;
    received_data = receive_data(4321);
#endif

    



    FILE* usage_out = fopen("usage_out.txt", "a");


    char received_numbers[20][1000],tmp,tmps[20];
    int vmsize, rss;
    double cpu;
    long extime;

    sscanf(buf_recieve, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %c %s %d %s %d %s %lf %s %ld",
        received_numbers[0], received_numbers[1], received_numbers[2], received_numbers[3], received_numbers[4], received_numbers[5], received_numbers[6], received_numbers[7],
        received_numbers[8], received_numbers[9], received_numbers[10], received_numbers[11], received_numbers[12], received_numbers[13], received_numbers[14],
        received_numbers[15], received_numbers[16], received_numbers[17], received_numbers[18], received_numbers[19],
        &tmp,tmps,&vmsize,tmps,&rss,tmps,&cpu,tmps,&extime);




    fprintf(usage_out, "%f,%d,%d,%ld\n", cpu, vmsize, rss, extime);    


    char rec_cut[2][100000];
    split(rec_cut, buf_recieve, "@");

    //printf("get ptrace result: \n%s\n", rec_cut[1]);
    fprintf(fopen("ptrace result.txt","w"),"%s\n", rec_cut[1]);


    int ptrace_sys_count[1000000];
    long syscall;
    int syscall_count;
    memset(ptrace_sys_count, 0, sizeof(int) * 1000000);

    FILE *ptrace_read=fopen("ptrace result.txt", "r");

    while (fscanf(ptrace_read, "%ld %d", &syscall, &syscall_count) != EOF) {
        ptrace_sys_count[syscall] = syscall_count;
        printf(" %lu\t%d\n", syscall, ptrace_sys_count[syscall]);
    }
    //for (long i = 0; i < 1000000; i++) {
    //    if(ptrace_sys_count[i]>0)
    //        printf("%ld\t%d\n", i, ptrace_sys_count[i]);
    //}
    


    SSL_shutdown(ssl);
    SSL_free(ssl);
    closesocket(socketClient);
    WSACleanup();

    return 0;
}
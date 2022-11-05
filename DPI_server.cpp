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



char composite_number[] = "7778509999111111111111111110000000999231112332199999999999999999999999999999999999999999999999999992";
char client_ip_address[] = "192.168.0.108";
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

        else {
            perror("SSL connection error: ");
            int err_gererror= ERR_get_error();
            cout <<"ERR get error = "<< err_gererror<<endl;
            int ssl_geterror = SSL_get_error(ssl, result_ssl);
            cout << "SSL get error : "<< ssl_geterror <<endl;
            if (ssl_geterror == SSL_ERROR_SYSCALL) {
                printf("%s (errno: %d)\n", strerror(errno), errno);
            }

        }
    }



    



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
    FILE* ptrace_write = fopen("ptrace result.txt", "w");
    fprintf(ptrace_write,"%s\n", rec_cut[1]);
    fclose(ptrace_write);


   
    int syscall_tag=0;

    struct syscall_list {
        long syscall;
        int syscall_count;
    }_syscall_list[500];

    memset(_syscall_list,NULL, sizeof(_syscall_list[0]) * 500);

    FILE *ptrace_read=fopen("ptrace result.txt", "r");

    while (fscanf(ptrace_read, "%ld %d", &_syscall_list[syscall_tag].syscall, &_syscall_list[syscall_tag].syscall_count) != EOF) {
        syscall_tag++;
    }
    fclose(ptrace_read);

    


    SSL_shutdown(ssl);
    SSL_free(ssl);
    closesocket(socketClient);
    WSACleanup();



    for (int i = 0; i < syscall_tag; i++) {
        printf(" %ld\t%d\n", _syscall_list[i].syscall, _syscall_list[i].syscall_count);
    }




    return 0;
}
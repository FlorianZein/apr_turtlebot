#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#include <iostream>
#include <cstring>
#include <string>

#define RCVBUFSIZE 512   /* Size of receive buffer */

void DieWithError(char *errorMessage);  /* Error handling function */

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort = 9999;     /* Echo server port */
    const char *servIP = "192.168.100.55";      // local ip of desktop                /* Server IP address (dotted quad) */
    const char *echoString = "---START---{\"linear\": 0.00, \"angular\": 0.0}___END___";               /* String to send to echo server */
    unsigned int echoStringLen;      /* Length of string to echo */ 


    while(true)
    {

        /* Create a reliable, stream socket using TCP */
        if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            DieWithError("socket() failed");

        /* Construct the server address structure */
        memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
        echoServAddr.sin_family      = AF_INET;             /* Internet address family */
        echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
        echoServAddr.sin_port        = htons(echoServPort); /* Server port */

        /* Establish the connection to the echo server */
        if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
            DieWithError("connect() failed");

        echoStringLen = strlen(echoString);          /* Determine input length */

        /* Send the string to the server */
        if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
            DieWithError("send() sent a different number of bytes than expected");
            
        std::cout << "send stop" << std::endl;

        

        close(sock);

        usleep(10);  // 10ms

    }


    exit(0);
}

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
    const char *servIP = "192.168.100.50";      // local ip of desktop                /* Server IP address (dotted quad) */
    const char *echoString = "---START---{\"linear\": 0.00, \"angular\": 0.0}___END___";               /* String to send to echo server */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    unsigned int echoStringLen;      /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() 
                                        and total bytes read */

    // if ((argc < 3) || (argc > 4))    /* Test for correct number of arguments */
    // {
    //    fprintf(stderr, "Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n",
    //            argv[0]);
    //    exit(1);
    // }

    // servIP = argv[1];             /* First arg: server IP address (dotted quad) */
    // echoString = argv[2];         /* Second arg: string to echo */

    // if (argc == 4)
    //     echoServPort = atoi(argv[3]); /* Use given port, if any */
    // else
    //     echoServPort = 7;  /* 7 is the well-known port for the echo service */


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
        
    std::cout << "test" << std::endl;

    /* Receive the same string back from the server */
    // totalBytesRcvd = 0;
    // printf("Received: ");                /* Setup to print the echoed string */
    // while ( !(  echoBuffer[bytesRcvd-9] == '_'  &&
    //             echoBuffer[bytesRcvd-8] == '_'  &&
    //             echoBuffer[bytesRcvd-7] == '_'  &&
    //             echoBuffer[bytesRcvd-6] == 'E'  &&
    //             echoBuffer[bytesRcvd-5] == 'N'  &&
    //             echoBuffer[bytesRcvd-4] == 'D'  &&
    //             echoBuffer[bytesRcvd-3] == '_'  &&
    //             echoBuffer[bytesRcvd-2] == '_'  &&
    //             echoBuffer[bytesRcvd-1] == '_'      ))
    // {
    //     /* Receive up to the buffer size (minus 1 to leave space for
    //        a null terminator) bytes from the sender */
    //     if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
    //         DieWithError("recv() failed or connection closed prematurely");
    //     totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
    //     std::cout << bytesRcvd << std::endl;
    //     if( echoBuffer[bytesRcvd-9] == '_'  &&
    //         echoBuffer[bytesRcvd-8] == '_'  &&
    //         echoBuffer[bytesRcvd-7] == '_'  &&
    //         echoBuffer[bytesRcvd-6] == 'E'  &&
    //         echoBuffer[bytesRcvd-5] == 'N'  &&
    //         echoBuffer[bytesRcvd-4] == 'D'  &&
    //         echoBuffer[bytesRcvd-3] == '_'  &&
    //         echoBuffer[bytesRcvd-2] == '_'  &&
    //         echoBuffer[bytesRcvd-1] == '_')
    //     {
    //         printf("hello");
    //     }
    //     echoBuffer[bytesRcvd] = '\0';  /* Terminate the string! */
    //     printf("%s", echoBuffer);      /* Print the echo buffer */
    // }

    printf("\n");    /* Print a final linefeed */
    echoBuffer[bytesRcvd-1] = 'X';

    

    close(sock);

    usleep(10);  // 10ms

    }


    exit(0);
}

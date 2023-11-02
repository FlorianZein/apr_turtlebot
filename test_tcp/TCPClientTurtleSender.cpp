#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#include <string>
#include <cstring>
#include <iostream>

#define RCVBUFSIZE 16384   /* Size of receive buffer */

void DieWithError(char *errorMessage);  /* Error handling function */

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */
    char *echoString = new char[200];                /* String to send to echo server */
    char *vel_x, *vel_theta;
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    unsigned int echoStringLen;      /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() 
                                        and total bytes read */

    if ((argc < 5) || (argc > 5))    /* Test for correct number of arguments */
    {
       fprintf(stderr, "Usage: %s <Server IP> <vel_x> <vel_theta> <Echo Port>\n",
               argv[0]);
       exit(1);
    }

    servIP = argv[1];             /* First arg: server IP address (dotted quad) */
    // echoString = argv[2];         /* Second arg: string to echo */
    vel_x = argv[2]; 
    vel_theta = argv[3];

    if (argc == 5)
        echoServPort = atoi(argv[4]); /* Use given port, if any */
    else
        echoServPort = 7;  /* 7 is the well-known port for the echo service */

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
        

    std::string str_stringecho = "---START---{\"linear\": " + std::string(vel_x) + ", \"angular\": " + std::string(vel_theta) + "}___END___";
    std::cout << str_stringecho << std::endl;
    // echoString = str_stringecho.c_str();
    std::cout << echoString << std::endl;
    std::strcpy(echoString, str_stringecho.c_str());
    std::cout << "test" << std::endl;
    printf("%s", echoString);

    echoStringLen = strlen(echoString);          /* Determine input length */

    /* Send the string to the server */
    if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
        DieWithError("send() sent a different number of bytes than expected");

    /* Receive the same string back from the server */
    // totalBytesRcvd = 0;
    // printf("Received: ");                /* Setup to print the echoed string */
    // while (totalBytesRcvd < echoStringLen )
    // {
    //     /* Receive up to the buffer size (minus 1 to leave space for
    //        a null terminator) bytes from the sender */
        
    //     if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
    //         DieWithError("recv() failed or connection closed prematurely");
    //     totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
    //     echoBuffer[bytesRcvd] = '\0';  /* Terminate the string! */
    //     printf("%s", echoBuffer);      /* Print the echo buffer */
    // }

    printf("\n");    /* Print a final linefeed */
    delete[] echoString;
    close(sock);
    exit(0);
}

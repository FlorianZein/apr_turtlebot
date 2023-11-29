#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>  
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <iostream>
#include <cstring>
#include <string>

#define RCVBUFSIZE 512   /* Size of receive buffer */

key_t KEY_ODOM = 810;
void producerHandler (int sig);
int msgqid_odom; 
enum MessageType { PROD_MSG=1, CONS_MSG };
struct Message_Odom
{
    long type;
    float data[7];
};

void DieWithError(char *errorMessage);  /* Error handling function */

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort = 9998;     /* Echo server port */
    const char *servIP = "192.168.100.50";      // local ip of desktop                /* Server IP address (dotted quad) */
    const char *echoString = "---START---{\"header\": {\"seq\": 67769, \"stamp\": {\"secs\": 1677511096, \"nsecs\": 329690933}, \"frame_id\": \"odom\"}, \"child_frame_id\": \"base_footprint\", \"pose\": {\"pose\": {\"position\": {\"x\": -8.901372348191217e-05, \"y\": 6.059087172616273e-05, \"z\": 0.0}, \"orientation\": {\"x\": 0.0, \"y\": 0.0, \"z\": -0.5472193956375122, \"w\": 0.8369892239570618}}, \"covariance\": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]}, \"twist\": {\"twist\": {\"linear\": {\"x\": 0.0003163835790473968, \"y\": 0.0, \"z\": 0.0}, \"angular\": {\"x\": 0.0, \"y\": 0.0, \"z\": 0.0009506940841674805}}, \"covariance\": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]}}___END___";               /* String to send to echo server */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    unsigned int echoStringLen;      /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() 
                                        and total bytes read */


    msgqid_odom = msgget(KEY_ODOM, 0666 | IPC_CREAT);
    if (msgqid_odom == -1) {
      std::cerr << "msgget odom prod failed\n";
      exit(EXIT_FAILURE);
    }
    signal(SIGINT, producerHandler);
    std::cout << "I am the odom prod" << std::endl;

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

    // /* Send the string to the server */
    // if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
    //     DieWithError("send() sent a different number of bytes than expected");

    /* Receive the same string back from the server */
    totalBytesRcvd = 0;
    printf("Received: ");                /* Setup to print the echoed string */

    std::string odomData;

    while ( !(  echoBuffer[bytesRcvd-9] == '_'  &&
                echoBuffer[bytesRcvd-8] == '_'  &&
                echoBuffer[bytesRcvd-7] == '_'  &&
                echoBuffer[bytesRcvd-6] == 'E'  &&
                echoBuffer[bytesRcvd-5] == 'N'  &&
                echoBuffer[bytesRcvd-4] == 'D'  &&
                echoBuffer[bytesRcvd-3] == '_'  &&
                echoBuffer[bytesRcvd-2] == '_'  &&
                echoBuffer[bytesRcvd-1] == '_'      ))
    {
        /* Receive up to the buffer size (minus 1 to leave space for
           a null terminator) bytes from the sender */
        if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
            DieWithError("recv() failed or connection closed prematurely");
        totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
        // std::cout << bytesRcvd << std::endl;
        // if( echoBuffer[bytesRcvd-9] == '_'  &&
        //     echoBuffer[bytesRcvd-8] == '_'  &&
        //     echoBuffer[bytesRcvd-7] == '_'  &&
        //     echoBuffer[bytesRcvd-6] == 'E'  &&
        //     echoBuffer[bytesRcvd-5] == 'N'  &&
        //     echoBuffer[bytesRcvd-4] == 'D'  &&
        //     echoBuffer[bytesRcvd-3] == '_'  &&
        //     echoBuffer[bytesRcvd-2] == '_'  &&
        //     echoBuffer[bytesRcvd-1] == '_')
        // {
        //     printf("hello");
        // }
        echoBuffer[bytesRcvd] = '\0';  /* Terminate the string! */
        // printf("%s", echoBuffer);      /* Print the echo buffer */
        odomData.append(echoBuffer);
    }

    printf("\n");    /* Print a final linefeed */

    close(sock);
    std::cout << odomData << std::endl;



    float odomArray[7];
    std::string delimiter = ",";
    std::string tmp;
    int counter;
    int pos_range_begin, pos_range_end;

    // std::cout << "test" << std::endl;
    // std::cout << dataString << std::endl;
    pos_range_begin = dataString.find_first_of("[");
    pos_range_end = dataString.find_first_of("]");
    std::string newData = dataString.substr(pos_range_begin,pos_range_end);
    // std::cout << pos_range_begin << std::endl;
    // std::cout << pos_range_end << std::endl;
    // std::cout << newData << std::endl;
    for( ; ; )
    {
        tmp = newData.substr(1, newData.find(delimiter) - 1);
        // std::cout << std::stold(tmp) << std::endl;
        // std::cout << tmp << " ";
        ranges.push_back(std::stof(tmp));
        // std::cout << ranges[357] << std::endl;
        counter++;
        if(tmp.find("]") < 20)
        {
            tmp = tmp.substr(0, tmp.find("]"));
            // std::cout << tmp << std::endl;
            ranges.push_back(std::stold(tmp));
            break;
        }
        newData.erase(0, newData.find(delimiter) + 1);
        // std::cout << newData << std::endl;
    }
    // std::cout << std::endl << std::endl;
    
    int j = 0;
    for(float i : ranges)
    {
        rangesArray[j] = i;
        // std::cout << rangesArray[j] <<  " " ;
        ++j;
        // << std::endl;
    }




    


    usleep(10);
    Message_Odom odomMessage;
    odomMessage.type = PROD_MSG;
    // odomMessage.data = echoBuffer;
    for(int i = 0; i < 7; i++)
    {
        odomMessage.data[i] = rand()%5 * 0.1;
    }
    usleep(10);

    if(msgsnd(msgqid_odom, &odomMessage, sizeof(float) * 20, 0) != 0)
    {
        std::cout << "msgq_odom send failed" << std::endl;
    };
    std::cout << "test" << std::endl;

    usleep(10000);

    }


    exit(0);
}

void producerHandler (int sig)
{
    exit(EXIT_SUCCESS);

}
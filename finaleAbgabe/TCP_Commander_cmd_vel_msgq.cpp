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

// keys for msgqs
key_t KEY_VEL       = 830;
key_t KEY_VEL_ACT   = 730;

// program handler to exit succesfully
void consumerHandler(int sig);

// msgq IDs
int msgqid_vel, msgqid_vel_act;

// message type
enum MessageType {PROD_MSG=1, CONS_MSG };

// structs for specific messages
struct Message_Vel
{
    long type;
    float velocities[2];
};

struct Message_Act
{
    long type;
    bool act;
};


void DieWithError(char* errorMessage);  /* Error handling function */

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* server address */
    unsigned short echoServPort = 9999;     /* server port */
    const char *servIP = "192.168.100.55";     /* Server IP address (dotted quad) */
    char echoString[] = "---START---{\"linear\": 0.00, \"angular\": 0.0}___END___";               /* String to send to cmd_vel */
    unsigned int echoStringLen;      /* Length of string to echo */
                                    
    // attach/create to msgqs
    msgqid_vel = msgget(KEY_VEL, 0666 | IPC_CREAT);
    if (msgqid_vel == -1) {
      std::cerr << "msgget vel cons failed\n";
      exit(EXIT_FAILURE);
    }

    msgqid_vel_act = msgget(KEY_VEL_ACT, 0666 | IPC_CREAT);
    if (msgqid_vel_act == -1) {
      std::cerr << "msgget odom prod failed\n";
      exit(EXIT_FAILURE);
    }

    signal(SIGINT, consumerHandler);
    std::cout << "I am the vel cons" << std::endl;

    // send vel_act for odom
    Message_Act actVel;

    actVel.type = PROD_MSG;
    actVel.act = true;

    if(msgsnd(msgqid_vel_act, &actVel, sizeof(bool), 0) != 0)
    {
        std::cout << "msgq_odom send failed" << std::endl;
    };

    while(true)
    {
        // receiving msg from botControl and cast it in the cmd_vel string for TCP connection
        Message_Vel velMessage;

        msgrcv(msgqid_vel, &velMessage, sizeof(float) * 2, PROD_MSG, 0);
        std::cout << "Read: " << velMessage.velocities[0] << " m/s " << velMessage.velocities[1] << " rad/s" << std::endl;        
        
        std::string str_stringecho = "---START---{\"linear\": " + std::to_string(velMessage.velocities[0]) + ", \"angular\": " + std::to_string(velMessage.velocities[1]) + "}___END___";
        std::strcpy(echoString, str_stringecho.c_str());


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

        close(sock);

        

        // send vel_act for odom
        actVel.type = PROD_MSG;
        actVel.act = true;

        if(msgsnd(msgqid_vel_act, &actVel, sizeof(bool), 0) != 0)
        {
            std::cout << "msgq_odom send failed" << std::endl;
        };

        usleep(10); // 10us

    }


    exit(0);
}

void consumerHandler(int sig)
{
    exit(EXIT_SUCCESS);
}

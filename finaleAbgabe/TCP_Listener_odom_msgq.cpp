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
key_t KEY_ODOM = 810;
key_t KEY_ODOM_ACT       = 710;
key_t KEY_VEL_ACT        = 730;

// program handler to exit succesfully
void producerHandler (int sig);

// msgq IDs
int msgqid_odom, msgqid_odom_act, msgqid_vel_act; 

// message type
enum MessageType { PROD_MSG=1, CONS_MSG };

// structs for specific messages
struct Message_Odom
{
    long type;
    double pose[6];
};

struct Message_Act
{
    long type;
    bool act;
};

// variables for data extraction
// std::string dataString="---START---{\"header\": {\"seq\": 67769, \"stamp\": {\"secs\": 1677511096, \"nsecs\": 329690933}, \"frame_id\": \"odom\"}, \"child_frame_id\": \"base_footprint\", \"pose\": {\"pose\": {\"position\": {\"x\": -8.901372348191217e-05, \"y\": 6.059087172616273e-05, \"z\": 0.0}, \"orientation\": {\"x\": 0.0, \"y\": 0.0, \"z\": -0.5472193956375122, \"w\": 0.8369892239570618}}, \"covariance\": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]}, \"twist\": {\"twist\": {\"linear\": {\"x\": 0.0003163835790473968, \"y\": 0.0, \"z\": 0.0}, \"angular\": {\"x\": 0.0, \"y\": 0.0, \"z\": 0.0009506940841674805}}, \"covariance\": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]}}___END___";               /* String to send to echo server */
std::string dataString;
std::string delimiter = ",";
std::string tmp;
int pos_x_min,pos_x_max, pos_y_min, pos_y_max, pos_range_begin, pos_range_end;

// extracted data 
double arPose[6];

// function to extract data
void poseExtract();

void DieWithError(char *errorMessage);  /* Error handling function */

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* server address */
    unsigned short echoServPort = 9998;     /* server port */
    const char *servIP = "192.168.100.55";     /* Server IP address (dotted quad) */
    const char *echoString = "---START---{\"header\": {\"seq\": 67769, \"stamp\": {\"secs\": 1677511096, \"nsecs\": 329690933}, \"frame_id\": \"odom\"}, \"child_frame_id\": \"base_footprint\", \"pose\": {\"pose\": {\"position\": {\"x\": -8.901372348191217, \"y\": 6.059087172616273, \"z\": 0.0}, \"orientation\": {\"x\": 0.0, \"y\": 0.0, \"z\": -0.5472193956375122, \"w\": 0.8369892239570618}}, \"covariance\": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]}, \"twist\": {\"twist\": {\"linear\": {\"x\": 0.0003163835790473968, \"y\": 0.0, \"z\": 0.0}, \"angular\": {\"x\": 0.0, \"y\": 0.0, \"z\": 0.0009506940841674805}}, \"covariance\": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]}}___END___";               /* String to send to echo server */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for received string */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() 
                                        and total bytes read */

    // attach/create to msgqs
    msgqid_odom = msgget(KEY_ODOM, 0666 | IPC_CREAT);
    if (msgqid_odom == -1) {
      std::cerr << "msgget odom prod failed\n";
      exit(EXIT_FAILURE);
    }

    msgqid_odom_act = msgget(KEY_ODOM_ACT, 0666 | IPC_CREAT);
    if (msgqid_odom_act == -1) {
      std::cerr << "msgget odom prod failed\n";
      exit(EXIT_FAILURE);
    }

    msgqid_vel_act = msgget(KEY_VEL_ACT, 0666 | IPC_CREAT);
    if (msgqid_vel_act == -1) {
      std::cerr << "msgget odom prod failed\n";
      exit(EXIT_FAILURE);
    }

    signal(SIGINT, producerHandler);
    std::cout << "I am the odom prod" << std::endl;

    while(true)
    {
        // receiving vel_act msg
        Message_Act actVel;
        msgrcv(msgqid_vel_act, &actVel, sizeof(bool), PROD_MSG, 0);

        // receiving odom msg over TCP
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

        /* Receive the same string back from the server */
        totalBytesRcvd = 0;
        printf("Received: ");                /* Setup to print the echoed string */
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
            echoBuffer[bytesRcvd] = '\0';  /* Terminate the string! */
            dataString.append(echoBuffer);
        }

        printf("\n");    /* Print a final linefeed */
        echoBuffer[bytesRcvd-1] = 'X';

        close(sock);

        usleep(10);

        // extract pose of received message
        poseExtract();

        // sending extracted odom data and odom_act
        Message_Odom odomMessage;
        Message_Act actOdom;
        odomMessage.type = PROD_MSG;
        actOdom.type = PROD_MSG;
        actOdom.act = true;
        for(int i = 0; i < 6; i++)
        {
            odomMessage.pose[i] = arPose[i];
        }
        usleep(10);

        // sending pose over msgq to main program
        if(msgsnd(msgqid_odom, &odomMessage, sizeof(double) * 6, 0) != 0)
        {
            std::cout << "msgq_odom send failed" << std::endl;
        };

        if(msgsnd(msgqid_odom_act, &actOdom, sizeof(bool), 0) != 0)
        {
            std::cout << "msgq_odom_act send failed" << std::endl;
        };

        usleep(1000); // 1ms

        }


    exit(0);
}

void producerHandler(int sig)
{
    exit(EXIT_SUCCESS);
}


void poseExtract()
{
    // extracting data over substringing received string and casting values into double
    pos_x_min = dataString.find_first_of("x") + 2;
    pos_x_max = dataString.find_first_of("y");
    std::string pose_x = dataString.substr(pos_x_min,pos_x_max);
    tmp = pose_x.substr(1, pose_x.find(delimiter) - 1);
    pose_x = tmp;
    std::cout << "Pos x: " << pose_x << std::endl;

    pos_y_min = dataString.find_first_of("y") + 2;
    pos_y_max = dataString.find_first_of("z");
    std::string pose_y = dataString.substr(pos_y_min,pos_y_max);
    tmp = pose_y.substr(1, pose_y.find(delimiter) - 1);
    pose_y = tmp;
    std::cout << "Pos y: " << pose_y << std::endl;

    std::string orient=dataString.substr(dataString.find("orientation") + 2, dataString.find("covariance"));
    std::string orient_x = orient.substr(orient.find_first_of("x") + 2, orient.find_first_of("y"));
    //int test=orient.find("x");
    tmp = orient_x.substr(1, orient_x.find(delimiter) - 1);
    orient_x = tmp;
    std::cout << "Orientation x: " << orient_x << std::endl;

    std::string orient_y = orient.substr(orient.find_first_of("y") + 2, orient.find_first_of("z"));
    tmp = orient_y.substr(1, orient_y.find(delimiter) - 1);
    orient_y = tmp;
    std::cout << "Orientation y: " << orient_y << std::endl;

    std::string orient_z = orient.substr(orient.find_first_of("z") + 2, orient.find_first_of("w"));
    tmp = orient_z.substr(1, orient_z.find(delimiter) - 1);
    orient_z = tmp;
    std::cout << "Orientation z: " << orient_z << std::endl;

    std::string orient_w = orient.substr(orient.find_first_of("w") + 2, orient.find_first_of("}}"));
    tmp = orient_w.substr(1, orient_w.find(delimiter) - 3);
    orient_w = tmp;
    std::cout << "Orientation w: " << orient_w << std::endl;

    arPose[0] = stod(pose_x);
    arPose[1] = stod(pose_y);
    arPose[2] = stod(orient_x);
    arPose[3] = stod(orient_y);
    arPose[4] = stod(orient_z);
    arPose[5] = stod(orient_w);

    dataString.clear();
}

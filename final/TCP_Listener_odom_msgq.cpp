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
    double pose[6];
};

// std::string dataString="---START---{\"header\": {\"seq\": 67769, \"stamp\": {\"secs\": 1677511096, \"nsecs\": 329690933}, \"frame_id\": \"odom\"}, \"child_frame_id\": \"base_footprint\", \"pose\": {\"pose\": {\"position\": {\"x\": -8.901372348191217e-05, \"y\": 6.059087172616273e-05, \"z\": 0.0}, \"orientation\": {\"x\": 0.0, \"y\": 0.0, \"z\": -0.5472193956375122, \"w\": 0.8369892239570618}}, \"covariance\": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]}, \"twist\": {\"twist\": {\"linear\": {\"x\": 0.0003163835790473968, \"y\": 0.0, \"z\": 0.0}, \"angular\": {\"x\": 0.0, \"y\": 0.0, \"z\": 0.0009506940841674805}}, \"covariance\": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]}}___END___";               /* String to send to echo server */
std::string dataString;
std::string delimiter = ",";
std::string tmp;

int pos_x_min,pos_x_max, pos_y_min, pos_y_max, pos_range_begin, pos_range_end;
double arPose[6];

void poseExtract();

void DieWithError(char *errorMessage);  /* Error handling function */

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort = 9998;     /* Echo server port */
    const char *servIP = "192.168.100.50";     /* Server IP address (dotted quad) */
    // const char *servIP = "172.19.178.59";
    const char *echoString = "---START---{\"header\": {\"seq\": 67769, \"stamp\": {\"secs\": 1677511096, \"nsecs\": 329690933}, \"frame_id\": \"odom\"}, \"child_frame_id\": \"base_footprint\", \"pose\": {\"pose\": {\"position\": {\"x\": -8.901372348191217, \"y\": 6.059087172616273, \"z\": 0.0}, \"orientation\": {\"x\": 0.0, \"y\": 0.0, \"z\": -0.5472193956375122, \"w\": 0.8369892239570618}}, \"covariance\": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]}, \"twist\": {\"twist\": {\"linear\": {\"x\": 0.0003163835790473968, \"y\": 0.0, \"z\": 0.0}, \"angular\": {\"x\": 0.0, \"y\": 0.0, \"z\": 0.0009506940841674805}}, \"covariance\": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]}}___END___";               /* String to send to echo server */
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
        dataString.append(echoBuffer);
    }

    printf("\n");    /* Print a final linefeed */
    echoBuffer[bytesRcvd-1] = 'X';

    close(sock);

    usleep(10);


    // extract pose of received message
    poseExtract();

    Message_Odom odomMessage;
    odomMessage.type = PROD_MSG;
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

    usleep(10); // 10ms
    // usleep(2000000);

    }


    exit(0);
}

void producerHandler(int sig)
{
    exit(EXIT_SUCCESS);
}


void poseExtract()
{

    using namespace std;
    // pos_range_begin = dataString.find_first_of("[");
    // pos_range_end = dataString.find_first_of("]");
    // std::string newData = dataString.substr(pos_range_begin,pos_range_end);

    pos_x_min = dataString.find_first_of("x")+2;
    pos_x_max = dataString.find_first_of("y");
    std::string pose_x = dataString.substr(pos_x_min,pos_x_max);
    // cout << pose_x << endl;
    // string pose=pose_x;
    tmp = pose_x.substr(1, pose_x.find(delimiter) - 1);
    // cout<< tmp<< endl;
    pose_x=tmp;
    cout << "Pos x: " << pose_x<<endl;


    pos_y_min = dataString.find_first_of("y")+2;
    pos_y_max = dataString.find_first_of("z");
    std::string pose_y = dataString.substr(pos_y_min,pos_y_max);
    tmp = pose_y.substr(1, pose_y.find(delimiter) - 1);
    // cout<< tmp<< endl;
    pose_y=tmp;
    cout << "Pos y: " << pose_y << endl;

    string orient=dataString.substr(dataString.find("orientation")+2, dataString.find("covariance"));
    // cout << dataString.find("orientation")+2<< endl;
    // cout<<orient<<endl;
    std::string orient_x = orient.substr(orient.find_first_of("x")+2,orient.find_first_of("y"));
    //int test=orient.find("x");
    tmp = orient_x.substr(1, orient_x.find(delimiter) - 1);
    orient_x=tmp;
    cout <<"Orientation x: "<<orient_x << endl;

    std::string orient_y = orient.substr(orient.find_first_of("y")+2,orient.find_first_of("z"));
    tmp = orient_y.substr(1, orient_y.find(delimiter) - 1);
    orient_y=tmp;
    cout <<"Orientation y: "<<orient_y << endl;

    std::string orient_z = orient.substr(orient.find_first_of("z")+2,orient.find_first_of("w"));
    tmp = orient_z.substr(1, orient_z.find(delimiter) - 1);
    orient_z=tmp;
    cout <<"Orientation z: "<<orient_z << endl;

    std::string orient_w = orient.substr(orient.find_first_of("w")+2,orient.find_first_of("}}"));
    tmp = orient_w.substr(1, orient_w.find(delimiter) - 3);
    orient_w=tmp;
    cout <<"Orientation w: "<<orient_w << endl;

    arPose[0] = stod(pose_x);
    arPose[1] = stod(pose_y);
    arPose[2] = stod(orient_x);
    arPose[3] = stod(orient_y);
    arPose[4] = stod(orient_z);
    arPose[5] = stod(orient_w);

    dataString.clear();
}

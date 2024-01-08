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

#include <vector>

#define RCVBUFSIZE 512   /* Size of receive buffer */

key_t KEY_LASER = 820;
void producerHandler (int sig);
int msgqid_laser; 
enum MessageType { PROD_MSG=1, CONS_MSG };
struct Message_Laser
{
    long type;
    double scan[360];
};

std::string dataString;
double rangesArray[360];

void scanExtract();

void DieWithError(char *errorMessage);  /* Error handling function */

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort = 9997;     /* Echo server port */
    // const char *servIP = "192.168.100.54";      // local ip of desktop                /* Server IP address (dotted quad) */
    const char *servIP = "172.19.178.59";
    const char *echoString = "---START---{\"header\": {\"seq\": 19420, \"stamp\": {\"secs\": 1677511007, \"nsecs\": 782577255}, \"frame_id\": \"base_scan\"}, \"angle_min\": 0.0, \"angle_max\": 6.2657318115234375, \"angle_increment\": 0.01745329238474369, \"time_increment\": 0.0005592841189354658, \"scan_time\": 0.20134228467941284, \"range_min\": 0.11999999731779099, \"range_max\": 3.5, \"ranges\": [2.2780001163482666, 2.2890000343322754, 2.302999973297119, 2.319999933242798, 2.3289999961853027, 2.3320000171661377, 2.3369998931884766, 2.3580000400543213, 2.380000114440918, 2.4019999504089355, 2.3929998874664307, 2.4110000133514404, 2.443000078201294, 2.4560000896453857, 2.4809999465942383, 2.4719998836517334, 2.51200008392334, 2.5309998989105225, 2.566999912261963, 2.617000102996826, 2.615999937057495, 2.628000020980835, 2.684999942779541, 2.6649999618530273, 2.7230000495910645, 2.765000104904175, 2.7850000858306885, 2.8359999656677246, 2.8469998836517334, 0.16500000655651093, 0.16500000655651093, 0.16200000047683716, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.21400000154972076, 0.21400000154972076, 0.23000000417232513, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.628000020980835, 2.563999891281128, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.9709999561309814, 1.934000015258789, 1.8910000324249268, 1.8869999647140503, 1.8539999723434448, 1.2309999465942383, 1.8040000200271606, 1.909999966621399, 1.8680000305175781, 1.9140000343322754, 1.968999981880188, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.09399999678134918, 0.09399999678134918, 0.09600000083446503, 0.10000000149011612, 0.10400000214576721, 0.10700000077486038, 0.11100000143051147, 0.11400000005960464, 0.11900000274181366, 0.12399999797344208, 0.1289999932050705, 0.1340000033378601, 0.14000000059604645, 0.1459999978542328, 0.15399999916553497, 0.16200000047683716, 0.17100000381469727, 0.18199999630451202, 0.19200000166893005, 0.20399999618530273, 0.2199999988079071, 0.23499999940395355, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.121000051498413, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2160000056028366, 0.2160000056028366, 0.0, 0.0, 0.0, 0.0, 0.0, 2.7730000019073486, 2.7269999980926514, 2.6500000953674316, 2.6489999294281006, 2.568000078201294, 2.552000045776367, 2.5160000324249268, 2.496000051498413, 2.427999973297119, 2.4110000133514404, 2.385999917984009, 2.3510000705718994, 2.3440001010894775, 2.2809998989105225, 2.2829999923706055, 2.234999895095825, 2.24399995803833, 2.255000114440918, 2.190999984741211, 2.187000036239624, 2.1740000247955322, 2.1489999294281006, 2.131999969482422, 2.134000062942505, 2.0969998836517334, 2.0989999771118164, 2.11299991607666, 2.078000068664551, 2.0880000591278076, 2.062999963760376, 2.052000045776367, 2.059999942779541, 2.0409998893737793, 2.0269999504089355, 2.0420000553131104, 2.0390000343322754, 2.0230000019073486, 2.015000104904175, 2.0380001068115234, 2.000999927520752, 1.9279999732971191, 1.8960000276565552, 1.8880000114440918, 1.8969999551773071, 2.2300000190734863, 2.252000093460083, 2.236999988555908, 2.242000102996826, 2.253999948501587, 2.253999948501587, 2.25, 2.2639999389648438, 2.2679998874664307], \"intensities\": [844.0, 837.0, 819.0, 841.0, 824.0, 834.0, 800.0, 797.0, 812.0, 805.0, 776.0, 760.0, 775.0, 758.0, 752.0, 729.0, 715.0, 712.0, 708.0, 704.0, 647.0, 656.0, 649.0, 616.0, 622.0, 602.0, 584.0, 557.0, 191.0, 333.0, 503.0, 1962.0, 78.0, 0.0, 0.0, 65.0, 42.0, 87.0, 47.0, 0.0, 38.0, 40.0, 0.0, 45.0, 69.0, 77.0, 59.0, 49.0, 64.0, 84.0, 53.0, 0.0, 0.0, 0.0, 34.0, 0.0, 47.0, 0.0, 0.0, 0.0, 0.0, 47.0, 0.0, 0.0, 47.0, 0.0, 0.0, 0.0, 0.0, 2033.0, 10760.0, 1179.0, 0.0, 0.0, 0.0, 0.0, 0.0, 47.0, 0.0, 0.0, 0.0, 787.0, 474.0, 162.0, 339.0, 327.0, 351.0, 338.0, 353.0, 340.0, 328.0, 459.0, 597.0, 747.0, 727.0, 154.0, 852.0, 159.0, 565.0, 676.0, 545.0, 435.0, 84.0, 47.0, 47.0, 0.0, 64.0, 88.0, 0.0, 0.0, 55.0, 47.0, 128.0, 747.0, 249.0, 0.0, 60.0, 0.0, 0.0, 47.0, 47.0, 0.0, 0.0, 47.0, 0.0, 47.0, 0.0, 0.0, 47.0, 0.0, 47.0, 0.0, 0.0, 47.0, 0.0, 0.0, 0.0, 47.0, 47.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 47.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 47.0, 0.0, 0.0, 47.0, 0.0, 0.0, 0.0, 0.0, 47.0, 47.0, 0.0, 0.0, 0.0, 47.0, 0.0, 0.0, 0.0, 47.0, 47.0, 0.0, 0.0, 47.0, 0.0, 47.0, 0.0, 0.0, 47.0, 0.0, 47.0, 0.0, 47.0, 0.0, 0.0, 0.0, 0.0, 47.0, 47.0, 47.0, 0.0, 0.0, 0.0, 0.0, 0.0, 47.0, 47.0, 0.0, 47.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 47.0, 0.0, 0.0, 0.0, 47.0, 0.0, 41.0, 57.0, 83.0, 0.0, 316.0, 758.0, 1696.0, 279.0, 1531.0, 2154.0, 2278.0, 2443.0, 2732.0, 2681.0, 2618.0, 2549.0, 2335.0, 2079.0, 1959.0, 1638.0, 1472.0, 1372.0, 1252.0, 1088.0, 978.0, 759.0, 584.0, 515.0, 242.0, 224.0, 245.0, 101.0, 101.0, 79.0, 56.0, 0.0, 0.0, 0.0, 66.0, 88.0, 86.0, 79.0, 101.0, 111.0, 47.0, 285.0, 0.0, 67.0, 152.0, 47.0, 0.0, 0.0, 47.0, 0.0, 47.0, 0.0, 47.0, 47.0, 0.0, 0.0, 0.0, 0.0, 47.0, 0.0, 47.0, 0.0, 47.0, 47.0, 0.0, 0.0, 47.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 47.0, 0.0, 0.0, 49.0, 0.0, 0.0, 37.0, 57.0, 672.0, 105.0, 92.0, 47.0, 0.0, 0.0, 0.0, 270.0, 433.0, 467.0, 476.0, 479.0, 506.0, 527.0, 502.0, 522.0, 528.0, 539.0, 558.0, 559.0, 565.0, 570.0, 561.0, 583.0, 609.0, 653.0, 668.0, 676.0, 700.0, 698.0, 728.0, 724.0, 743.0, 752.0, 759.0, 764.0, 772.0, 785.0, 775.0, 791.0, 810.0, 805.0, 827.0, 826.0, 828.0, 832.0, 842.0, 2417.0, 2054.0, 2062.0, 2682.0, 839.0, 899.0, 846.0, 852.0, 865.0, 860.0, 877.0, 882.0, 833.0]}___END___";               /* String to send to echo server */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    unsigned int echoStringLen;      /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() 
                                        and total bytes read */


    msgqid_laser = msgget(KEY_LASER, 0666 | IPC_CREAT);
    if (msgqid_laser == -1) {
      std::cerr << "msgget laser prod failed\n";
      exit(EXIT_FAILURE);
    }
    signal(SIGINT, producerHandler);
    std::cout << "I am the laser prod" << std::endl;



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
    if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
        DieWithError("send() sent a different number of bytes than expected");

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

    // std::cout << dataString << std::endl;

    scanExtract();


    Message_Laser laserMessage;
    laserMessage.type = PROD_MSG;
    
    for(int i = 0; i < 360; i++)
    {
        laserMessage.scan[i] = rangesArray[i];
    }
    
    usleep(10);

    if(msgsnd(msgqid_laser, &laserMessage, sizeof(double) * 360, 0) != 0)
    {
        std::cout << "msgq_laser send failed" << std::endl;
    };

    // usleep(10000);
    usleep(2000000);

    }


    exit(0);
}

void producerHandler(int sig)
{
    exit(EXIT_SUCCESS);
}

void scanExtract()
{
    std::vector<double> ranges;
    std::string delimiter = ",";
    std::string tmp;
    int counter;
    int pos_range_begin, pos_range_end;

    pos_range_begin = dataString.find_first_of("[");
    pos_range_end = dataString.find_first_of("]");
    std::string newData = dataString.substr(pos_range_begin,pos_range_end);

    while(true)
    {
        tmp = newData.substr(1, newData.find(delimiter) - 1);
        // std::cout << std::stold(tmp) << std::endl;
        // std::cout << tmp << " ";
        ranges.push_back(stod(tmp));
        // std::cout << ranges[357] << std::endl;
        counter++;
        if(tmp.find("]") < 20)
        {
            tmp = tmp.substr(0, tmp.find("]"));
            // std::cout << tmp << std::endl;
            ranges.push_back(stod(tmp));
            break;
        }
        newData.erase(0, newData.find(delimiter) + 1);
        // std::cout << newData << std::endl;
    }
    // std::cout << std::endl << std::endl;
    
    int j = 0;
    for(double i : ranges)
    {
        rangesArray[j] = i;
        std::cout << rangesArray[j] <<  " " ;
        // << std::endl;
        j++;
    }
    
    ranges.clear();

    dataString.clear();
}
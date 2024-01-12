#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// declaration of msgq
key_t KEY_ODOM          = 810;
key_t KEY_LASER         = 820;
key_t KEY_VEL           = 830;
key_t KEY_VEL_ACT        = 730;

int msgqid_odom, msgqid_laser, msgqid_vel, msgqid_vel_act;

enum MessageType {PROD_MSG=1, CONS_MSG};

struct Message_Vel
{
    long type;
    float velocities[2];
};

struct Message_Laser
{
    long type;
    double scan[360];
};

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

void msgqHandler(int sig)
{
    exit(EXIT_SUCCESS);
}

struct Pose2d
{
    double x;
    double y;
    double theta;
};


double PI = 3.141592653589793238463;

const unsigned int numGoPoses = 7;
Pose2d goPoses[numGoPoses];
Pose2d posePole;

Pose2d currentPose;
// double start_x = 0;
// double start_y = 0;
// double start_theta = 0;

Pose2d currentGoal;
// double goal_x = 0;
// double goal_y = 0;
// double goal_theta = 0;

//double kp = 0.2;
//double k_alpha = 0.7;
//double k_beta = -0.8;

// double kp = 0.3;
// double k_alpha = 0.8;
// double k_beta = -0.15;

double kp = 0.5;
double k_alpha = 3.0;
double k_beta = 1.0;

Pose2d currentDeltaPose;
// double delta_x = 0.0;
// double delta_y = 0.0;
// double delta_theta = 0.0;

double rho = 0;
double alpha = 0;
double beta = 0;

double vt = 0;
double wt = 0;

bool drive = true;

Message_Odom odomMsg;
Message_Laser laserMsg;
Message_Vel velMsg;

int goalCount = 0;
int poleCounter = 0;


bool goPosesDefined = false;

// für berechnung der poses
const int numOfScansForDistanceToPole = 50;
double distanceToPoleMiddle = 0;
double poleDiameter = 0.085;
double distanceFromPole = poleDiameter + 0.30;   // normalabstand von der pole

bool driveBool = false;

// variables for laserscan box wall
double distanceLeft = 0, distanceRight = 0, laserDistanceMiddle = 0, deltaDistanceWall = 0;
double k_wall = 0.5;
bool wallFollowing = false;

Pose2d firstgoaltest;
Pose2d firstodomtest;

const double VT_MAX = 0.07;
const double WT_MAX = 1.0;




// double pose_x = 4;
// double pose_y = -2;

// double position_x=0; //to define a delta variable
// double position_y=0;
// double position_theta=0;

// double r=0.16; //difference between the wheels
// double p_w=r*10;

// limit angles to -180 to 180 degrees
double limitAng(double param)
{
    if(param > PI && param <= PI)
    {

    }
    else if(param < 0)
    {
        while(param < -PI)
        {
            param = param + 2*PI;
        }
    }
    else if(param > 0)
    {
        while(param > PI)
        {
            param = param - 2*PI;
        }
    }

    return param;
}



int main()
{
    msgqid_odom = msgget(KEY_ODOM, 0666 | IPC_CREAT);
    if (msgqid_odom == -1) {
      std::cerr << "msgget laser prod failed\n";
      exit(EXIT_FAILURE);
    }

    msgqid_laser = msgget(KEY_LASER, 0666 | IPC_CREAT);
    if (msgqid_laser == -1) {
      std::cerr << "msgget laser prod failed\n";
      exit(EXIT_FAILURE);
    }

    msgqid_vel = msgget(KEY_VEL, 0666 | IPC_CREAT);
    if (msgqid_vel == -1) {
      std::cerr << "msgget laser prod failed\n";
      exit(EXIT_FAILURE);
    }

    msgqid_vel_act = msgget(KEY_VEL_ACT, 0666 | IPC_CREAT);
    if (msgqid_vel_act == -1) {
      std::cerr << "msgget odom prod failed\n";
      exit(EXIT_FAILURE);
    }



    while(true)
    {
        // DONE odometrie theta
        // DONE basic controller with sending velocities
        // DONE goPoses definierung
        // DONE distanceToPoleMiddle, distanceFromPole
        // DONE auswertung Laserscan
        // DONE wallfollowing

        // receiving odom and laser data from msg queue
        msgrcv(msgqid_odom, &odomMsg, sizeof(double) * 6, PROD_MSG, 0);
        msgrcv(msgqid_laser, &laserMsg, sizeof(double) * 360, PROD_MSG, 0);

        // for(double i : odomMsg.pose)
        // {
        //     std::cout << i << std::endl;
        // }

        // for(double i : laserMsg.scan)
        // {
        //     std::cout << i << std::endl;
        // }

        // from: https://stackoverflow.com/questions/5782658/extracting-yaw-from-a-quaternion
        // double yaw = atan2(2.0*(q.y*q.z + q.w*q.x), q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z);
        // double pitch = asin(-2.0*(q.x*q.z - q.w*q.y));
        // double roll = atan2(2.0*(q.x*q.y + q.w*q.z), q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z);

        // double yaw_z = atan2( 2.0 * (w*z + x*y), 1.0 - 2.0 *(y*y + z*z));

        // std::cout << odomMsg.pose[2] << " " << odomMsg.pose[3] << " " << odomMsg.pose[4] << " " << odomMsg.pose[5] << "\n-----------------------------" << std::endl;

        currentPose.x = odomMsg.pose[0];
        currentPose.y = odomMsg.pose[1];
        // möglicherweise muss atan2 * -1 gerechnet werden
        currentPose.theta = atan2(  (2.0 * (odomMsg.pose[5]*odomMsg.pose[4] + odomMsg.pose[2]*odomMsg.pose[3])),
                                    (1.0 - 2.0*(pow(odomMsg.pose[3], 2) + pow(odomMsg.pose[4], 2)))
                                    );


        // goPoses definition
        if(goPosesDefined == false)
        {
            // laserscan auswertung für poledistance
            for(int i = 0; i < numOfScansForDistanceToPole; i++)
            {
                Message_Vel actVelMsg;

                actVelMsg.type = PROD_MSG;
                actVelMsg.velocities[0] = 0.0;
                actVelMsg.velocities[1] = 0.0;


                if(msgsnd(msgqid_vel, &actVelMsg, sizeof(float) * 2, 0) != 0)
                {
                    std::cout << "msgq_odom send failed" << std::endl;
                };

                msgrcv(msgqid_odom, &odomMsg, sizeof(double) * 6, PROD_MSG, 0);
                int tmp = msgrcv(msgqid_laser, &laserMsg, sizeof(double) * 360, PROD_MSG, 0);
                if(tmp > 0)
                {
                    // double laserToPole = laserMsg.scan[358] + laserMsg.scan[359] + laserMsg.scan[0] + laserMsg.scan[1] + laserMsg.scan[2];
                    // laserToPole = laserToPole / 5;
                    // distanceToPoleMiddle = distanceToPoleMiddle + laserToPole + poleDiameter/2;

                    double laserToPole = laserMsg.scan[0];
                    if(laserMsg.scan[0] > 0.0)
                    {

                        distanceToPoleMiddle = distanceToPoleMiddle + laserToPole + poleDiameter/2;
                        std::cout << "laserdata: " << laserMsg.scan[0] << std::endl;
                        poleCounter++;

                    }


                    tmp = 0;
                }
            }

            distanceToPoleMiddle = distanceToPoleMiddle / poleCounter;
            std::cout << "distance_middle: " << distanceToPoleMiddle << std::endl;

            // gettin wall distance
            distanceLeft = 0;
            distanceRight = 0;

            for(int i = 268; i < 268 + 5; i++)
            {
                distanceLeft = distanceLeft + laserMsg.scan[i];
            }
            distanceLeft = distanceLeft / 5;

            for(int i = 88; i < 88 + 5; i++)
            {
                distanceRight = distanceRight + laserMsg.scan[i];
            }
            distanceRight = distanceRight / 5;

            // anfangsposition von y
            laserDistanceMiddle = (distanceLeft + distanceRight) / 2;

            // REAL Positions new with first theta
            goPoses[0].x = currentPose.x;
            goPoses[0].y = currentPose.y;
            goPoses[0].theta = currentPose.theta;
            std:: cout << "Go0x: " <<goPoses[0].x << std::endl;
            std:: cout << "Go0y: " <<goPoses[0].y << std::endl;

            goPoses[6].x = goPoses[0].x;
            goPoses[6].y = goPoses[0].y;
            goPoses[6].theta = limitAng( goPoses[0].theta + (180.0 * PI / 180) ) ;
            std:: cout << "Go7x: " <<goPoses[7].x << std::endl;
            std:: cout << "Go7y: " <<goPoses[7].y << std::endl;


            posePole.x = goPoses[0].x + cos(goPoses[0].theta) * (distanceToPoleMiddle);
            posePole.y = goPoses[0].y + sin(goPoses[0].theta) * (distanceToPoleMiddle);
            posePole.theta = limitAng( goPoses[0].theta );

            std:: cout << "polex: " << posePole.x << std::endl;
            std:: cout << "poley: " << posePole.y << std::endl;
            std::cout << "posePole theta" << posePole.theta << std::endl;

            goPoses[1].x = posePole.x + cos(posePole.theta + PI) * (distanceFromPole);
            goPoses[1].y = posePole.y + sin(posePole.theta + PI) * (distanceFromPole);
            goPoses[1].theta = limitAng( posePole.theta + (-0.0 * PI / 180) );
            std:: cout << "Go1x: " <<goPoses[1].x << std::endl;
            std:: cout << "Go1y: " <<goPoses[1].y << std::endl;

            goPoses[2].x = posePole.x + cos(posePole.theta - PI/2) * (distanceFromPole);
            goPoses[2].y = posePole.y + sin(posePole.theta - PI/2) * (distanceFromPole);
            goPoses[2].theta = limitAng( posePole.theta + 0.0 );
            std:: cout << "Go2x: " <<goPoses[2].x << std::endl;
            std:: cout << "Go2y: " <<goPoses[2].y << std::endl;

            goPoses[3].x = posePole.x + cos(posePole.theta) * (distanceFromPole);
            goPoses[3].y = posePole.y + sin(posePole.theta) * (distanceFromPole);
            goPoses[3].theta = limitAng( posePole.theta + (90.0 * PI / 180) );
            std:: cout << "Go3x: " <<goPoses[3].x << std::endl;
            std:: cout << "Go3y: " <<goPoses[3].y << std::endl;

            goPoses[4].x = posePole.x + cos(posePole.theta + PI/2) * (distanceFromPole);
            goPoses[4].y = posePole.y + sin(posePole.theta + PI/2) * (distanceFromPole);
            goPoses[4].theta = limitAng( posePole.theta + (178.0 * PI / 180) );
            std:: cout << "Go4x: " <<goPoses[4].x << std::endl;
            std:: cout << "Go4y: " <<goPoses[4].y << std::endl;

            goPoses[5].x = goPoses[1].x;
            goPoses[5].y = goPoses[1].y;
            goPoses[5].theta = limitAng( goPoses[1].theta + (-135.0 * PI / 180) );
            std:: cout << "Go5x: " <<goPoses[5].x << std::endl;
            std:: cout << "Go5y: " <<goPoses[5].y << std::endl;

            // goPoses[6].x = goPoses[5].x;
            // goPoses[6].y = goPoses[5].y;
            // goPoses[6].theta = limitAng( goPoses[0].theta + (180.0 * PI / 180) );
            // std:: cout << "Go6x: " <<goPoses[6].x << std::endl;
            // std:: cout << "Go6y: " <<goPoses[6].y << std::endl;


            // // REAL Positiona
            // goPoses[0].x = currentPose.x;
            // goPoses[0].y = currentPose.y;
            // goPoses[0].theta = currentPose.theta;
            // std:: cout << "Go0x: " <<goPoses[0].x << std::endl;
            // std:: cout << "Go0y: " <<goPoses[0].y << std::endl;

            // goPoses[7].x = goPoses[0].x;
            // goPoses[7].y = goPoses[0].y;
            // goPoses[7].theta = (180.0 * PI / 180)  ;
            // std:: cout << "Go7x: " <<goPoses[7].x << std::endl;
            // std:: cout << "Go7y: " <<goPoses[7].y << std::endl;

            // goPoses[1].x = goPoses[0].x + distanceToPoleMiddle - distanceFromPole;
            // goPoses[1].y = goPoses[0].y;
            // goPoses[1].theta = (-90.0 * PI / 180) ;
            // std:: cout << "Go1x: " <<goPoses[1].x << std::endl;
            // std:: cout << "Go1y: " <<goPoses[1].y << std::endl;

            // goPoses[2].x = goPoses[1].x + distanceFromPole;
            // goPoses[2].y = goPoses[1].y - distanceFromPole;
            // goPoses[2].theta =  0.0 ;
            // std:: cout << "Go2x: " <<goPoses[2].x << std::endl;
            // std:: cout << "Go2y: " <<goPoses[2].y << std::endl;

            // goPoses[3].x = goPoses[1].x + 2*distanceFromPole;
            // goPoses[3].y = goPoses[1].y;
            // goPoses[3].theta =  (90.0 * PI / 180) ;
            // std:: cout << "Go3x: " <<goPoses[3].x << std::endl;
            // std:: cout << "Go3y: " <<goPoses[3].y << std::endl;

            // goPoses[4].x = goPoses[1].x + distanceFromPole;
            // goPoses[4].y = goPoses[1].y + distanceFromPole;
            // goPoses[4].theta = (180.0 * PI / 180) ;
            // std:: cout << "Go4x: " <<goPoses[4].x << std::endl;
            // std:: cout << "Go4y: " <<goPoses[4].y << std::endl;

            // goPoses[5].x = goPoses[1].x;
            // goPoses[5].y = goPoses[1].y;
            // goPoses[5].theta = goPoses[1].theta ;
            // std:: cout << "Go5x: " <<goPoses[5].x << std::endl;
            // std:: cout << "Go5y: " <<goPoses[5].y << std::endl;

            // goPoses[6].x = goPoses[5].x;
            // goPoses[6].y = goPoses[5].y;
            // goPoses[6].theta =  (180.0 * PI / 180) ;
            // std:: cout << "Go6x: " <<goPoses[6].x << std::endl;
            // std:: cout << "Go6y: " <<goPoses[6].y << std::endl;


            // // TEST
            // goPoses[0].x = currentPose.x;
            // goPoses[0].y = currentPose.y;
            // goPoses[0].theta = currentPose.theta;

            // goPoses[7].x = goPoses[0].x;
            // goPoses[7].y = goPoses[0].y;
            // goPoses[7].theta = goPoses[0].theta + (180.0 * PI / 180);

            // goPoses[1].x = goPoses[0].x + 0.4;
            // goPoses[1].y = goPoses[0].y;
            // goPoses[1].theta = goPoses[0].theta + (45 * PI / 180);

            // goPoses[2].x = goPoses[1].x;
            // goPoses[2].y = goPoses[1].y + 0.4;
            // goPoses[2].theta = goPoses[0].theta + 0.0;

            // goPoses[3].x = goPoses[1].x + 2 * distanceFromPole;
            // goPoses[3].y = goPoses[1].y;
            // goPoses[3].theta = goPoses[0].theta + (90.0 * PI / 180);

            // goPoses[4].x = goPoses[1].x + distanceFromPole;
            // goPoses[4].y = goPoses[1].y + distanceFromPole;
            // goPoses[4].theta = goPoses[0].theta + (180.0 * PI / 180);

            // goPoses[5].x = goPoses[1].x;
            // goPoses[5].y = goPoses[1].y;
            // goPoses[5].theta = goPoses[1].theta;

            // goPoses[6].x = goPoses[5].x;
            // goPoses[6].y = goPoses[5].y;
            // goPoses[6].theta = goPoses[0].theta + (180.0 * PI / 180);



            currentGoal = goPoses[1];
            goalCount = 1;

            goPosesDefined = true;
            firstgoaltest = currentGoal;
            firstodomtest = currentPose;

            std::cout << "cgx: " << currentGoal.x << std::endl;
            std::cout << "cgy: " << currentGoal.y << std::endl;
            std::cout << "cgtheta: " << currentGoal.theta << std::endl;
            std::cout << "----------" << std::endl;

            std::string strTmp;
            std::cin >> strTmp;
        }


        // laserscan wall following
        distanceLeft = 0;
        distanceRight = 0;

        for(int i = 268; i < 268 + 5; i++)
        {
            distanceLeft = distanceLeft + laserMsg.scan[i];
        }
        distanceLeft = distanceLeft / 5;

        for(int i = 88; i < 88 + 5; i++)
        {
            distanceRight = distanceRight + laserMsg.scan[i];
        }
        distanceRight = distanceRight / 5;

        deltaDistanceWall = distanceLeft - distanceRight;

        if((laserDistanceMiddle + 0.2) > ((distanceLeft + distanceRight) / 2))      // möglich hinzufügen: if right >>> left -> wallfollowing false
        {
            wallFollowing = true;
        }
        else
        {
            wallFollowing = false;
        }









        // TO DO implementieren der driveBool abfrage

        // std::cin >> tmp;
        // if tmp == "drive";
        driveBool = true;

        if(driveBool == true)
        {
            currentDeltaPose.x = currentGoal.x - currentPose.x;
            currentDeltaPose.y = currentGoal.y - currentPose.y;
            currentDeltaPose.theta = currentGoal.theta  - currentPose.theta;

            currentDeltaPose.theta = limitAng( currentDeltaPose.theta );

            rho = sqrt(pow(currentDeltaPose.x, 2) + pow(currentDeltaPose.y, 2));
            alpha = (-currentPose.theta + atan2(currentDeltaPose.y, currentDeltaPose.x));
            // if(alpha >= PI){
            // alpha = PI;
            // }
            // else if(alpha < (-PI)){
            //     alpha = (-PI);
            // }
            alpha = limitAng( alpha );

            beta = -currentDeltaPose.theta - alpha;
            // if(beta < (-PI)){
            // beta = (-PI);
            // }
            // else if(beta > PI){
            //     beta = PI;
            // }

            beta = limitAng( beta );

            if(drive == true)
            {
                vt = kp * rho;
                wt = (k_alpha*alpha) + (k_beta*beta);

                // if(wallFollowing)
                // {
                //     wt = wt + k_wall*deltaDistanceWall;
                // }

                if(vt > VT_MAX)
                {
                    vt = VT_MAX;
                }

                if(wt > WT_MAX)
                {
                    wt = WT_MAX;
                }
                else if(wt < -WT_MAX)
                {
                    wt = -WT_MAX;
                }

                if((abs(currentDeltaPose.x) < 0.1) && (abs(currentDeltaPose.y) < 0.1) && (abs(currentDeltaPose.theta) < 0.2)  && goalCount < numGoPoses)
                {
                    vt = 0.0;
                    wt = 0.0;

                    goalCount++;
                    if(goalCount == numGoPoses)
                    {
                        // stop while and end program
                        drive = false;
                    }
                    currentGoal = goPoses[goalCount];
                    std::cout << goalCount << std::endl;

                }


            }
            else
            {
                vt = 0;
                wt = 0;
            }

            velMsg.type = PROD_MSG;
            velMsg.velocities[0] = vt;
            velMsg.velocities[1] = wt;

            if(msgsnd(msgqid_vel, &velMsg, sizeof(float) * 2, 0) != 0)
            {
                std::cout << "msgq_vel send failed" << std::endl;
            };
        }

        // std::cout << "cpx: " << currentPose.x << std::endl;
        // std::cout << "cpy: " << currentPose.y << std::endl;
        // std::cout << "cptheta: " << currentPose.theta << std::endl;

        // std::cout << "gpx: " << currentGoal.x << std::endl;
        // std::cout << "gpy: " << currentGoal.y << std::endl;
        // std::cout << "gptheta: " << currentGoal.theta << std::endl;

        // std::cout << "current goal num: " << goalCount << std::endl;

        // // std::cout << "dist to middle: " << distanceToPoleMiddle << std::endl;
        // // std::cout << "laserdistancemiddle: " << laserDistanceMiddle << std::endl;

        // std::cout << "vt: " << velMsg.velocities[0] << std::endl;
        // std::cout << "wt: " << velMsg.velocities[1] << std::endl;

        // std::cout << "fox: " << firstodomtest.x << std::endl;
        // std::cout << "foy: " << firstodomtest.y << std::endl;
        // std::cout << "fotheta: " << firstodomtest.theta << std::endl;

        // std::cout << "fgx: " << firstgoaltest.x << std::endl;
        // std::cout << "fgy: " << firstgoaltest.y << std::endl;
        // std::cout << "fgtheta: " << firstgoaltest.theta << std::endl;

        // std::cout << "wallfollowing: " << wallFollowing << std::endl;

        usleep(10); // 1ms
        // usleep(2000000);

    }


    return 0;
}
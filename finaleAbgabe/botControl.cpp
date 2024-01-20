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

// keys for msgqs
key_t KEY_ODOM          = 810;
key_t KEY_LASER         = 820;
key_t KEY_VEL           = 830;
key_t KEY_VEL_ACT        = 730;

// msgq IDs
int msgqid_odom, msgqid_laser, msgqid_vel, msgqid_vel_act;

// message type
enum MessageType {PROD_MSG=1, CONS_MSG};

// structs for specific messages
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

// program handler to exit succesfully
void msgqHandler(int sig)
{
    exit(EXIT_SUCCESS);
}

// struct for 2D Pose
struct Pose2d
{
    double x;
    double y;
    double theta;
};

double PI = 3.141592653589793238463;

// declaration of variables for goal definition and calculations
const unsigned int numGoPoses = 7;
Pose2d goPoses[numGoPoses];
Pose2d posePole;
Pose2d currentPose;
Pose2d currentGoal;
Pose2d currentDeltaPose;
bool goPosesDefined = false;

// proportional factor for linear control
double kp = 0.5;
double k_alpha = 3.0;
double k_beta = 1.0;

double maxRangeforPole = 2.0;

// declaration of linear control variables
double rho = 0;
double alpha = 0;
double beta = 0;
double vt = 0;
double wt = 0;

// maximum values for velocities
const double VT_MAX = 0.07;
const double WT_MAX = 1.0;

// bools to control driving
bool drive = true;
bool driveBool = false;

// declaration of messages
Message_Odom odomMsg;
Message_Laser laserMsg;
Message_Vel velMsg;

// counters
int goalCount = 0;
int poleCounter = 0;

// variables for calculating poses depending on pole distance
const int numOfScansForDistanceToPole = 50;
double distanceToPoleMiddle = 0;
double poleDiameter = 0.085;
double distanceFromPole = poleDiameter + 0.30;   // normal distance to pole

// variables for wallfollowing --> NOT IMPLEMENTED
double distanceLeft = 0, distanceRight = 0, laserDistanceMiddle = 0, deltaDistanceWall = 0;
double k_wall = 1.33;
bool wallFollowing = false;

// function to limit angles from -PI to PI radiant
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
    // attach/create msgqs
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

    // receiving odom and laser data
    msgrcv(msgqid_odom, &odomMsg, sizeof(double) * 6, PROD_MSG, 0);
    msgrcv(msgqid_laser, &laserMsg, sizeof(double) * 360, PROD_MSG, 0);

    // sending cmd_vel data
    Message_Vel actVelMsg;

    actVelMsg.type = PROD_MSG;
    actVelMsg.velocities[0] = 0.0;
    actVelMsg.velocities[1] = 0.0;


    if(msgsnd(msgqid_vel, &actVelMsg, sizeof(float) * 2, 0) != 0)
    {
        std::cout << "msgq_odom send failed" << std::endl;
    };



    while(true)
    {
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

        // calculation basis for converting quaternions to euler angles
        // double yaw_z = atan2( 2.0 * (w*z + x*y), 1.0 - 2.0 *(y*y + z*z));

        currentPose.x = odomMsg.pose[0];
        currentPose.y = odomMsg.pose[1];
        currentPose.theta = atan2(  (2.0 * (odomMsg.pose[5]*odomMsg.pose[4] + odomMsg.pose[2]*odomMsg.pose[3])),
                                    (1.0 - 2.0*(pow(odomMsg.pose[3], 2) + pow(odomMsg.pose[4], 2)))
                                    );


        // goPoses definition
        if(goPosesDefined == false)
        {
            // laserscan evaluation for pole distance
            for(int i = 0; i < numOfScansForDistanceToPole; i++)
            {
                actVelMsg.type = PROD_MSG;
                actVelMsg.velocities[0] = 0.0;
                actVelMsg.velocities[1] = 0.0;

                // sending cmd_vel to activate all programs
                if(msgsnd(msgqid_vel, &actVelMsg, sizeof(float) * 2, 0) != 0)
                {
                    std::cout << "msgq_odom send failed" << std::endl;
                };

                // receiving odom and laser data
                msgrcv(msgqid_odom, &odomMsg, sizeof(double) * 6, PROD_MSG, 0);
                int tmp = msgrcv(msgqid_laser, &laserMsg, sizeof(double) * 360, PROD_MSG, 0);

                if(tmp > 0)
                {
                    double laserToPole = 0;

                    // std::cout << "laserdata: " << laserMsg.scan[354] << std::endl;
                    // std::cout << "laserdata: " << laserMsg.scan[355] << std::endl;
                    // std::cout << "laserdata: " << laserMsg.scan[356] << std::endl;
                    // std::cout << "laserdata: " << laserMsg.scan[357] << std::endl;
                    // std::cout << "laserdata: " << laserMsg.scan[358] << std::endl;
                    // std::cout << "laserdata: " << laserMsg.scan[359] << std::endl;
                    // std::cout << "laserdata: " << laserMsg.scan[0] << std::endl;
                    // std::cout << "laserdata: " << laserMsg.scan[1] << std::endl;
                    // std::cout << "laserdata: " << laserMsg.scan[2] << std::endl;
                    // std::cout << "laserdata: " << laserMsg.scan[3] << std::endl;
                    // std::cout << "laserdata: " << laserMsg.scan[4] << std::endl;
                    // std::cout << "laserdata: " << laserMsg.scan[5] << std::endl;
                    // std::cout << "laserdata: " << laserMsg.scan[6] << std::endl;
                    // std::cout << "____________"<< std::endl;

                    int counterForDistanceMeasure = 356;

                    // searching pole in small field of view and middle the distances
                    for(int i = 0; i < 9; i++)
                    {
                        if( (laserMsg.scan[counterForDistanceMeasure] > 0.0) && (laserMsg.scan[counterForDistanceMeasure] < maxRangeforPole))
                        {
                            laserToPole = laserMsg.scan[counterForDistanceMeasure];
                            distanceToPoleMiddle = distanceToPoleMiddle + laserToPole + poleDiameter/2;
                            std::cout << "laserdata: " << laserMsg.scan[counterForDistanceMeasure] << std::endl;

                            poleCounter++;

                        }

                        counterForDistanceMeasure ++;
                        if (counterForDistanceMeasure == 360)
                        {
                            counterForDistanceMeasure = 0;
                        }
                    }
                    
                    std::cout << "----------" << std::endl;



                    tmp = 0;
                }
            }

            distanceToPoleMiddle = distanceToPoleMiddle / poleCounter;
            std::cout << "Distance to middle of pole: " << distanceToPoleMiddle << std::endl;

            // // getting wall distance
            // distanceLeft = 0;
            // distanceRight = 0;

            // for(int i = 268; i < 268 + 5; i++)
            // {
            //     distanceLeft = distanceLeft + laserMsg.scan[i];
            // }
            // distanceLeft = distanceLeft / 5;

            // for(int i = 88; i < 88 + 5; i++)
            // {
            //     distanceRight = distanceRight + laserMsg.scan[i];
            // }
            // distanceRight = distanceRight / 5;

            // // anfangsposition von y
            // laserDistanceMiddle = (distanceLeft + distanceRight) / 2;


            // pose calculation in dependency of first pose
            goPoses[0].x = currentPose.x;
            goPoses[0].y = currentPose.y;
            goPoses[0].theta = currentPose.theta;

            goPoses[6].x = goPoses[0].x;
            goPoses[6].y = goPoses[0].y;
            goPoses[6].theta = limitAng( goPoses[0].theta + (180.0 * PI / 180) ) ;

            posePole.x = goPoses[0].x + cos(goPoses[0].theta) * (distanceToPoleMiddle);
            posePole.y = goPoses[0].y + sin(goPoses[0].theta) * (distanceToPoleMiddle);
            posePole.theta = limitAng( goPoses[0].theta );

            goPoses[1].x = posePole.x + cos(posePole.theta + PI) * (distanceFromPole);
            goPoses[1].y = posePole.y + sin(posePole.theta + PI) * (distanceFromPole);
            goPoses[1].theta = limitAng( posePole.theta + (-0.0 * PI / 180) );

            goPoses[2].x = posePole.x + cos(posePole.theta - PI/2) * (distanceFromPole);
            goPoses[2].y = posePole.y + sin(posePole.theta - PI/2) * (distanceFromPole);
            goPoses[2].theta = limitAng( posePole.theta + 0.0 );

            goPoses[3].x = posePole.x + cos(posePole.theta) * (distanceFromPole);
            goPoses[3].y = posePole.y + sin(posePole.theta) * (distanceFromPole);
            goPoses[3].theta = limitAng( posePole.theta + (90.0 * PI / 180) );

            goPoses[4].x = posePole.x + cos(posePole.theta + PI/2) * (distanceFromPole);
            goPoses[4].y = posePole.y + sin(posePole.theta + PI/2) * (distanceFromPole);
            goPoses[4].theta = limitAng( posePole.theta + (180.0 * PI / 180) );

            goPoses[5].x = goPoses[1].x;
            goPoses[5].y = goPoses[1].y;
            goPoses[5].theta = limitAng( goPoses[1].theta + (-90.0 * PI / 180) );

            goalCount = 1;
            currentGoal = goPoses[goalCount];

            goPosesDefined = true;

            std::cout << "GoalPose[0].x: "     << goPoses[0].x << std::endl;
            std::cout << "GoalPose[0].y: "     << goPoses[0].y << std::endl;
            std::cout << "GoalPose[0].theta: " << goPoses[0].theta << std::endl;

            std::cout << "GoalPose[1].x: "     << goPoses[1].x << std::endl;
            std::cout << "GoalPose[1].y: "     << goPoses[1].y << std::endl;
            std::cout << "GoalPose[1].theta: " << goPoses[1].theta << std::endl;

            std::cout << "GoalPose[2].x: "     << goPoses[2].x << std::endl;
            std::cout << "GoalPose[2].y: "     << goPoses[2].y << std::endl;
            std::cout << "GoalPose[2].theta: " << goPoses[2].theta << std::endl;

            std::cout << "GoalPose[3].x: "     << goPoses[3].x << std::endl;
            std::cout << "GoalPose[3].y: "     << goPoses[3].y << std::endl;
            std::cout << "GoalPose[3].theta: " << goPoses[3].theta << std::endl;

            std::cout << "GoalPose[4].x: "     << goPoses[4].x << std::endl;
            std::cout << "GoalPose[4].y: "     << goPoses[4].y << std::endl;
            std::cout << "GoalPose[4].theta: " << goPoses[4].theta << std::endl;

            std::cout << "GoalPose[5].x: "     << goPoses[5].x << std::endl;
            std::cout << "GoalPose[5].y: "     << goPoses[5].y << std::endl;
            std::cout << "GoalPose[5].theta: " << goPoses[5].theta << std::endl;

            std::cout << "GoalPose[6].x: "     << goPoses[6].x << std::endl;
            std::cout << "GoalPose[6].y: "     << goPoses[6].y << std::endl;
            std::cout << "GoalPose[6].theta: " << goPoses[6].theta << std::endl;

            std::cout << "--------------------" << std::endl;

            // waiting for user input to start movement
            std::string strTmp;
            std::cin >> strTmp;

            std::cout << "Driving to goal " << goalCount << "!" << std::endl;
        }


        // laserscan wall following
        // distanceLeft = 0;
        // distanceRight = 0;

        // for(int i = 268; i < 268 + 5; i++)
        // {
        //     distanceLeft = distanceLeft + laserMsg.scan[i];
        // }
        // distanceLeft = distanceLeft / 5;

        // for(int i = 88; i < 88 + 5; i++)
        // {
        //     distanceRight = distanceRight + laserMsg.scan[i];
        // }
        // distanceRight = distanceRight / 5;

        // deltaDistanceWall = distanceLeft - distanceRight;

        // if((laserDistanceMiddle + 0.1) > ((distanceLeft + distanceRight) / 2))
        // {
        //     wallFollowing = true;
        // }
        // else
        // {
        //     wallFollowing = false;
        // }

        driveBool = true;

        if(driveBool == true)
        {
            // calculation of linear control
            currentDeltaPose.x = currentGoal.x - currentPose.x;
            currentDeltaPose.y = currentGoal.y - currentPose.y;
            currentDeltaPose.theta = currentGoal.theta  - currentPose.theta;

            currentDeltaPose.theta = limitAng( currentDeltaPose.theta );

            rho = sqrt(pow(currentDeltaPose.x, 2) + pow(currentDeltaPose.y, 2));

            alpha = (-currentPose.theta + atan2(currentDeltaPose.y, currentDeltaPose.x));
            alpha = limitAng( alpha );

            beta = -currentDeltaPose.theta - alpha;
            beta = limitAng( beta );

            if(drive == true)
            {
                // calculation of velocities
                vt = kp * rho;
                wt = (k_alpha*alpha) + (k_beta*beta);

                //  if(wallFollowing)
                //  {
                //      wt = wt + k_wall*deltaDistanceWall;
                //  }

                // limitation of velocities
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

                // if goal is reached, new goal is selected
                // if last goal is reached, program will be stopped
                if((abs(currentDeltaPose.x) < 0.075) && (abs(currentDeltaPose.y) < 0.075) && (abs(currentDeltaPose.theta) < 0.1)  && goalCount < numGoPoses)
                {
                    std::cout << "Goal " << goalCount << " reached! " << std::endl;
                    goalCount++;
                    if(goalCount == numGoPoses)
                    {
                        // stop while and end program
                        drive = false;
                    }
                    currentGoal = goPoses[goalCount];
                    std::cout << "Driving to goal " << goalCount << "!" << std::endl;
                }


            }
            else
            {
                // sending cmd_vel to stop robot and exiting program
                vt = 0;
                wt = 0;

                velMsg.type = PROD_MSG;
                velMsg.velocities[0] = vt;
                velMsg.velocities[1] = wt;

                if(msgsnd(msgqid_vel, &velMsg, sizeof(float) * 2, 0) != 0)
                {
                    std::cout << "msgq_vel send failed" << std::endl;
                };

                std::cout << "Final destination reached --> ending program" << std::endl;

                return 0;
            }

            // sending calculated and limited velocities
            velMsg.type = PROD_MSG;
            velMsg.velocities[0] = vt;
            velMsg.velocities[1] = wt;

            if(msgsnd(msgqid_vel, &velMsg, sizeof(float) * 2, 0) != 0)
            {
                std::cout << "msgq_vel send failed" << std::endl;
            };
        }

        usleep(10); // 10us

    }


    return 0;
}
#include "cmath"
#include <iostream>
#include <string>
#include "vector"
#include <sstream>

using namespace std;


string dataString="---START---{\"header\": {\"seq\": 67769, \"stamp\": {\"secs\": 1677511096, \"nsecs\": 329690933}, \"frame_id\": \"odom\"}, \"child_frame_id\": \"base_footprint\", \"pose\": {\"pose\": {\"position\": {\"x\": -8.901372348191217e-05, \"y\": 6.059087172616273e-05, \"z\": 0.0}, \"orientation\": {\"x\": 0.0, \"y\": 0.0, \"z\": -0.5472193956375122, \"w\": 0.8369892239570618}}, \"covariance\": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]}, \"twist\": {\"twist\": {\"linear\": {\"x\": 0.0003163835790473968, \"y\": 0.0, \"z\": 0.0}, \"angular\": {\"x\": 0.0, \"y\": 0.0, \"z\": 0.0009506940841674805}}, \"covariance\": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]}}___END___";               /* String to send to echo server */


std::vector<long double> ranges;
std::string rang="ranges";
std::string delimiter = ",";
std::string tmp;
int counter;
int pos_x_min,pos_x_max, pos_y_min, pos_y_max, pos_range_begin, pos_range_end;


int poses()
{
     pos_range_begin = dataString.find_first_of("[");
    pos_range_end = dataString.find_first_of("]");
    std::string newData = dataString.substr(pos_range_begin,pos_range_end);

    pos_x_min = dataString.find_first_of("x")+2;
    pos_x_max = dataString.find_first_of("y");
    std::string pose_x = dataString.substr(pos_x_min,pos_x_max);
    cout << pose_x<< endl;
    string pose=pose_x;
    tmp = pose_x.substr(1, pose_x.find(delimiter) - 1);
    cout<< tmp<< endl;
    pose_x=tmp;
    cout <<pose_x<<endl;
   

    pos_y_min = dataString.find_first_of("y")+2;
    pos_y_max = dataString.find_first_of("z");
    std::string pose_y = dataString.substr(pos_y_min,pos_y_max);
    tmp = pose_y.substr(1, pose_y.find(delimiter) - 1);
    cout<< tmp<< endl;
    pose_y=tmp;
    cout <<pose_y << endl;

    string orient=dataString.substr(dataString.find("orientation")+2, dataString.find("covariance"));
    cout << dataString.find("orientation")+2<< endl;
    cout<<orient<<endl;
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



    for( ; ; )
    {
        tmp = newData.substr(1, newData.find(delimiter) - 1);
        ranges.push_back(std::stold(tmp));
        counter++;
        if(tmp.find("]") < 20)
        {
            tmp = tmp.substr(0, tmp.find("]"));
            ranges.push_back(std::stold(tmp));
            break;
        }
        newData.erase(0, newData.find(delimiter) + 1);
    }

    for(float i : ranges)
    {
        std::cout << i <<  " " ;
    }

    std::cout << std::endl;
    std::cout << counter << std::endl;

    return 0;
}

double PI = 3.141592653589793238463;

double start_x = 0;
double start_y = 0;
double start_theta = 0;

double goal_x= 100;
double goal_y = 100;
double goal_theta = 45;

double k_alpha = 1;

double kp = 0.2;
const double k_b=0.2;
double k_betha = k_b;

double difference_x = 0.0;
double difference_y = 0.0;
double difference_theta = 0.0;

double alpha = 0; 
double betha = 0;


double pose_x=4;
double pose_y=-2;


double r=0.16; //difference between the wheels
double p_w=r*10;

//4 positions in one array, chosen by the symmetrie of a circle, so it have always the same way from his goal to the pose
double positions[4][3]={{pose_x+p_w,pose_y,-90},{pose_x,pose_y-p_w,-180}, {pose_x-p_w,pose_y,90}, {pose_x,pose_y+p_w,0} };

double p=0;

double vt = 0;
double wt = 0;

bool drive=true;

double position_x=0; //to define a delta variable
double position_y=0;
double position_theta=0;

int count=0;

vector<long double>pose2d;

/*void odometryCallback_(const nav_msgs::Odometry::ConstPtr msg) { //https://gist.github.com/marcoarruda/f931232fe3490b7fa20dbb38da1195ac
    //msg auf eusgelesenen Daten ändern
    pose2d[0] = ranges[0];
    pose2d[1] = ranges[1];
    
    /*tf::Quaternion q(
        msg->pose.pose.orientation.x,
        msg->pose.pose.orientation.y,
        msg->pose.pose.orientation.z,
        msg->pose.pose.orientation.w);
    tf::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);
    
    pose2d[2] = yaw;
}*/

int main()
{

    poses();
    //cout << ranges[0] << endl;

    pose2d=ranges;



    goal_x=positions[count][0];
    goal_y=positions[count][1];
    goal_theta=positions[count][2]*PI/180;

/*
   while (true)
    {


      difference_x = goal_x-pose2d[0];
      difference_y = goal_y-pose2d[1];
      difference_theta = (goal_theta-pose2d[2]);

       p = sqrt((difference_x*difference_x)+(difference_y*difference_y));
      alpha = (-pose2d[2] + atan2(difference_y, difference_x));
      if(alpha >= PI){
        alpha = PI;
      }
      if(alpha < (-PI)){
        alpha = (-PI);
      }

      betha = -difference_theta-alpha;

      if(betha < (-PI)){
        betha = (-PI);
      }
      if(betha > PI){
        betha = PI;
      }

      if (drive==true ){
            vt=kp*p;
            wt=(k_alpha*alpha)+(k_betha*betha);

            if (vt>0.22){
                vt=0.22;
            }
            if (wt>2.84){
                wt=2.84;
            }
            if (wt<-2.84){
                wt=-2.84;
            }
            if((difference_x<0.2) and (difference_y<0.2)){
              pose2d[0]=goal_x;
              pose2d[1]=goal_y;
            }

            if(abs(difference_x)<0.2 and abs(difference_y)<0.2 and abs(difference_theta)<0.2 and count<3){
              count++;
                goal_x=positions[count][0];
                goal_y=positions[count][1];
                goal_theta=positions[count][2]*PI/180;
            }
            else if(abs(difference_x)<0.2 and abs(difference_y)<0.2 and abs(difference_theta)<0.2 and count==3){
              
              break;
            }
      }
      else{
        vt = 0;
        wt = 0;

      }

/*
       cout << " wt = " << wt << endl;
       cout << "vt= " <<vt << endl;
       cout << " goal_x = " << goal_x<< endl;
       cout << " goal_y = " << goal_y << endl;
       cout << " goal_theta = " << goal_theta<< endl;
       cout << "pos X = " << pose2d[0] << endl;
       cout << "pos Y = " << pose2d[1] << endl;
       cout << "pos theta = " << pose2d[2] << endl;

   /* pose.pose.position.x=position_x;
    pose.pose.position.y=position_y;

    pose.pose.orientation.x=position_x;
    pose.pose.orientation.y=position_y;
    pose.pose.orientation.z=position_theta;
    pose.pose.orientation.w=wt;

    msg2.startx = difference_x;
     msg2.starty = difference_y;
     msg2.vt = vt;

     msg2.goalx = position_x;
     msg2.goaly = position_y;
     msg2.wt = wt;

     cmd_msg.linear.x = vt;
     cmd_msg.linear.y = 0;
     cmd_msg.linear.z = 0;
     cmd_msg.angular.x = 0;
     cmd_msg.angular.y = 0;
     cmd_msg.angular.z = wt;


    }*/
    
    return 0;
}
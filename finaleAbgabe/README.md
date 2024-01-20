This is the documentation of the submition of Fuchs-Robetin and Zeinler.

The control of the robot splits in 5 seperate programms:<br>
    1. msgq_creater.cpp<br>
        This program handles the message queues for the shared memory between the TCP clients and the main program (botControl.cpp).
        The programm creates 6 message queues. After cancelling the programm these message queues will be deleted.<br>
    2. TCP_Commander_cmd_vel_msgq.cpp<br>
        This program receives the calculated linear and angular velocities for the turtlebot from the main program and inserts them to the cmd_vel message and further sends the message to the robot. <br>
    3. TCP_Listener_laser_msgq.cpp<br>
        This program receives the laser scan data from the robot over TCP connection and extracts the data from the received message and further sends this data to the main program over a message queue.<br>
    4. TCP_Listener_odom_msgq.cpp<br>
        This program receives the odometry data from the robot over TCP connection and extracts the data from the received message and further sends this data to the main program over a message queue.<br>
    5. botControl.cpp<br>
        This program receives the data from the message queues and and sends linear and angular velocity to the TCP_Commander_cmd_vel_msgq.cpp.<br>
        Further is this the main calculation program for the path planning of the robot.<br>
        Program process:<br>
            1. the odometry and laserscan data is received<br>
            2. the distance to the pole is extracted and calculated with the laserscan data<br>
            3. the goal poses are calculated with the information of step 2 and the odometry data<br>
            4. the robot calculates the linear and angular velocities with linear control and if a goal pose is reached, the next goal is selected<br>
            5. the velocities are sended to the TCP_Commander_cmd_vel_msgq.cpp

The ip address of robot needs to be adjusted in the TCP client files!
        
How to start the control:<br>
Place the turtlebot in the startpositions --> the pole must be placed straight linear forward to the turtlebot<br>
5 terminals are required<br>
In the directory:<br>
    - compile the programms with "make":<br>
        msgq_creater.cpp                --> compiles to -->     msgq_creater<br>
        TCP_Commander_cmd_vel_msgq.cpp  --> compiles to -->     tcp_vel<br>
        TCP_Listener_laser_msgq.cpp     --> compiles to -->     tcp_laser<br>
        TCP_Listener_odom_msgq.cpp      --> compiles to -->     tcp_odom<br>
        botControl.cpp                  --> compiles to -->     bot_control<br>
    - start in each terminal the needed programs<br>
        1.  ./msgq_creater<br>
        2.  ./bot_control<br>
        3.  ./tcp_laser<br>
        4.  ./tcp_odom<br>
        5.  ./tcp_vel<br>
            if tcp_odom has a segmentation error, cancel tcp_vel and start tcp_odom and tcp_vel again, repeat until tcp_odom prints the odometry data<br>
    - after starting programs: wait until bot_control prints the values of the calculated goals --> user input is needed to start the motion. It is irrelevant, what the input is. Recommendation: "go" and enter.<br>
    - watch and enjoy the movement of the turtlebot<br>
    - when the last goal is reached, bot_control stops and the process is completed<br>
    - bot_control can be started again and the process starts from the beginning, the robot must be placed in the start position again<br>

If an unexpected movement of the robot occures and the wheels still move, stop all programs and execute tcp_vel_stop for stopping the wheels.

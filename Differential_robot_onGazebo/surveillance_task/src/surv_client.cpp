#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "nav_msgs/Odometry.h"
#include "boost/thread.hpp"
#include <actionlib/client/simple_action_client.h>
#include "surveillance_task/navAction.h"
#include "surveillance.h"


#define LIN_VEL 0.2
#define ANG_VEL 0.8

using namespace std;
class SURV_CLIENT {
    public:
        SURV_CLIENT();
        void laser_cb( sensor_msgs::LaserScanConstPtr );
        void odom_cb( nav_msgs::OdometryConstPtr );
        void run();
				void main_loop();
        int teaching(int);
				
    private:
        bool _tooClose;
        ros::NodeHandle _nh;
        ros::Subscriber _odom_sub;
        ros::Subscriber _laser_sub;
        ros::Publisher _vel_pub; 
        bool _obstacles_on_tha_path;
        geometry_msgs::Point odomPos;
        std::vector< geometry_msgs::Point > point_list;
        
};

int SURV_CLIENT::teaching(int index){

    string input;
    bool finished=false;
    float _fv; //Forward velocity	
		float _rv; //Rotational velocity
    geometry_msgs::Twist cmd_vel;
    std::vector<geometry_msgs::Point >::iterator it;
    
    cout << "Keyboard Input: " << endl;
    cout << "[w]: Forward direction velocity" << endl;
    cout << "[x]: Backward direction velocity" << endl;
    cout << "[a]: Left angular velocity" << endl;
    cout << "[d]: Right angular velocity" << endl;
    cout << "[k]: stop the robot!" << endl;
    cout << "[f]: stop the robot!" << endl;
    ros::Rate r(10);
    while(!finished){
      getline( cin, input);
      if( input == "w" ) 
        _fv = (_fv < 0.0 ) ? 0.0 : LIN_VEL;
      else if( input == "x" ) 
        _fv = (_fv > 0.0 ) ? 0.0 : -LIN_VEL;
      else if( input == "a" ) 
        _rv = (_rv > 0.0 ) ? 0.0 : -ANG_VEL;
      else if( input == "d" )
        _rv = (_rv < 0.0 ) ? 0.0 : ANG_VEL;
      else if( input == "k" ){
      it=point_list.begin()+index;

      point_list.insert(it,1,odomPos);
      cout<<"aggiunto nuovo punto"<<endl;
      index++;
        _fv = _rv = 0.0;}
      else if( input == "f" ) {
        //finished
        finished=true;
      }

      cmd_vel.linear.x = _fv;
      cmd_vel.angular.z = _rv;
      _vel_pub.publish( cmd_vel );
      r.sleep();
      }

index=index-1;
return index;
}

SURV_CLIENT::SURV_CLIENT() {
    _laser_sub = _nh.subscribe("/laser/scan",0,&SURV_CLIENT::laser_cb,this);
    _odom_sub = _nh.subscribe("/odom", 0, &SURV_CLIENT::odom_cb, this);
     _obstacles_on_tha_path = false;
      _vel_pub = _nh.advertise< geometry_msgs::Twist >("/cmd_vel", 0);
     
}


void SURV_CLIENT::laser_cb( sensor_msgs::LaserScanConstPtr  laser_data) {
 float laser_threshold=0.6;
 int first_index = int( (( 90-20 )/180.0*M_PI) / laser_data->angle_increment   );
    int last_index  = int( (( 90+20 )/180.0*M_PI) / laser_data->angle_increment   );
    int i=first_index;
   while(i<=last_index && !_obstacles_on_tha_path){
     _obstacles_on_tha_path=(laser_data->ranges[i]<=laser_threshold);
     i++;
   }
  //if too close preemption _obstacles_on_tha_path=true;
//cout<<laser_data->ranges[first_index]<<"...."<<laser_data->ranges[last_index]<<endl;
     //cout << "Laser!" << endl;   
}
 
void SURV_CLIENT::odom_cb( nav_msgs::OdometryConstPtr odom) {
    //cout << "Odom!" << endl;
    odomPos.x = odom->pose.pose.position.x;
    odomPos.y = odom->pose.pose.position.y;
      
}
 
//main loop! 
void SURV_CLIENT::main_loop() {



	//while(1) { ros::spinOnce(); sleep(1); }

  actionlib::SimpleActionClient<surveillance_task::navAction> ac("auto_nav_server", true);
  ac.waitForServer(); //will wait for infinite time

  surveillance_task::navGoal g;
  
  point_list.resize(4);
  
  point_list[0].x = 6.0;
  point_list[0].y = 0.0;
  
  point_list[1].x = 3.0;
  point_list[1].y = 5.0;
  
  
  point_list[2].x = 0.0;
  point_list[2].y = 1.0;
  
  point_list[3].x = 0.0;
  point_list[3].y = 0.0;

    
      while(ros::ok()) {
        
          ros::Rate r(10);
          bool done = false;
          
          
          //motion logic
          for(int i=0;i<point_list.size();i++)
          {   
              bool done = false;
              g.x_dest = point_list[i].x;
              g.y_dest = point_list[i].y;
          
          
              ac.sendGoal(g);

              while ( !done && !_obstacles_on_tha_path ) {
                  if ( ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED) {
                      done = true;
                      r.sleep();
                    } 
              r.sleep();
              }
              if( _obstacles_on_tha_path ) {
                
								ac.cancelGoal();		
								cout<<"sent to the robot the human generated velocity"<<endl;
								
                i=teaching(i);//learning new point
                //until human operator terminates with an additional input
                _obstacles_on_tha_path=false;            	
							}
      }

      if( ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED ) cout << "Target position reached!" << endl;
      else {
      cout<<"failAction"<<endl;
      }   
    }

}
 
 
void SURV_CLIENT::run() {
   	boost::thread ctrl_loop_t( &SURV_CLIENT::main_loop, this );
    ros::spin();
}



int main( int argc, char** argv) {

    ros::init(argc, argv, "surv_task_client" );
    SURV_CLIENT s;
    s.run();
    
    return 0;

}	

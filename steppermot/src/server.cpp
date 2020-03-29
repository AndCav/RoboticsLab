#include "ros/ros.h"
#include "std_msgs/Int32.h"
#include <actionlib/server/simple_action_server.h>
#include "steppermot/stepAction.h"  //if i call the message "section" i'll get to include packageName/sectionAction.h"
#include <iostream>
#include <sstream>


class action_class{
	private:
		ros::NodeHandle nh_;
		actionlib::SimpleActionServer<steppermot::stepAction> acser;
		steppermot::stepFeedback feedback;
		steppermot::stepResult   result;
		
		std::string action_name;
		float goal_pos;
		float cpos;
		int maxvel;
	public:
		action_class(std::string name) : acser(nh_,name ,boost::bind( &action_class::executeCB , this , _1), false),action_name(name),goal_pos(0.0),cpos(0.0),maxvel(0){
		acser.registerPreemptCallback(boost::bind(&action_class::preemptCB,this));
		acser.start();
		}
		
		void preemptCB();
		void executeCB(const steppermot::stepGoalConstPtr &goal);
};

void action_class::preemptCB(){
	ROS_WARN("%s got preempted!", action_name.c_str());
	result.final_pos = cpos;
	acser.setPreempted(result,"I got Preempted"); 
}

void action_class::executeCB(const steppermot::stepGoalConstPtr &goal){
	if(!acser.isActive() || acser.isPreemptRequested()) return ;
	ros::Rate rate(10);
	ROS_INFO("%s is processing the goal %.2f from %.2f in %d second",action_name.c_str(),goal->init_pos,goal->des_pos,goal->time);
	float ipos=goal->init_pos;
	float fpos=goal->des_pos;
	float necvel=(fpos-ipos)/(float)goal->time;
	ROS_INFO("VELOCITA' NECESSARIA: %.2f",necvel);
	float vel=0;
	if(necvel>(float)goal->vel_max) { vel=goal->vel_max; //ROS_INFO("too fast : %.2f",necvel);
					}
	else { vel=necvel; //ROS_INFO("i can do it : %.2f",necvel);
		}
	cpos=ipos;
	float passo=vel*36; //rps*360/10
	//ROS_INFO("PASSO: %.2f",passo);
	int numpassi=goal->time*10;
	int i=0;
	ROS_INFO("starting");
	while(cpos<fpos){
		cpos=cpos+passo;
		//ROS_INFO("%.2f",cpos);
		if(acser.isPreemptRequested()){return;}
		feedback.current_pos = cpos;
		acser.publishFeedback(feedback);
		rate.sleep();
		}
	ROS_INFO("Reached goal");
	result.final_pos = fpos;
	acser.setSucceeded(result);
}
int main(int argc, char** argv)
{
  ros::init(argc, argv, "step_action");
  ROS_INFO("Starting Demo Action Server");
  action_class demo_action_obj(ros::this_node::getName());
  ros::spin();
  return 0;
}





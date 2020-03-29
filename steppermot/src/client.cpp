#include "ros/ros.h"
#include <iostream>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>
#include "steppermot/stepAction.h"

/*
#goal definition
float64 init_pos
float64 des_pos
int32 time
int32 vel_max
---
#result definition
float64 final_pos
---
#feedback
float64 current_pos*/
void doneCb(const actionlib::SimpleClientGoalState &state,const steppermot::stepResultConstPtr &result){
ROS_INFO("Finished in state [%s]", state.toString().c_str());
  ROS_INFO("Answer: %f", result->final_pos);
}

// Called once when the goal becomes active
void activeCb()
{
  ROS_INFO("activation");
}

// Called every time feedback is received for the goal
void feedbackCb(const steppermot::stepFeedbackConstPtr &feedback)
{
  ROS_INFO("Got current position: %f", feedback->current_pos);
}
int main (int argc, char **argv) {

  ros::init(argc, argv, "step_action_client");
  if(argc != 5){
		ROS_INFO("%d",argc);
		ROS_WARN("Usage: steppermot_action_client <i_pos> <f_pos> <maxvel> <time in sec>");
		return 1;
	}

  actionlib::SimpleActionClient<steppermot::stepAction> ac("step_action", true);

  ROS_INFO("Waiting for action server to start.");

  // wait for the action server to start
  ac.waitForServer(); //will wait for infinite time

  ROS_INFO("Action server started, sending goal.");

  // send a goal to the action
  steppermot::stepGoal goal;
  goal.init_pos = atof(argv[1]);
  goal.des_pos = atof(argv[2]);
  goal.time = atoi(argv[4]);
  goal.vel_max = atoi(argv[3]);
  
//sending  
ROS_INFO("Sending Goal [%.2f to %.2f max: %d  ] and Preempt time of [%d]",goal.init_pos,goal.des_pos,goal.vel_max,goal.time);
ac.sendGoal(goal, &doneCb, &activeCb, &feedbackCb);

bool finished_before_timeout = ac.waitForResult(ros::Duration(goal.time+0.008));
ac.cancelGoal();
if (!finished_before_timeout) {
    	  
  sleep(1);
	
  }
  else  ROS_INFO("you did it");

  return 0;
}

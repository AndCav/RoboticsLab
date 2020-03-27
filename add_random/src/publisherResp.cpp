/*subscribe to topic of Node1 and republishes the result on another topic using response.msg
*/

#include "ros/ros.h"
#include "add_random/custom.h"
#include "add_random/response.h"
#include <iostream>
#include <stdlib.h>

using namespace std;
class calculator{
	public:
	calculator();
	void callback(add_random::custom req);

	private:
	ros::NodeHandle _nh;
	ros::Subscriber _topic_sub;
	ros::Publisher _topic_pub;

};

calculator::calculator(){
_topic_sub = _nh.subscribe("/question", 3, &calculator::callback,this);
_topic_pub = _nh.advertise<add_random::response>("/total",3);
}

void calculator::callback(add_random::custom req){	
ROS_INFO("GOT A QUESTION");
ROS_INFO_STREAM("::"<<req.first<<"+"<<req.second<<"=?");
add_random::response res;
res.first=req.first;
res.second=req.second;
res.result=res.first+res.second;
ROS_INFO_STREAM(res.result);
_topic_pub.publish(res);

}


int main (int argc,char** argv){
int i=0;
ros::init(argc,argv,"ros_calculator" ) ;
calculator cl;
ros::spin();
return 0;
}

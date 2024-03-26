#include <ros/ros.h>
#include"stdio.h"
#include"wiringPi.h" 
int main(int argc, char **argv)
{       
        setlocale(LC_ALL,"");
//执行节点初始化
        ros::init(argc,argv,"HelloVSCode");
//输出日志
        ROS_INFO("Hello VSCode!!!");
        wiringPiSetup();  
        pinMode(26,OUTPUT);  
        int angle=180;  
        int i=0;  
        float x=0;  
        int k=180;//180次循环的时间够了  
        while(k--)  
        {  
                x=11.11*i;  
                digitalWrite(26,HIGH);  
                delayMicroseconds(500+x);  
                digitalWrite(26,LOW);  
                delayMicroseconds(19500-x);  
                if(i==angle)  
                    break;  
                i++;  
         }  
        return 0;  
}  
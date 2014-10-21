#include "communicationmanager.h"
#include "rosThread.h"

#include <QThread>
#include <ros/ros.h>
#include <QApplication>

int main(int argc,char** argv)
{

    ros::init(argc,argv,"communicationISLH");

    QApplication app(argc,argv);

    CommunicationManager* manager = new CommunicationManager();

    RosThread* rosthread = new RosThread(manager);

    manager->rosthread = rosthread;

    QThread thr;
    QThread thr2;

    rosthread->moveToThread(&thr);

    manager->moveToThread(&thr2);

    QObject::connect(rosthread,SIGNAL(rosFinished()),&thr,SLOT(quit()));

    QObject::connect(&thr,SIGNAL(finished()),&app,SLOT(quit()));

    QObject::connect(&thr,SIGNAL(finished()),rosthread,SLOT(deleteLater()));

    QObject::connect(&thr,SIGNAL(started()),rosthread,SLOT(work()));

    QObject::connect(&thr2,SIGNAL(started()),manager,SLOT(start()));

    thr.start();

    thr2.start();

    return app.exec();

  //  ros::NodeHandle n;

//    ros::Subscriber amclSub = n.subscribe("amcl_pose",2,amclPoseCallback);

 //   ros::spin();

   // tcpComm comm;

   // comm.myServer->setupServer();

}



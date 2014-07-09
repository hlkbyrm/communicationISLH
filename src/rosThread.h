//#include "navigationController.h"
#include <QThread>
#include <QObject>
#include <ros/ros.h>
#include "communicationISLH/robotInfo.h"
#include <communicationISLH/networkInfo.h>
#include <communicationISLH/helpMessage.h>
#include "communicationmanager.h"
#include <QTimer>

#define numOfRobots 5

class RosThread:public QObject
{
    Q_OBJECT

public:

    RosThread();

    RosThread(CommunicationManager* currentmanager);

   // RosThread(int argc, char **argv, std::string nodeName);

public:

     void shutdownROS();

     friend class CommunicationManager;

     friend class Robot;

private:

     CommunicationManager* manager;

     bool shutdown;

     ros::NodeHandle n;

     ros::Subscriber robotInfoSubscriber;

     ros::Publisher neighborInfoPublisher;

     // Publish the received coordinator Update to the coordinator
     ros::Publisher coordinatorUpdatePublisher;

     ros::Publisher hotspotHandlerMessageInPublisher;

     // Subscribe for coordinatorUpdate from Navigation
     ros::Subscriber coordinatorUpdateSubscriber;

     // Subscribe for a network update from coordinator
     ros::Subscriber networkUpdateSubscriber;

     ros::Subscriber hotspotHandlerMessageOutSubscriber;




public slots:
     void work();

signals:
   void rosFinished();
   void  rosStarted();
   void  rosStartFailed();

};

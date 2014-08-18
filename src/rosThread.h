#include <QThread>
#include <QObject>
#include <ros/ros.h>
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

     ros::Publisher neighborInfoPublisher;

     /*
     ros::Subscriber robotInfoSubscriber;

     // Publish the received coordinator Update to the coordinator
     ros::Publisher coordinatorUpdatePublisher;

     ros::Publisher hotspotHandlerMessageInPublisher;

     // Subscribe for coordinatorUpdate from Navigation
     ros::Subscriber coordinatorUpdateSubscriber;

     // Subscribe for a network update from coordinator
     ros::Subscriber networkUpdateSubscriber;

     ros::Subscriber hotspotHandlerMessageOutSubscriber;
*/

   ros::Publisher messageInPublisher;

   ros::Subscriber messageOutSubscriber;


public slots:
     void work();

signals:
   void rosFinished();
   void  rosStarted();
   void  rosStartFailed();

};

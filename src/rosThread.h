#include <QThread>
#include <QObject>
#include <ros/ros.h>
#include "communicationmanager.h"
#include <QTimer>
#include <qjson/parser.h>

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

    int queueSize;

    bool shutdown;

    ros::NodeHandle n;

    ros::Publisher neighborInfoPublisher;
    ros::Publisher robotConnectionInfoPub;

    ros::Publisher messageInPublisher;

    ros::Subscriber messageOutSubscriber;

    bool readConfigFile(QString filename);

public slots:
    void work();

signals:
    void  rosFinished();
    void  rosStarted();
    void  rosStartFailed();

};

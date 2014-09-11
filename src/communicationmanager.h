#ifndef COMMUNICATIONMANAGER_H
#define COMMUNICATIONMANAGER_H


#include "network/tcpComm.h"
#include "robot.h"
#include <QObject>
#include <ISLH_msgs/inMessage.h>
#include <ISLH_msgs/outMessage.h>
#include <QXmlStreamReader>

class RosThread;

class CommunicationManager : public QObject
{
    Q_OBJECT
public:
    explicit CommunicationManager(QObject *parent = 0);

    // Connection request to the given address and port
    void connectToHost(QString hostAddress, quint16 port);

    // Connection request to the given address and port - wait until connected
    void connectToHostWithWait(QString hostAddress, quint16 port);

    void delay(int duration);

    /*
    // Handle messages from Navigation ISL
    void handleNavigationISLInfo(ISLH_msgs::robotInfo msg);

    // Handle coordinator updates from Navigation ISL
    void handleCoordinatorUpdate(ISLH_msgs::robotInfo info);

    // Handle the coordinator update
    void handleNetworkUpdateFromCoordinator(ISLH_msgs::networkInfo info);

    void handleHotspotHandlerMessageOut(ISLH_msgs::helpMessage msg);
*/

    void handleMessageOut(ISLH_msgs::outMessage msg);

    bool readConfigFile(QString filename);

    bool initializeNetwork();

    RosThread* rosthread;

private:

    tcpComm* TcpComm;

    QTcpSocket tempSocket;

    Client* tempClient;

    QVector<Robot*> robots;

    QStringList neighbors;

    Robot* myrobot;

    bool firstNetworkReceived;

   // RosThread* rosthread;

signals:
    
public slots:

private slots:

    void connecttt();

    void handleSocketConnect();

    void handleSocketError(QAbstractSocket::SocketError error);
    
    void handleNewCommRequest(QTcpSocket* socket);

    void getClientDisconnected(int type);

    void connectToRobots();

   // void handleNetworkInfo(QStringList list);


};

#endif // COMMUNICATIONMANAGER_H

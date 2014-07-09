#ifndef COMMUNICATIONMANAGER_H
#define COMMUNICATIONMANAGER_H


#include "network/tcpComm.h"
#include "robot.h"
#include <QObject>
#include <communicationISLH/robotInfo.h>
#include <communicationISLH/networkInfo.h>
#include <communicationISLH/helpMessage.h>
#include <QXmlStreamReader>

class RosThread;

class CommunicationManager : public QObject
{
    Q_OBJECT
public:
    explicit CommunicationManager(QObject *parent = 0);

    // Connection request to the given address and port
    void connectToHost(QString hostAddress, quint16 port);

    // Handle messages from Navigation ISL
    void handleNavigationISLInfo(communicationISLH::robotInfo msg);

    // Handle coordinator updates from Navigation ISL
    void handleCoordinatorUpdate(communicationISLH::robotInfo info);

    // Handle the coordinator update
    void handleNetworkUpdateFromCoordinator(communicationISLH::networkInfo info);

    void handleHotspotHandlerMessageOut(communicationISLH::helpMessage msg);

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

    void handleNetworkInfo(QStringList list);


};

#endif // COMMUNICATIONMANAGER_H

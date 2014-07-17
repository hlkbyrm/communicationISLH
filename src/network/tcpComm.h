#pragma once
#include "tcpServer.h"
#include "Client.h"
#include <QtCore/QString>
#include <QList>
#define MAX_INCOMING_CLIENT_NUM 2
#define MAX_OUTGOING_CLIENT_NUM 2

class robot;
class tcpComm : public QObject
{
	Q_OBJECT
public:
	
	tcpComm(QObject* parent = 0);
    //tcpComm(robot* rbt);
	~tcpComm(void);

public:
	// tcp server object
	tcpServer* myServer;

    //robot* itsRobot;
	
    /// Clients that are connected to me
    Client* incomingClients[MAX_INCOMING_CLIENT_NUM];

    QList<Client*>incomingClientss;

    QVector<Client*>outgoingClients;

    /// Clients that I am connected to
   // Client* outgoingClients[MAX_OUTGOING_CLIENT_NUM];

	// Array that holds the empty slots for my clients
    bool incomingClientSlots[MAX_INCOMING_CLIENT_NUM];

	// Array that holds the empty slots for remote clients
    bool outgoingClientSlots[MAX_OUTGOING_CLIENT_NUM];

    int findEmptyIncomingClientSlot();

    int findEmptyOutgoingClientSlot();

	bool readHostName();

	QString getHostName();

	void setHostName(QString Name);

	bool checkIPorHostName(QString text); // checks whether the given string is an ip address or hostname

private:
	
	QString hostName;

public slots:
		
		
		void getClientInfo(const QList<QString>& clientInfo, int clientType);
		void getClientDisconnected(int id, int clientType);
        //void getClientImageReceived(const QImage& image);

signals:
		
		void sendclientInfo(const QList<QString>& clientInfo, int clientType);
        //void sendClientImage(const QImage& image);
	
};

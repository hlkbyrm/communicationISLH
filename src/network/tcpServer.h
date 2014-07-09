#pragma once
#include <QtNetwork/QtNetwork>
#include "Client.h"
class tcpComm;
// tcpServer class is derived from QTcpServer to handle incoming tcp connections

class tcpServer : public QTcpServer
{
	Q_OBJECT

public:
//	tcpServer(QObject* parent = 0);
	
	tcpServer(QObject* parent = 0);	// default constructor
	 
	QList<QTcpSocket*> connectedList; // keeps track of connected machines

	bool setupServer(); // set server for listening state

	void shutDown(); // shut-down server 

	tcpComm* TcpComm;

	//QTcpSocket* trial;

	//Client clients; // client objects for 

	//Client* clients;

public:
	~tcpServer(void);

signals:
    void newCommRequest(QTcpSocket* socket);

	public slots:
		void acceptConnection();

		
};

#include "tcpServer.h"
#include <QtGui/QMessageBox>
#include <QDebug>
#include "tcpComm.h"
tcpServer::tcpServer(QObject* parent):QTcpServer(parent), TcpComm((tcpComm*)parent)
{
	connect(this,SIGNAL(newConnection()),this,SLOT(acceptConnection()));

}

tcpServer::~tcpServer(void)
{
    this->shutDown();
}
bool tcpServer::setupServer(){

	if(!listen(QHostAddress::Any,1200)){
		
		return false;
		
	}
	 
	return true; 

}
void tcpServer::acceptConnection(){

    QTcpSocket* newConnection = nextPendingConnection();


    emit newCommRequest(newConnection);

}
void tcpServer::shutDown(){

	close();

}

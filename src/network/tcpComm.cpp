#include "tcpComm.h"
#include "Client.h"
//#include "globals.h"
#include <QtGui/QMessageBox>
#include <QtGui/QValidator>

tcpComm::tcpComm(QObject* parent): QObject(parent) ,outgoingClients(MAX_OUTGOING_CLIENT_NUM)
{
	myServer = new tcpServer(this);

    for(int i = 0; i < MAX_INCOMING_CLIENT_NUM; i++) incomingClientSlots[i] = false;

    for(int i = 0; i < MAX_OUTGOING_CLIENT_NUM; i++){

        outgoingClientSlots[i] = false;
     }


		
    if(!readHostName())
    {

        this->setHostName("dummy");


    }
}


tcpComm::~tcpComm(void)
{

}

void tcpComm::getClientInfo(const QList<QString>& clientInfo, int clientType){

	
	emit (sendclientInfo(clientInfo, clientType));

}
void tcpComm::getClientDisconnected(int id, int clientType){

	bool isLocalHost = false;

    if(clientType == INCOMING_CLIENT){
		
        incomingClientSlots[id] = false;

		QList<QString> clientInfo;

		QString str;
	
		str.setNum(id);

		QString ip = "";

		clientInfo<<str<<ip<<ip;

        getClientInfo(clientInfo , INCOMING_CLIENT);

	//	if(myClients[id]->getClientHostName() == "localhost") isLocalHost = true;
	//	else if(myClients[id]->getClientIP() == "127.0.0.1")isLocalHost = true;

        incomingClients[id]->deleteLater();
	}
    else if(clientType == OUTGOING_CLIENT  || isLocalHost){
	
        outgoingClientSlots[id] = false;

		QList<QString> clientInfo;

		QString str;
	
		str.setNum(id);

		QString ip = "";

		clientInfo<<str<<ip<<ip;

        getClientInfo(clientInfo , OUTGOING_CLIENT);

        if(id >=0)
        //this->outgoingClients.remove(id);

        outgoingClients[id]->deleteLater();

		
	
	}

}
/// Find the first empty server slot
int tcpComm::findEmptyIncomingClientSlot(){

	int min = 1000;

    for(int i = 0; i < MAX_INCOMING_CLIENT_NUM; i++){
		
        if(!incomingClientSlots[i]){
			
			if(i < min) min = i;

		}
	}
	
	if(min != 1000) return min;

	return -1;
}
int tcpComm::findEmptyOutgoingClientSlot(){

	int min = 1000;

    for(int i = 0; i < MAX_OUTGOING_CLIENT_NUM; i++){
		
        if(!outgoingClientSlots[i]){
			
			if(i < min) min = i;

		}
	}
	
	if(min != 1000) return min;

	return -1;


}
QString tcpComm::getHostName(){

	return hostName;

}
void tcpComm::setHostName(QString Name){


	hostName = Name;

}
bool tcpComm::readHostName(){


	QFile file("hostname.txt");

	if(!file.exists()){
	
    //    QMessageBox::critical((QWidget*)this->parent(),tr("QTServer"),tr("Error!! 'hostname.txt' does not exist"));
		return false;
	
	}
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
	
    //	QMessageBox::critical(NULL,tr("QTServer"),tr("Error!! Cannot open 'hostname.txt'"));
		return false;
	}

	QTextStream in(&file);
    
	while (!in.atEnd()) {
         
		hostName = in.readLine();
         
     }

	file.close();

	return true;
	

}
// returns true if the given text is a hostname otherwise returns false
bool tcpComm::checkIPorHostName(QString text){

	QRegExp rx("[A-Z]|[a-z]{1,}"); // check if there is any letter in the given text
	
	QRegExpValidator v( rx, 0 );
	
	int pos = 0;
	
	QValidator::State result;

	result = v.validate(text,pos);

	if(result == QValidator::Acceptable) return true; // if the text is host name return true

	return false; //else return false
	
	




}


/* 
 * File:   client.h
 * Author: Vladislav
 *
 * Created on September 28, 2012, 11:11 PM
 * 
 * Special Thanks for Dragonway
 */

#ifndef CLIENT_H
#define	CLIENT_H

#include <QWidget>

class QTcpSocket;
class QTextEdit;
class QLineEdit;

class Gui;

#include <QAbstractSocket>

class Client : public QWidget
{
	Q_OBJECT
public:
	Client(const QString &host, int port, Gui *gui, QWidget *parent = 0);
	QStringList getUserList();
private:
	/*Variables*/
	
	static const quint8 _action_MessageToAll		= 1;
	static const quint8 _action_MessageToUser		= 2;
	static const quint8 _action_Authentification	= 3;
	
	static const quint8 _action_YourMessage			= 4;
	static const quint8 _action_PublicMessage		= 5;
	static const quint8 _action_PrivateMessage		= 6;
	static const quint8 _action_UserLeft			= 7;
	static const quint8 _action_UserJoin			= 8;
	static const quint8 _action_UserList			= 9;
	
	static const quint8 _error_NameIsNotValid		= 101;
	static const quint8 _error_NameIsUsed			= 102;
	static const quint8 _error_FileIsNotOpened		= 103;
	
	static const quint8 _action_StartVideoStream	= 200;
	
	Gui				*_gui;
	QTcpSocket		*_socket;
	quint16			_nextBlockSize;
	QString			_nameUser;
	QStringList		_userList;
	bool			_isAuthorized;
	
	/*Functions*/
	void sendName();
	void sendToUser();
		
private slots:
	void slotReadyRead();
	void slotError(QAbstractSocket::SocketError error);
	
	void slotConnected();	
public slots:
	void slotSendToServer();
	void slotStartVideoStream();
};

#endif	/* CLIENT_H */


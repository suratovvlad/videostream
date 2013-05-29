/* 
 * File:   user.h
 * Author: Vladislav
 *
 * Created on October 2, 2012, 8:43 PM
 * 
 * Special Thanks for Dragonway
 */

#ifndef USER_H
#define	USER_H

#include <QObject>

class Server;

class QTcpSocket;
class QString;

class User : public QObject
{
	Q_OBJECT
public:
	User(QTcpSocket *socket = NULL, Server *server = NULL);
	
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
	
	
	/*Functions*/
	bool isAuthorized();
	void sendBlock(const QByteArray &outBlock);
	void sendUserList();
	void sendError(const quint8 errorCode);
	void sendSender(const QString &message, const QString &recipient);
	void addUser(const QString &name);
	QString getNameUser() const;
	
	
	void StartVideoStream(QString fileName);
private:
	QTcpSocket	*_socketUser;
	Server		*_serverUser;
	QString		_nameUser;
	bool		_isAuthorized;
	quint16		_nextBlockSize;
private slots:
	void slotDisconnectUser();
	void slotReadyClient();
	
signals:
	void removeUser(User *removal);
	
};

#endif	/* USER_H */
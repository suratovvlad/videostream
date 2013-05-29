/* 
 * File:   server.h
 * Author: Vladislav
 *
 * Created on September 28, 2012, 8:39 PM
 * 
 * Special Thanks for Dragonway
 */

#ifndef SERVER_H
#define	SERVER_H

#include <QObject>
#include <QList>

class User;

class QTcpServer;
class QTextEdit;

class Server : public QObject
{
	Q_OBJECT
public:
	Server(int port, QObject *parent = 0);
private:
	/*Variables*/
	QTcpServer	*_server;
	quint16		_nextBlockSize;
public:	
	/*Functions*/
	
	
	void sendToUser(const QString &message, const QString &senderName, const QString &recipientName);
	void sendToAll(const QString &message, const QString &senderName);
	void sendUserLeft(const QString &nameUser);
	void sendUserJoin(const QString &nameUser);
	QStringList getUserList();
	
	bool isNameValid(const QString &nameUser);
	bool isNameUsed	(const QString &nameUser);
private:
	/*Containers*/
	QList<User*> _clients;
	
public slots:
	void slotNewConnection();
	void slotRemoveUser(User *removal);
};

#endif	/* SERVER_H */


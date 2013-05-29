/* 
 * File:   server.cpp
 * Author: Vladislav
 * 
 * Created on September 28, 2012, 8:39 PM
 * 
 * Special Thanks for Dragonway
 */

#include "server.h"
#include "user.h"

#include <QTcpServer>

#include <QString>
#include <QDataStream>
#include <QByteArray>
#include <QStringList>
#include <QRegExp>

#define DEBUG
#ifdef DEBUG
	#include <QDebug>
#endif

Server::Server(int port, QObject *parent) : QObject(parent), _nextBlockSize(0)
{
	_server = new QTcpServer(this);
	if (!_server->listen(QHostAddress::Any, port))
	{
		//_server->close();
#ifdef DEBUG
		qDebug() << "Unable to start the server: " + _server->errorString();
#endif
		return;
	}
#ifdef DEBUG
	qDebug() << "Server has started";
#endif
	connect(_server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

void Server::slotNewConnection()
{
	User *new_client = new User(_server->nextPendingConnection(), this);
	connect(new_client, SIGNAL(removeUser(User*)), this, SLOT(slotRemoveUser(User*)));
	_clients.append(new_client);
}

void Server::sendToUser(const QString &message, const QString &senderName, const QString &recipientName)
{
#ifdef DEBUG
	qDebug() << "New message to all" << " " << senderName << " : " << message;
#endif
	QByteArray outBlock;
	QDataStream out(&outBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_8);
	out << quint16(0) << User::_action_PrivateMessage << senderName << message;
	
	//out.device()->seek(0);
	//out << quint16(outBlock.size() - sizeof(quint16));
	
	for (int i = 0; i < _clients.length(); ++i)
		if (_clients.at(i)->getNameUser() == recipientName && _clients.at(i)->isAuthorized())
		{
			_clients.at(i)->sendBlock(outBlock);
			break;
		}
}

void Server::sendToAll(const QString &message, const QString &senderName)
{
#ifdef DEBUG
	qDebug() << "New message to all" << " " << senderName << " : " << message;
#endif
	QByteArray outBlock;
	QDataStream out(&outBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_8);
	out << quint16(0) << User::_action_PublicMessage << senderName << message;
	
	out.device()->seek(0);
	out << quint16(outBlock.size() - sizeof(quint16));
	for (int i = 0; i < _clients.length(); ++i)
		if (_clients.at(i)->isAuthorized())
			_clients.at(i)->sendBlock(outBlock);
}

void Server::sendUserLeft(const QString &nameUser)
{
	QByteArray outBlock;
	QDataStream out(&outBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_8);
	out << quint16(0) << User::_action_UserLeft << nameUser;
	
	out.device()->seek(0);
	out << quint16(outBlock.size() - sizeof(quint16));
	for (int i = 0; i < _clients.length(); ++i)
		if (_clients.at(i)->isAuthorized() && _clients.at(i)->getNameUser() != nameUser)
			_clients.at(i)->sendBlock(outBlock);
}

void Server::sendUserJoin(const QString &nameUser)
{
	QByteArray outBlock;
	QDataStream out(&outBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_8);
	out << quint16(0) << User::_action_UserJoin << nameUser;
	
	out.device()->seek(0);
	out << quint16(outBlock.size() - sizeof(quint16));
	for (int i = 0; i < _clients.length(); ++i)
		if (_clients.at(i)->isAuthorized() && _clients.at(i)->getNameUser() != nameUser)
			_clients.at(i)->sendBlock(outBlock);
}

QStringList Server::getUserList()
{
	QStringList userList;
	foreach(User *cl, _clients)
		if (cl->isAuthorized())
			userList << cl->getNameUser();
	return userList;
}


void Server::slotRemoveUser(User *removal)
{
	_clients.removeAt(_clients.indexOf(removal));
}

bool Server::isNameValid(const QString &nameUser)
{
	if (nameUser.length() < 2 || nameUser.length() > 15)
		return false;
	QRegExp rx("[A-Za-z0-9_]+");
	return rx.exactMatch(nameUser);
}

bool Server::isNameUsed(const QString &nameUser)
{
	for (int i = 0; i < _clients.length(); ++i)
		if (_clients.at(i)->getNameUser() == nameUser)
			return true;
	return false;
}



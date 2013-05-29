/* 
 * File:   user.cpp
 * Author: Vladislav
 * 
 * Created on October 2, 2012, 8:43 PM
 * 
 * Special Thanks for Dragonway
 */
#include "server.h"
#include "user.h"

#include <QTcpSocket>
#include <QStringList>
#include <QFile>
#include <QByteArray>

#define DEBUG

User::User(QTcpSocket *socket, Server *server) : _nextBlockSize((quint16)0) 
{
	_socketUser = socket;
	_serverUser = server;
	_isAuthorized = false;
	_nameUser = "";
	
	connect(_socketUser,SIGNAL(readyRead()),this,SLOT(slotReadyClient()));	
	connect(_socketUser, SIGNAL(disconnected()), this, SLOT(slotDisconnectUser()));
}

bool User::isAuthorized()
{
	return _isAuthorized;
}

QString User::getNameUser() const
{
	return _nameUser;
}

void User::slotDisconnectUser()
{
	if (_isAuthorized)
		_serverUser->sendUserLeft(_nameUser);
	emit removeUser(this);
	this->deleteLater();
}

void User::sendBlock(const QByteArray &outBlock)
{
	_socketUser->write(outBlock);
}

void User::sendUserList()
{
	QByteArray outBlock;
	QDataStream out(&outBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_8);
	out << quint16(0) << User::_action_UserList;
	
	QStringList userList = _serverUser->getUserList();
	out << userList.length();
	for (int i = 0; i < userList.length(); ++i)
		out << userList.at(i);
	
	out.device()->seek(0);
	out << quint16(outBlock.size() - sizeof(quint16));
	_socketUser->write(outBlock);
}

void User::sendError(const quint8 errorCode)
{
	QByteArray outBlock;
	QDataStream out(&outBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_8);
	out << quint16(0) << errorCode;
	
	out.device()->seek(0);
	out << quint16(outBlock.size() - sizeof(quint16));
	_socketUser->write(outBlock);
}

void User::addUser(const QString &name)
{
	_nameUser = name;
	if (!_isAuthorized) _isAuthorized = true;
}

void User::slotReadyClient()
{
	QDataStream in(_socketUser);
	in.setVersion(QDataStream::Qt_4_8);
	for (;;)
	{
		if (!_nextBlockSize)
		{
			if (_socketUser->bytesAvailable() < sizeof(quint16))
				break;
			in >> _nextBlockSize;
		}
		
		if (_socketUser->bytesAvailable() < _nextBlockSize)
		{
			_nextBlockSize = 0;
			break;
		}
		
		quint8 recievedActionCode;
		in >> recievedActionCode;
		
		if(!_isAuthorized && recievedActionCode != _action_Authentification)
			return;
		
		QString recievedMessage;
		QString recipientName;
		
		switch (recievedActionCode)
		{
			case _action_MessageToAll:
				in >> recievedMessage;
				_serverUser->sendToAll(recievedMessage, _nameUser);
				break;
			case _action_MessageToUser:
				in >> recipientName >> recievedMessage;
				_serverUser->sendToUser(recievedMessage, _nameUser, recipientName);
				if (recipientName != _nameUser)
					sendSender(recievedMessage, recipientName);
				break;
			case _action_Authentification:
				in >> recievedMessage; //name
				if (!_serverUser->isNameValid(recievedMessage))
				{
					sendError(this->_error_NameIsNotValid);
					return;
				}
				if (_serverUser->isNameUsed(recievedMessage))
				{
					sendError(this->_error_NameIsUsed);
					return;
				}
				addUser(recievedMessage);
				sendUserList();
				_serverUser->sendUserJoin(_nameUser);
				break;
			case _action_StartVideoStream:
				in >> recievedMessage;
				StartVideoStream(recievedMessage);
				break;
		}		
		_nextBlockSize = 0;
	}
}

void User::sendSender(const QString &message, const QString &recipient)
{
	QByteArray outBlock;
	QDataStream out(&outBlock,QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_8);
	
	out << quint16(0) << _action_YourMessage << recipient << message;
	out.device()->seek(0);
	
	out << quint16(outBlock.size() - sizeof(quint16));
	_socketUser->write(outBlock);
}

void User::StartVideoStream(QString fileName)
{	
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))
	{
#ifdef DEBUG
		qDebug() << "Error: Cannot open file!";
#endif
		QByteArray outBlock;
		QDataStream out(&outBlock, QIODevice::WriteOnly);
		out.setVersion(QDataStream::Qt_4_8);
		out << quint16(0) << User::_error_FileIsNotOpened << "Error: Cannot open file!";
		out.device()->seek(0);
		out << quint16(outBlock.size() - sizeof(quint16));
		_socketUser->write(outBlock);
		return;
	}
	
	QByteArray byteArray = file.readAll();
	file.close();
	
	QByteArray outBlock;
	QDataStream out(&outBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_8);
	out << quint16(0) << User::_action_StartVideoStream << "Now, there is streaming of " + fileName << byteArray;
	out.device()->seek(0);
	out << quint16(outBlock.size() - sizeof(quint16));
	_socketUser->write(outBlock);
}
/* 
 * File:   client.cpp
 * Author: Vladislav
 * 
 * Created on September 28, 2012, 11:11 PM
 * 
 * Special Thanks for Dragonway
 */

#include "client.h"
#include "gui.h"

#include <QTcpSocket>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QDataStream>
#include <QTime>
#include <QString>
#include <QByteArray>

#define DEBUG
#ifdef DEBUG
	#include <QDebug>
#endif

Client::Client(const QString& host, int port, Gui *gui, QWidget* parent) : QWidget(parent), _nextBlockSize(0), _nameUser("")
{
	_isAuthorized = false;
	_nextBlockSize = 0;
	_socket = new QTcpSocket(this);
	_gui = gui;
	
	_socket->connectToHost(host, port);
	connect(_socket, SIGNAL(connected()), SLOT(slotConnected()));
	connect(_socket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
	connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
	
#ifdef DEBUG
	qDebug() << "Client has started";
#endif
}

QStringList Client::getUserList()
{
	return _userList;
}

void Client::slotReadyRead()
{
	QDataStream in(_socket);
	in.setVersion(QDataStream::Qt_4_8);	
	for (;;)
	{
		if (!_nextBlockSize)
		{
			if (_socket->bytesAvailable() < sizeof(quint16))
				break;
			in >> _nextBlockSize;
		}
		
		if (_socket->bytesAvailable() < _nextBlockSize)
		{
			_nextBlockSize = 0;
			break;
		}
		
		quint8 receivedActionCode;
		in >> receivedActionCode;
		
		if (!_isAuthorized
			&& receivedActionCode != _action_UserList
			&& receivedActionCode != _error_NameIsNotValid
			&& receivedActionCode != _error_NameIsUsed
			)
		{
            return;
		}
		
		QString receivedMessage;
		QString receiver;
		QByteArray byteArray;
		
		switch (receivedActionCode)
		{
			case _action_UserList:
				int numberOfUsers;
				in >> numberOfUsers;
				for (int i = 0; i < numberOfUsers; ++i)
				{
					in >> receivedMessage; //names of users
					_userList.append(receivedMessage);
				}
				if (!_isAuthorized)
				{
					_isAuthorized = true;
					_gui->authorizationPassed();
				}
				_gui->passListOfUsers(_userList);				
				break;
			case _action_PublicMessage:
				in >> receiver >> receivedMessage;
				_gui->publicMessage(receiver, receivedMessage);
				break;
			case _action_PrivateMessage:
				in >> receiver >> receivedMessage;
				_gui->privateMessage(receiver, receivedMessage);
				break;
			case _action_YourMessage:
				in >> receiver >> receivedMessage;
				_gui->yourMessage(receiver, receivedMessage, _nameUser);
				break;
			case _action_UserJoin:
				in >> receivedMessage; //name
				_userList.append(receivedMessage);
				_gui->userJoin(receivedMessage);
				break;
			case _action_UserLeft:
				in >> receivedMessage; //name
				_userList.removeAt(_userList.indexOf(receivedMessage));
				_gui->userLeft(receivedMessage);
				break;
			case _error_NameIsNotValid:
				_nameUser = "";
				_gui->nameIsNotValid();
				sendName();
				break;
			case _error_NameIsUsed:
				_nameUser = "";
				_gui->nameIsUsed();
				sendName();
				break;
			case _action_StartVideoStream:
				in >> receivedMessage >> byteArray;
				_gui->showMessage(receivedMessage);
				if (!_gui->StartVideoStream(receivedMessage, byteArray))
				{
					qDebug() << "Error : File is not saved";
					_gui->showError("File is not saved");
				}
				break;
			case _error_FileIsNotOpened:
				qDebug() << "Error : File is not opened";
				_gui->showError("File is not opened");
				break;
		}
		_nextBlockSize = 0;
	}
}

void Client::slotError(QAbstractSocket::SocketError error)
{
	QString strError = 
						tr("Error: ") +	
						(error == QAbstractSocket::HostNotFoundError ?
						tr("The host was not found.") :
						error == QAbstractSocket::RemoteHostClosedError ?
						tr("The remote host is closed.") :
						error == QAbstractSocket::ConnectionRefusedError ?
						tr("The connection was refused.") :
						QString(_socket->errorString()));
	_gui->showError(strError);
}

void Client::slotSendToServer()
{
	bool toUser = false;
	_gui->isSentToUser(toUser);
	if (!toUser)
	{
		sendToUser();
		return;
	}
	
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_8);	
	out << quint16(0) << _action_MessageToAll << _gui->getMessage();	
	out.device()->seek(0);
	out << quint16(arrBlock.size() - sizeof(quint16));	
	_socket->write(arrBlock);
	_gui->cleanInputArea();
}

void Client::slotConnected()
{
	_gui->connectEstablished();
	sendName();
}

void Client::sendName()
{
	_nameUser = _gui->getUserName();
	QByteArray outBlock;
	QDataStream out(&outBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_8);
	out << quint16(0) << _action_Authentification << _nameUser;
	out.device()->seek(0);
	out << quint16(outBlock.size() - sizeof(quint16));
	_socket->write(outBlock);
}

void Client::sendToUser()
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_8);	
	out << quint16(0) << _action_MessageToUser << _gui->getRecipient() << _gui->getMessage();	
	out.device()->seek(0);
	out << quint16(arrBlock.size() - sizeof(quint16));	
	_socket->write(arrBlock);
	_gui->cleanInputArea();
}

void Client::slotStartVideoStream()
{
	QString fileName = "./bmpt2.ogv";
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_8);
	out << quint16(0) << _action_StartVideoStream << fileName;
	out.device()->seek(0);
	out << quint16(arrBlock.size() - sizeof(quint16));	
	_socket->write(arrBlock);
}
/* 
 * File:   gui_qt.cpp
 * Author: Vladislav
 * 
 * Created on 20 Октябрь 2012 г., 17:35
 * 
 * Special Thanks for Dragonway
 */

#include "gui_qt.h"
#ifdef __LINUX__

#include "client.h"

//#include <QtGui>

#include <QString>
#include <QStringList>

#include <QTextEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTabWidget>
#include <QPushButton>
#include <QList>
#include <QColor>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QLineEdit>

#include <QFile>
#include <QByteArray>

#define DEBUG
#ifdef DEBUG
	#include <QDebug>
#endif

Gui::Gui(QWidget *parent) : QWidget(parent)
{
	_chatClient = new Client("localhost", 1991, this, this);
	
	_isPublicMessage = true;
	_inputArea = new QTextEdit;
	_outputArea = new QTextEdit;
	_outputArea->setReadOnly(true);	
	_logArea = new QTextEdit;
	_logArea->setReadOnly(true);
	
	_listOfUsers = new QListWidget;
	_sendButton = new QPushButton(tr("&Send"));
	connect(_sendButton, SIGNAL(clicked()), this, SLOT(slotSendToServer()));
	
	_startVideoStreamButton = new QPushButton(tr("&Video"));
	connect(_startVideoStreamButton, SIGNAL(clicked()), this, SLOT(slotStartVideoStream()));
	
	
	_outputAreas.append(_outputArea);
	_tabs = new QTabWidget;
	_tabs->addTab(_outputArea, "General");
	connect(_listOfUsers, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(newTab(QListWidgetItem *)));
	
	QHBoxLayout *topLayout = new QHBoxLayout;
	topLayout->addWidget(_tabs);
	topLayout->addWidget(_listOfUsers);
	
	QHBoxLayout *middleLayout = new QHBoxLayout;
	middleLayout->addWidget(_inputArea);
	middleLayout->addWidget(_logArea);
	
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(topLayout);
	mainLayout->addLayout(middleLayout);
	mainLayout->addWidget(_sendButton);
	mainLayout->addWidget(_startVideoStreamButton);
	
	this->setLayout(mainLayout);
}

Gui::~Gui()
{
}

void Gui::connectEstablished()
{
#ifdef DEBUG
	qDebug() << "Connection is Established";
#endif
	QColor _color(0,255,0);
    _logArea->setTextColor(_color);
    _logArea->append(tr("The server is connected"));
}

void Gui::nameIsNotValid()
{
#ifdef DEBUG
	qDebug() << "Name is not valid";
#endif
	QColor color(255,0,0);
	_logArea->setTextColor(color);
	_logArea->append(tr("Name is not valid"));
}

void Gui::nameIsUsed()
{
#ifdef DEBUG
	qDebug() << "Name is used";
#endif
	QColor color(255,0,0);
	_logArea->setTextColor(color);
	_logArea->append(tr("Name is used"));
}

void Gui::authorizationPassed()
{
#ifdef DEBUG
	qDebug() << "Authorization passed";
#endif
	QColor color(0,255,0);
	_logArea->setTextColor(color);
	_logArea->append(tr("Authorization passed"));
}

void Gui::passListOfUsers(const QStringList &userList)
{
	_listOfUsers->insertItems(0, userList);
}

void Gui::publicMessage(const QString &receiver, const QString &message)
{
#ifdef DEBUG
	qDebug() << "You got the public message";
#endif
	_outputArea->append(receiver + ": " + message);
}

void Gui::privateMessage(const QString &receiver, const QString &message)
{
#ifdef DEBUG
	qDebug() << "You got the private message";
#endif
	QTextEdit* outputArea = NULL;
	bool isNeededTab = false;
	for (int i = 0; i < _tabs->count(); ++i)
		if (_tabs->tabText(i) == receiver) 
		{
			_outputAreas.at(i)->append(receiver + ": " + message);
			isNeededTab = true;
			break;
		}
	if (isNeededTab) return;
	outputArea = new QTextEdit;
	outputArea->setReadOnly(true);
	_tabs->addTab(outputArea, receiver);
	_outputAreas.append(outputArea);
	outputArea->setGeometry(0,0,395,230);
	outputArea->append(receiver + ": " + message);
}

void Gui::yourMessage(const QString &receiver, const QString &message, const QString &nameUser)
{
#ifdef DEBUG
	qDebug() << "You got the your message";
#endif
	for (int i = 0; i < _tabs->count(); ++i)
	if (_tabs->tabText(i) == receiver) 
	{
		_outputAreas.at(i)->append(nameUser + ": " + message);
		break;
	}
}

void Gui::userJoin(const QString &userName)
{
#ifdef DEBUG
	qDebug() << "User " << userName << " Join";
#endif
	QColor color(0,255,0);
	_listOfUsers->addItem(userName);
	_logArea->setTextColor(color);
	_logArea->append(userName + tr(" Join"));
}

void Gui::userLeft(const QString &userName)
{
#ifdef DEBUG
	qDebug() << "User " << userName << " Left";
#endif
	QColor color(255,0,0);
	for (int i = 0; i < _listOfUsers->count(); ++i)
		if (_listOfUsers->item(i)->text() == userName)
		{
			_listOfUsers->takeItem(i);
			_logArea->setTextColor(color);
			_logArea->append(userName + tr(" Left"));
			break;
		}
	for (int i = 0; i < _tabs->count(); ++i)
		if (_tabs->tabText(i) == userName) 
		{
			_outputAreas.removeAt(i);
			_tabs->removeTab(i);
			break;
		}
}

void Gui::isSentToUser(bool &toUser)
{
	toUser = _isPublicMessage;
}

void Gui::showError(const QString &error)
{
	QColor _color(255,0,0);
    _logArea->setTextColor(_color);
	qDebug() << "Error : " << error;
	_logArea->append(error);
}

void Gui::showMessage(const QString &message)
{
	QColor _color(0,0,255);
    _logArea->setTextColor(_color);
	qDebug() << "Message : " << message;
	_outputArea->append(message);
	_logArea->append(message);
}

void Gui::cleanInputArea()
{
	_inputArea->setPlainText("");
}

QString Gui::getUserName()
{
	bool isEntered = false;
	QString buffer;
	while(!isEntered)
	{
		buffer = QInputDialog::getText(0,"Enter a nickname","My name is...",QLineEdit::Normal,"",&isEntered);
		if (buffer == "") isEntered = false;
	}
	return buffer;
}

QString Gui::getMessage()
{ 
	return _inputArea->toPlainText();
}

QString Gui::getRecipient()
{
	return _tabs->tabText(_tabs->currentIndex());
}

void Gui::slotSendToServer()
{
	if (_tabs->tabText(_tabs->currentIndex()) == "General")
		_isPublicMessage = true;
	else
		_isPublicMessage = false;
	_chatClient->slotSendToServer();
}

void Gui::newTab(QListWidgetItem *_item)
{
	for (int i = 0; i < _tabs->count(); ++i)
        if (_tabs->tabText(i) == _item->text()) return;
    QTextEdit *outputArea = new QTextEdit;
    outputArea->setReadOnly(true);
	_outputAreas.append(outputArea);
	outputArea->setGeometry(0,0,395,230);
    _tabs->addTab(outputArea,_item->text());
}


void Gui::slotStartVideoStream()
{
	_chatClient->slotStartVideoStream();
}

bool Gui::StartVideoStream(QString fileName, QByteArray byteArray)
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		qDebug() << "Error: Cannot open file!";
		this->showError("Cannot open file");
		return false;
	}
	
	file.write(byteArray);
	file.close();
	return true;
}
#endif /*__LINUX__*/

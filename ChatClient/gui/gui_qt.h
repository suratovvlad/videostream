/* 
 * File:   gui_qt.h
 * Author: Vladislav
 *
 * Created on November 2, 2012, 8:48 PM
 * 
 * Special Thanks for Dragonway
 */

#ifndef GUI_QT_H
#define	GUI_QT_H

#include "gui.h"
#ifdef __LINUX__

#include <QWidget>
#include <QList>

class Client;

class QString;
class QStringList;

class QTextEdit;
class QListWidget;
class QPushButton;
class QTabWidget;
//class QList;
class QListWidgetItem;

class Gui : public QWidget
{
	Q_OBJECT
public:
	Gui(QWidget *parent = 0);
	virtual ~Gui();
private:
	friend class Client;
	Client *_chatClient;
	
	void connectEstablished();
	void nameIsNotValid();
	void nameIsUsed();
	void authorizationPassed();
	void passListOfUsers(const QStringList &userList);
	void publicMessage(const QString &receiver, const QString &message);
	void privateMessage(const QString &receiver, const QString &message);
	void yourMessage(const QString &receiver, const QString &message, const QString &nameUser);
	void userJoin(const QString &nameUser);
	void userLeft(const QString &nameUser);
	void isSentToUser(bool &toUser);
	void showError(const QString &error);
	void showMessage(const QString &message);
	void cleanInputArea();
	
	QString getUserName();
	QString getMessage();
	QString getRecipient();
	
	bool _isPublicMessage;

	QTextEdit		*_inputArea;
	QTextEdit		*_outputArea;
	QTextEdit		*_logArea;
	QListWidget		*_listOfUsers;
	QTabWidget		*_tabs;
	QPushButton		*_sendButton;
	
	QPushButton		*_startVideoStreamButton;
	
	QList< QTextEdit* > _outputAreas;
	
	bool StartVideoStream(QString fileName, QByteArray byteArray);
	
private slots:
	void newTab(QListWidgetItem *_item);
	void slotSendToServer();
	
	void slotStartVideoStream();
};
#endif /*__LINUX__*/

#endif	/* GUI_QT_H */


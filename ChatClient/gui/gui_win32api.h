/* 
 * File:   gui_win32api.h
 * Author: Vladislav
 *
 * Created on November 2, 2012, 9:31 PM
 */

#ifndef GUI_WIN32API_H
#define	GUI_WIN32API_H

#include "gui.h"
#ifdef _WIN32

#include <windows.h>

class Client;
class QString;
class QStringList;
class MainWindow;

class Gui
{
public:
	Gui(HINSTANCE hInstance, int nCmdShow);
	void slotSendToServer();	
	virtual ~Gui();	
private:
	MainWindow *mainWnd;
	void AddTextToSomeArea(HWND hArea, char *text);
/*	
public:
	HWND GetHMainWnd() { return hMainWnd; }
	HWND GetHSendButton() { return hSendButton; }
	HWND GetHInputArea() { return hInputArea; }
	HWND GetHOutputArea() { return hOutputArea; }
	HWND GetHLogArea() { return hLogArea; }
	HWND GetHListOfUsers() { return hListOfUsers; }
	
protected:
	HWND hMainWnd;
	HWND hSendButton;
	HWND hInputArea;
	HWND hOutputArea;
	HWND hLogArea;
	HWND hListOfUsers;
	
	WNDCLASSEX wc;
	
private:
	HINSTANCE	_hInst;
	int			_cmdShow;
	LPCTSTR		_windowName;
	LPCTSTR		_menuName;
	int			_x, _y, _width, _height;
	UINT		_classStyle;
	DWORD		_windowStyle;
	HWND		_hParent;
	
	std::string	_szClassName;
	
	int Init(LRESULT (WINAPI *pWndProc)(HWND,UINT,WPARAM,LPARAM));
	void CreateWnd();
	void Initialization();
*/	
private:
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK AuthDlg(HWND, UINT, WPARAM, LPARAM);
	
	friend class Client;
	Client *_chatClient;
	bool _isPublicMessage;
	
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
	void cleanInputArea();
	
	QString getUserName();
	QString getMessage();
	QString getRecipient();
};

#endif /*_WIN32*/

#endif	/* GUI_WIN32API_H */
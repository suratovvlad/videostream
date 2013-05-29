/* 
 * File:   mainwindow_win32api.h
 * Author: Vladislav
 *
 * Created on December 8, 2012, 3:31 PM
 */

#ifndef KWND_H
#define	KWND_H

#include "gui.h"
#ifdef _WIN32

#include <windows.h>
#include <string>

#include "../client.h"

class MainWindow
{
public:	
	MainWindow(Client *chat, 
		LPCTSTR windowName, HINSTANCE hInst, int cmdShow,
		LRESULT (WINAPI *pWndProc)(HWND,UINT,WPARAM,LPARAM),
		LPCTSTR menuName = NULL,
		int x = CW_USEDEFAULT, int y = 0,
		int width = CW_USEDEFAULT, int height = 0,
		UINT classStyle =  CS_HREDRAW | CS_VREDRAW,
		DWORD windowStyle = WS_OVERLAPPEDWINDOW,
		HWND hParent = NULL);

	HWND GetHMainWnd() { return hMainWnd; }
	HWND GetHSendButton() { return hSendButton; }
	HWND GetHInputArea() { return hInputArea; }
	HWND GetHOutputArea() { return hOutputArea; }
	HWND GetHLogArea() { return hLogArea; }
	HWND GetHListOfUsers() { return hListOfUsers; }
	
	void OnButtonClicked() {_chat->slotSendToServer();}
	
protected:
	HWND hMainWnd;
	HWND hSendButton;
	HWND hInputArea;
	HWND hOutputArea;
	HWND hLogArea;
	HWND hListOfUsers;
	
	WNDCLASSEX wc;
	
private:
	Client *_chat;
	
	LPCTSTR		_windowName;
	HINSTANCE	_hInst;
	int			_cmdShow;
	LPCTSTR		_menuName;
	int			_x, _y, _width, _height;
	UINT		_classStyle;
	DWORD		_windowStyle;
	HWND		_hParent;
	
	std::string	_szClassName;
	int Init(LRESULT (WINAPI *pWndProc)(HWND,UINT,WPARAM,LPARAM));
	void CreateWnd();
};

#endif /*_WIN32*/

#endif	/* KWND_H */
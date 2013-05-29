/* 
 * File:   mainwindow_win32api.cpp
 * Author: Vladislav
 * 
 * Created on December 8, 2012, 3:31 PM
 */

#include "mainwindow_win32api.h"
#ifdef _WIN32

MainWindow::MainWindow(Client *chat,
			LPCTSTR windowName, HINSTANCE hInst, int cmdShow,
			LRESULT (WINAPI *pWndProc)(HWND,UINT,WPARAM,LPARAM),
			LPCTSTR menuName, int x, int y, int width, int height,
			UINT classStyle, DWORD windowStyle, HWND hParent)

			: _chat(chat),
			 _windowName(windowName), _hInst(hInst), _cmdShow(cmdShow), 
			 _menuName(menuName), _x(x), _y(y), _width(width), _height(height),
			 _classStyle(classStyle), _windowStyle(windowStyle), _hParent(hParent)
{
	_szClassName = "KWndClass";
	
	Init(pWndProc);
	CreateWnd();
	
	SendMessage(hMainWnd, WM_COMMAND, IDM_AUTHDLG, 0); //Why is it not running??
	
	if (!hMainWnd) {
		char text[100] = "Cannot create window: ";
		strcat(text, windowName);
		MessageBox(NULL, text, "Error", MB_OK);
		return;
	}

	ShowWindow(hMainWnd, cmdShow); 
}

int MainWindow::Init(LRESULT (WINAPI *pWndProc)(HWND,UINT,WPARAM,LPARAM))
{
	wc.cbSize        = sizeof(wc);
	wc.style         = _classStyle;
	wc.lpfnWndProc   = pWndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = _hInst;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = _menuName;
	wc.lpszClassName = _szClassName.c_str();
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
	
	if (!RegisterClassEx(&wc)) {
		char msg[100] = "Cannot register class: ";
		strcat(msg, _szClassName.c_str());
		MessageBox(NULL, msg, "Error", MB_OK);
		return 0;
	}
	return 1;
}

void MainWindow::CreateWnd()
{
	hMainWnd = CreateWindowA(
						_szClassName.c_str(),
						_windowName,
						/*WS_OVERLAPPEDWINDOW*/ WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
						_x,
						_y,
						_width,
						_height,
						HWND_DESKTOP,
						NULL,
						NULL/*_hInst*/,
						NULL
						);	
	SetWindowLongPtrA( hMainWnd, GWLP_USERDATA, (LONG_PTR)this);
	
	hSendButton = CreateWindowA(
									"BUTTON", // Window class
									"Send", // Title
									WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
									10,
									WindowHeight/2 + 220,
									WindowWidth + 60,
									30,
									hMainWnd,
									(HMENU)ID_SEND_BUTTON, // для WM_COMMAND
									NULL /*hThisInst*/,
									NULL
								);
	
	hInputArea = CreateWindowA(
									"EDIT",
									NULL,
									ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL |WS_BORDER | WS_VISIBLE | WS_CHILD | ES_LEFT,
									10,
									WindowHeight/2 + 15,
									WindowWidth/2 + WindowWidth/4 - 20,
									195,
									hMainWnd,
									(HMENU) ID_INPUT_AREA,
									NULL,
									NULL
								);
	
	hOutputArea = CreateWindowA(
									"EDIT",
									NULL,
									ES_READONLY /*WS_DISABLED*/ | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL |WS_BORDER | WS_VISIBLE | WS_CHILD | ES_LEFT,
									10,
									10,
									WindowWidth/2 + WindowWidth/4 - 20,
									195,
									hMainWnd,
									(HMENU) ID_OUTPUT_AREA,
									NULL,
									NULL
								);
	
	hListOfUsers = CreateWindowA(
									"LISTBOX",
									NULL,
									//ES_READONLY /*WS_DISABLED*/ | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL |WS_BORDER | WS_VISIBLE | WS_CHILD | ES_LEFT,
									WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL,
									WindowWidth/2 + WindowWidth/4,
									10,
									220,
									195,
									hMainWnd,
									(HMENU) ID_LIST_OF_USERS_AREA,
									NULL,
									NULL
								);
	
	hLogArea = CreateWindowA(
									"EDIT",
									NULL,
									ES_READONLY /*WS_DISABLED*/ | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL |WS_BORDER | WS_VISIBLE | WS_CHILD | ES_LEFT,
									WindowWidth/2 + WindowWidth/4,
									WindowHeight/2 + 15,
									220,
									195,
									hMainWnd,
									(HMENU) ID_LOG_AREA,
									NULL,
									NULL
								);
	SetFocus(hInputArea);
}

#endif /*_WIN32*/
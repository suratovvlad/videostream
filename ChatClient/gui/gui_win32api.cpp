/* 
 * File:   gui_win32api.cpp
 * Author: Vladislav
 * 
 * Created on November 2, 2012, 9:31 PM
 */
#include "gui_win32api.h"
#ifdef _WIN32

#include "client.h"

#include <QString>
#include <QStringList>

#define DEBUG
#ifdef DEBUG
	#include <QDebug>
#endif

char NickName[1024];

//INT_PTR CALLBACK AuthDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

Gui::Gui(HINSTANCE hInstance, int nCmdShow)/*:_hInst(hInstance), _cmdShow(nCmdShow)*/
{
	_chatClient = new Client("localhost", 1991, this);
	
	MSG msg;
	mainWnd = new MainWindow(_chatClient, "ChatClient", hInstance, nCmdShow, WndProc,
						NULL, WindowInitialWidth, WindowInitialHeight, 
						WindowInitialWidth + WindowWidth, WindowInitialHeight + WindowHeight
						);
	
	/*Initialization();*/
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

Gui::~Gui()
{
	delete _chatClient;
}

void Gui::AddTextToSomeArea(HWND hArea, char *text)
{
	SendMessage(hArea, EM_SETSEL, -1, -1); 
	SendMessage(hArea, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(text));
}

void Gui::connectEstablished()
{
#ifdef DEBUG
	qDebug() << "Connection is Established";
#endif
	AddTextToSomeArea(mainWnd->GetHLogArea(), ((char *)"The server is connected\r\n"));
}

void Gui::nameIsNotValid()
{
#ifdef DEBUG
	qDebug() << "Name is not valid";
#endif
	AddTextToSomeArea(mainWnd->GetHLogArea(), ((char *)"Name is not valid\r\n"));
}

void Gui::nameIsUsed()
{
#ifdef DEBUG
	qDebug() << "Name is used";
#endif
	AddTextToSomeArea(mainWnd->GetHLogArea(), ((char *)"Name is used\r\n"));	
}

void Gui::authorizationPassed()
{
#ifdef DEBUG
	qDebug() << "Authorization passed";
#endif
	AddTextToSomeArea(mainWnd->GetHLogArea(), ((char *)"Authorization passed\r\n"));
}

void Gui::passListOfUsers(const QStringList &userList)
{
	SendMessage(mainWnd->GetHListOfUsers(), LB_RESETCONTENT, 0, 0);
	//SetDlgItemTextA(hMainWnd, ID_LIST_OF_USERS_AREA, (userList.at(0) + "\r\n").toLocal8Bit().data());
	for (int i = 0; i < userList.length(); ++i)
	{
		//AddTextToSomeArea(hListOfUsers, (userList.at(i) + "\r\n").toLocal8Bit().data());
		SendMessage(mainWnd->GetHListOfUsers(), LB_ADDSTRING, 0, (LPARAM)userList.at(i).toLocal8Bit().data());
	}
}

void Gui::publicMessage(const QString &receiver, const QString &message)
{
#ifdef DEBUG
	qDebug() << "Public message:";
	qDebug() << receiver << ":" << message;
#endif
	//SetDlgItemTextA(hMainWnd, ID_OUTPUT_AREA, (receiver + (QString)": " + message).toLocal8Bit().data());
	AddTextToSomeArea(mainWnd->GetHOutputArea(), (receiver + (QString)": " + message + "\r\n").toLocal8Bit().data()); 
}

void Gui::privateMessage(const QString &receiver, const QString &message)
{
	#ifdef DEBUG
	qDebug() << "Private message:";
	qDebug() << receiver << ":" << message;
#endif
	//SetDlgItemTextA(hMainWnd, ID_OUTPUT_AREA, (receiver + (QString)": " + message).toLocal8Bit().data());
	AddTextToSomeArea(mainWnd->GetHOutputArea(), (receiver + (QString)": " + message + "\r\n").toLocal8Bit().data());
}

void Gui::yourMessage(const QString &receiver, const QString &message, const QString &nameUser)
{
#ifdef DEBUG
	qDebug() << "Your message:";
	qDebug() << receiver << ":" << nameUser << ":" << message;
#endif
	//SetDlgItemTextA(hMainWnd, ID_OUTPUT_AREA, (nameUser + (QString)": " + message).toLocal8Bit().data());
	AddTextToSomeArea(mainWnd->GetHOutputArea(), (nameUser + (QString)": " + message + "\r\n").toLocal8Bit().data());
}

void Gui::userJoin(const QString &nameUser)
{
#ifdef DEBUG
	qDebug() << "New user connected:";
	qDebug() << nameUser << "Join";
#endif
	//AddTextToSomeArea(hListOfUsers, (nameUser + "\r\n").toLocal8Bit().data());
	SendMessage(mainWnd->GetHListOfUsers(), LB_ADDSTRING, 0, (LPARAM)nameUser.toLocal8Bit().data());
	AddTextToSomeArea(mainWnd->GetHLogArea(), (nameUser + (QString)" - Join\r\n").toLocal8Bit().data());
}

void Gui::userLeft(const QString &nameUser)
{
#ifdef DEBUG
	qDebug() << "User disconnected:";
	qDebug() << nameUser << "Left";
#endif
	AddTextToSomeArea(mainWnd->GetHLogArea(), (nameUser + (QString)" - Left\r\n").toLocal8Bit().data());	
	passListOfUsers(_chatClient->getUserList());
}

void Gui::isSentToUser(bool &toUser)
{
	toUser = _isPublicMessage;
}

void Gui::showError(const QString &error)
{
	AddTextToSomeArea(mainWnd->GetHLogArea(), (error + "\r\n").toLocal8Bit().data());
}

void Gui::cleanInputArea()
{
	SetDlgItemTextA(mainWnd->GetHMainWnd(), ID_INPUT_AREA, "");
}

QString Gui::getUserName()
{
	/*extern std::string NickName;
	std::cout << "Enter your nickname:" << std::endl;
	std::cin >> NickName;
	std::cout << NickName << " - your name" << std::endl;
	return NickName.c_str();
	*/
	//AuthorizedDialog *authDialog;
	//authDialog->Run();
	//authDialog->Show(true);
	
	//qDebug() << Nick.c_str();
	//SendMessage(mainWnd->GetHMainWnd(), WM_COMMAND, IDM_AUTHDLG, 0);
	SendMessage(mainWnd->GetHMainWnd(), WM_COMMAND, IDM_AUTHDLG, 0);
	return "VladislaV";
}

QString Gui::getMessage()
{
	char TextFromTextBox[1024];
	GetDlgItemTextA(mainWnd->GetHMainWnd(), ID_INPUT_AREA, TextFromTextBox, 100);
	return TextFromTextBox;
}

QString Gui::getRecipient()
{
	return "General";
}

void Gui::slotSendToServer()
{
	_isPublicMessage = true;
	_chatClient->slotSendToServer();
}

LRESULT CALLBACK Gui::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/*MainWindow*/Gui* W = ( /*MainWindow*/Gui* )GetWindowLongPtrA( hWnd, GWLP_USERDATA );
	
	HDC hDC;
	PAINTSTRUCT ps;
	RECT rect;
	
	int userReply;
	int wmId;
	
	switch (uMsg)
	{
	case WM_COMMAND:
		/*
		if (LOWORD(wParam) == IDM_AUTHDLG)
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, AuthDlg);
		*/
		wmId = (int)LOWORD(wParam);
		switch (wmId)
		{
		case IDM_AUTHDLG:
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Gui::AuthDlg);
			break;
		case ID_SEND_BUTTON:
			//W->OnButtonClicked();
			W->slotSendToServer();
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &rect);
		EndPaint(hWnd, &ps);
		break;

	case WM_CLOSE:
		userReply = MessageBox(hWnd, "Do you really want to close the application?",
			"", MB_YESNO | MB_ICONQUESTION);
		if (IDYES == userReply)
		{
			DestroyWindow(hWnd);
		}
		break;

    case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

INT_PTR CALLBACK Gui::AuthDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			GetDlgItemTextA(hDlg, IDC_EDIT1, NickName, 100);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
/*
void Gui::Initialization()
{	
	_szClassName = "MainWindow";
	_windowName	= "ChatClient";
	_menuName = NULL;
	_x = WindowInitialWidth;
	_y = WindowInitialHeight;
	_width = WindowInitialWidth + WindowWidth;
	_height = WindowInitialHeight + WindowHeight;
	
	Init(Gui::WndProc);
	CreateWnd();
	
	SendMessage(hMainWnd, WM_COMMAND, IDM_AUTHDLG, 0); //Why is it not running??
	
	if (!hMainWnd)
	{
		char text[100] = "Cannot create window: ";
		strcat(text, _windowName);
		MessageBox(NULL, text, "Error", MB_OK);
		return;
	}
	
	ShowWindow(hMainWnd, _cmdShow);
}

int Gui::Init(LRESULT (WINAPI *pWndProc)(HWND,UINT,WPARAM,LPARAM))
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

void Gui::CreateWnd()
{
	hMainWnd = CreateWindowA(
						_szClassName.c_str(),
						_windowName,
						//WS_OVERLAPPEDWINDOW |  
						WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
						_x,
						_y,
						_width,
						_height,
						HWND_DESKTOP,
						NULL,
						NULL,
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
									NULL,
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
									ES_READONLY  | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL |WS_BORDER | WS_VISIBLE | WS_CHILD | ES_LEFT,
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
									//ES_READONLY | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL |WS_BORDER | WS_VISIBLE | WS_CHILD | ES_LEFT,
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
									ES_READONLY | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL |WS_BORDER | WS_VISIBLE | WS_CHILD | ES_LEFT,
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
*/

#endif /*_WIN32*/
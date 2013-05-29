/*
 * File:   main.cpp
 * Author: Vladislav
 *
 * Created on September 28, 2012, 11:08 PM
 * 
 * Special Thanks for Dragonway
 */

#define DEBUG
#ifdef DEBUG
	#include <QDebug>
#endif

#include <QtGui/QApplication>

#include "gui.h"

class Gui;

#ifdef	__LINUX__

int main(int argc, char *argv[])
{
	
#ifdef DEBUG
	qDebug() << "The client is starting...";
#endif
	QApplication app(argc, argv);
	Gui *gui = new Gui;
	gui->show();
	return app.exec();
}

#endif	/*__LINUX__*/


#ifdef	_WIN32
#include <shellapi.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef DEBUG
	qDebug() << "The client is starting...";
#endif
	int argc;
	char** argv;
	{
		LPWSTR* lpArgv = CommandLineToArgvW( GetCommandLineW(), &argc );
		argv = (char**)malloc( argc*sizeof(char*) );
		int size, i = 0;
		for( ; i < argc; ++i )
		{
			size = wcslen( lpArgv[i] ) + 1;
			argv[i] = new char[size];
			wcstombs( argv[i], lpArgv[i], size );
		}
		LocalFree( lpArgv );
	}

	QApplication app(argc, argv);
	Gui *gui = new Gui(hInstance, nCmdShow);
	delete gui;
	
	{
		int i = 0;
		for( ; i < argc; ++i )
			//free( argv[i] );
			delete argv[i];
		//free( argv );
		delete[] argv;
	}
	app.exit(0);
	return app.exec();
	
	SetLayout();
}

#endif	/*_WIN32*/
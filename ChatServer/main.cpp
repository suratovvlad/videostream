/*
 * File:   main.cpp
 * Author: Vladislav
 *
 * Created on September 28, 2012, 8:29 PM
 */
#define DEBUG
#ifdef DEBUG
	#include <QDebug>
#endif

#include <QtCore>

#include "server.h"
extern int _main();



int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
#ifdef DEBUG
	qDebug() << "The server is starting...";
#endif
	Server *chatServer = new Server(1991, 0);
	
	
	return _main();
}

/* 
 * File:   gui.h
 * Author: Vladislav
 *
 * Created on 20 Окт�?брь 2012 г., 17:35
 * 
 * Special Thanks for Dragonway
 */

#ifndef GUI_H
#define	GUI_H

#undef _WIN32
#define __LINUX__

#ifdef	_WIN32
	#include "gui/widgets.h"
	#include "gui/gui_win32api.h"
	#include "resources.h"
	#include "gui/mainwindow_win32api.h"
#else
#ifdef	__LINUX__
	#include "gui/gui_qt.h"
#endif
#endif

#endif	/* GUI_H */


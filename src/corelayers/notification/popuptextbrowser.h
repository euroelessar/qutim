/*
    PopupTextBrowser

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef POPUPTEXTBROWSER_H_
#define POPUPTEXTBROWSER_H_

#include <QTextBrowser>

class PopupTextBrowser : public QTextBrowser
{
	Q_OBJECT
public:
	PopupTextBrowser(QWidget *parent = 0){}
	~PopupTextBrowser(){}

signals:
	void closeWindow();
	void startChat();

protected:
	void mouseReleaseEvent ( QMouseEvent * event );
};

#endif /*POPUPTEXTBROWSER_H_*/

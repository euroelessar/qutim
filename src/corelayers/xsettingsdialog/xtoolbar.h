/****************************************************************************
 *  xtoolbar.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef XTOOLBAR_H
#define XTOOLBAR_H

#include <libqutim/actiontoolbar.h>

using namespace qutim_sdk_0_3;

class QMenu;
class XToolBar : public ActionToolBar
{
	Q_OBJECT
public:
    XToolBar ( QWidget* parent = 0 );
protected:
	virtual void contextMenuEvent(QContextMenuEvent* );
private:
	QMenu *m_context_menu;
private slots:
	int getSizePos (const int &size);
	void onSizeActionTriggered(QAction *action);
	void onToolButtonStyleActionTriggered(QAction *action);
};

#endif // XTOOLBAR_H

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

#include <QToolBar>

class QMenu;
class XToolBar : public QToolBar
{
	Q_OBJECT
public:
    XToolBar ( QWidget* parent = 0 );
protected:
	virtual void contextMenuEvent(QContextMenuEvent* );
private:
	QMenu *m_context_menu;
private slots:
	void onActionTriggered(QAction *action);
	void onAnimationActionTriggered(bool checked);
	int getSizePos (const int &size);
	void onSizeActionTriggered(QAction *action);
	void onToolButtonStyleActionTriggered(QAction *action);
};

#endif // XTOOLBAR_H

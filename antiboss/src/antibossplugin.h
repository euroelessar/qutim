/****************************************************************************
 * antibossplugin.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef ANTIBOSSPLUGIN_H
#define ANTIBOSSPLUGIN_H

#include <qutim/plugin.h>
#include <qutim/shortcut.h>
#include <QSet>

using namespace qutim_sdk_0_3;

class AntiBossPlugin : public Plugin
{
	Q_OBJECT
public:
	AntiBossPlugin();
	virtual void init();
	virtual bool load();
	virtual bool unload();
	bool eventFilter(QObject *, QEvent *);
public slots:
	void showHide();
private:
	bool m_hidden;
	GlobalShortcut *m_shortcut;
	QMultiHash<Qt::WindowStates, QPointer<QWidget> > m_widgets;
};

#endif // ANTIBOSSPLUGIN_H

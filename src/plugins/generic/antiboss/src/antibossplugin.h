/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

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
	QMultiHash<Qt::WindowStates, QWeakPointer<QWidget> > m_widgets;
};

#endif // ANTIBOSSPLUGIN_H


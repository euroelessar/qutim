/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef KINETICSCROLLER_H
#define KINETICSCROLLER_H
#include <QObject>
#include <QSet>

namespace Core {

class KineticScroller : public QObject
{
	Q_CLASSINFO("Service", "Scroller")
	Q_OBJECT
public:
    KineticScroller();
	virtual ~KineticScroller();
	Q_INVOKABLE void enableScrolling(QObject *widget);
public slots:
	void loadSettings();
protected slots:
	void onWidgetDeath(QObject *widget);
private:
	QSet<QObject*> m_widgets;
	int m_scrollingType;
};

} // namespace Core

#endif // KINETICSCROLLER_H


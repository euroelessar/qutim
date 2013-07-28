/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Nikita Belov <null@deltaz.org>
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

#ifndef DGLOBALHOTKEY_H
#define DGLOBALHOTKEY_H

#include <QObject>
#include <QAbstractEventDispatcher>
#include <QAbstractNativeEventFilter>
#include <QKeySequence>
#include <QMap>
#include <QHash>

class dGlobalHotKey : public QObject, public QAbstractNativeEventFilter
{
	Q_OBJECT

public:
	dGlobalHotKey();
	~dGlobalHotKey();

	static bool eventFilter( void *e );
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);
	static dGlobalHotKey *instance();

	bool shortcut( const QString &s, bool a = true );
	quint32 id( const QString &s );



signals:
	void hotKeyPressed( quint32 k );

private:
	void native( const QString &s, quint32 &k, quint32 &m );

	quint32 nativeModifiers( Qt::KeyboardModifiers m );
	quint32 nativeKeycode( Qt::Key k );
#if defined( Q_WS_X11 )
    bool error;
#endif
};

#endif // DGLOBALHOTKEY_H


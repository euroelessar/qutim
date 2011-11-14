/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef SHORTCUT_H
#define SHORTCUT_H

#include <QShortcut>
#include "localizedstring.h"

namespace qutim_sdk_0_3
{
class ShortcutPrivate;
class GlobalShortcutPrivate;

// REMOVE ME, it's ugly
struct KeySequence
{
	QString id;
	LocalizedString name;
	LocalizedString group;
	QKeySequence key;
	Qt::ShortcutContext context;
};

class LIBQUTIM_EXPORT Shortcut : public QShortcut
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(Shortcut)
public:
	explicit Shortcut(const QString &id, QWidget *parent);
	virtual ~Shortcut();

	static bool registerSequence(const QString &id, const LocalizedString &name,
							   const LocalizedString &group, const QKeySequence &key,
							   Qt::ShortcutContext context = Qt::WindowShortcut);
	static QStringList ids();
	static KeySequence getSequence(const QString &id);
	static void setSequence(const QString &id,const QKeySequence &key);
protected:
	QScopedPointer<ShortcutPrivate> d_ptr;
};

class LIBQUTIM_EXPORT GlobalShortcut : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(GlobalShortcut)
public:
	explicit GlobalShortcut(const QString &id, QObject *parent = 0);
	virtual ~GlobalShortcut();

	static bool registerSequence(const QString &id, const LocalizedString &name,
							   const LocalizedString &group, const QKeySequence &key);
	static QStringList ids();
	static KeySequence getSequence(const QString &id);
	static void setSequence(const QString &id,const QKeySequence &key);
signals:
	void activated();
private slots:
	void onHotKeyPressed(quint32);
protected:
	QScopedPointer<GlobalShortcutPrivate> d_ptr;
};
}

#endif // SHORTCUT_H


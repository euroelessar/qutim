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

#ifndef ICONBACKEND_P_H
#define ICONBACKEND_P_H

#include <QIconEngine>

struct IconPrivateHelper
{
	QIconEngine *engine;
};

class IconBackend : public QIconEngine
{
public:
	IconBackend(const QString &name);
	inline IconBackend() {}
	virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state);
	virtual QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state);
	virtual QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state);

	virtual void addPixmap(const QPixmap &pixmap, QIcon::Mode mode, QIcon::State state);
	virtual void addFile(const QString &fileName, const QSize &size, QIcon::Mode mode, QIcon::State state);

	virtual QString key() const;
	virtual QIconEngine *clone() const;
	virtual bool read(QDataStream &in);
	virtual bool write(QDataStream &out) const;
	virtual void virtual_hook(int id, void *data);
private:
	inline const QIcon &currentIcon() { return m_theme_icon; }
	// I really hope, that there will be no version 1 engine
	inline QIconEngine *currentIconEngine() { return reinterpret_cast<IconPrivateHelper *>(m_theme_icon.data_ptr())->engine; }
	QString m_name;
	QIcon   m_theme_icon;
};

#endif // ICONBACKEND_P_H


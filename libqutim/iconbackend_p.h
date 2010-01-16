/****************************************************************************
 *  iconbackend_p.h
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

#ifndef ICONBACKEND_P_H
#define ICONBACKEND_P_H

#include <QIconEngineV2>

struct IconPrivateHelper
{
	QIconEngineV2 *engine;
};

class IconBackend : public QIconEngineV2
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
	virtual QIconEngineV2 *clone() const;
	virtual bool read(QDataStream &in);
	virtual bool write(QDataStream &out) const;
	virtual void virtual_hook(int id, void *data);
private:
	inline const QIcon &currentIcon() { return m_theme_icon; }
	// I really hope, that there will be no version 1 engine
	inline QIconEngineV2 *currentIconEngine() { return reinterpret_cast<IconPrivateHelper *>(m_theme_icon.data_ptr())->engine; }
	QString m_name;
	QIcon   m_theme_icon;
};

#endif // ICONBACKEND_P_H

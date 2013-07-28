/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef AVATARICONENGINE_P_H
#define AVATARICONENGINE_P_H

#include <QIconEngine>

namespace qutim_sdk_0_3
{

/**
  @private
*/
class AvatarIconEngine : public QIconEngine
{
public:
	AvatarIconEngine(const QString &path,const QIcon &overlay);
	virtual ~AvatarIconEngine();
	virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state);
	virtual QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state);
	virtual QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state);
	virtual QString key() const;
    virtual AvatarIconEngine *clone() const;
private:
	QString m_path;
	QIcon m_overlay;
};

} //qutim_sdk_0_3
#endif // AVATARICONENGINE_P_H

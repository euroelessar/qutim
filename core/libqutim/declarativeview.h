/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef QUTIM_SDK_0_3_DECLARATIVEVIEW_H
#define QUTIM_SDK_0_3_DECLARATIVEVIEW_H
#include <QGraphicsView>
#include "libqutim_global.h"

class QDeclarativeEngine;
class QDeclarativeContext;
class QDeclarativeItem;
class QUrl;

namespace qutim_sdk_0_3 {

class DeclarativeViewPrivate;

class LIBQUTIM_EXPORT DeclarativeView : public QGraphicsView
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(DeclarativeView)

	Q_PROPERTY(Status status READ status NOTIFY statusChanged)
	Q_PROPERTY(QUrl source READ source WRITE setSource DESIGNABLE true)
	Q_ENUMS(Status ResizeMode)
public:
	explicit DeclarativeView(QWidget *parent = 0);
	virtual ~DeclarativeView();

	static void setEngine(QDeclarativeEngine *engine);
	static QDeclarativeEngine *engine();

	void setSource(const QUrl &source);
	QUrl source() const;
	QDeclarativeContext *rootContext() const;
	QObject *rootObject() const;
	enum Status { Null, Ready, Loading, Error };
	Status status() const;
	enum ResizeMode { SizeViewToRootObject, SizeRootObjectToView };
	ResizeMode resizeMode() const;
	void setResizeMode(ResizeMode);
signals:
	void sceneResized(const QSize &size);
	void statusChanged(qutim_sdk_0_3::DeclarativeView::Status);
protected:
	virtual void resizeEvent(QResizeEvent *event);
	virtual void setRootObject(QObject *object);
	virtual bool eventFilter(QObject *obj, QEvent *ev);

	QScopedPointer<DeclarativeViewPrivate> d_ptr;
private:
	Q_PRIVATE_SLOT(d_func(), void _q_updateView())
	Q_PRIVATE_SLOT(d_func(), void _q_continueExecute())
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_DECLARATIVEVIEW_H

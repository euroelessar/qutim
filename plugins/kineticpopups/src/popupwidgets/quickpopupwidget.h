/****************************************************************************
**
** qutIM - instant messenger
**
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

#ifndef KINETICPOPUPS_QUICKPOPUPWIDGET_H
#define KINETICPOPUPS_QUICKPOPUPWIDGET_H
#include "../kineticpopup.h"
#include <QtDeclarative>
#include <QTimer>

namespace qutim_sdk_0_3 {
class DeclarativeView;
} //namespace qutim

class QDeclarativeView;
namespace KineticPopups {

class QuickNotify : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString title READ title)
	Q_PROPERTY(QString text  READ text)
	Q_PROPERTY(QObject* object READ object)
	Q_PROPERTY(qutim_sdk_0_3::Notification::Type type READ type)
	Q_PROPERTY(QObjectList actions READ actions)
public:
	QuickNotify(qutim_sdk_0_3::Notification *notify);
	QString title() const;
	QString text() const;
	QObject* object() const;
	qutim_sdk_0_3::Notification::Type type() const;
	QObjectList actions() const;
private slots:
	void onActionTriggered();
private:
	qutim_sdk_0_3::Notification *m_notify;
	QObjectList m_actions;
};

class PopupAttributes : public QObject
{
	Q_OBJECT
	Q_ENUMS(FrameStyle)
	Q_PROPERTY(FrameStyle frameStyle READ frameStyle WRITE setFrameStyle NOTIFY frameStyleChanged)
public:
	enum FrameStyle
	{
		ToolTip,
		Normal, /** \ Normal window */
		Tool, /** \note Unsupported on X11 platform */
		ToolTipBlurAero,  /** \note Only Win7 Supported */
		ToolBlurAero,  /** \note Only Win7 Supported */
		AeroBlur  /** \note Only Win7 Supported */
	};
public:
	PopupAttributes(QObject *parent = 0);
	void setFrameStyle(FrameStyle frameStyle);
	FrameStyle frameStyle() const;
signals:
	void frameStyleChanged(KineticPopups::PopupAttributes::FrameStyle);
private:
	FrameStyle m_frameStyle;
};

class QuickPopupWidget : public PopupWidget
{
	Q_OBJECT
public:
    QuickPopupWidget(QWidget *parent = 0);
	virtual void addNotification(qutim_sdk_0_3::Notification* notify);
	virtual qutim_sdk_0_3::NotificationList notifications() const;
	virtual ~QuickPopupWidget();
	virtual QSize sizeHint() const;
	void setPopupAttributes(PopupAttributes *attributes = 0);
	virtual void loadTheme(const QString &themePath);
signals:
	void notifyAdded(QObject *notify);
public slots:
	void ignore();
	void accept();
	void reject();
	void timeout();
private slots:
	void onAtributesChanged();
protected:
	void mouseReleaseEvent(QMouseEvent *event);
	virtual bool event(QEvent *ev);
private:
	QHash<qutim_sdk_0_3::Notification*, QuickNotify*>  m_notifyHash;
    qutim_sdk_0_3::DeclarativeView *m_view;
	QTimer m_timeout;
};

} // namespace KineticPopups
Q_DECLARE_METATYPE(KineticPopups::QuickNotify*)
Q_DECLARE_METATYPE(KineticPopups::PopupAttributes*)

#endif // KINETICPOPUPS_QUICKPOPUPWIDGET_H


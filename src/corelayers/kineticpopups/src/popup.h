//   Modern notifications, which use new Qt Statemachine and Animation framework
//   (c) by Sidorov "Sauron" Aleksey, sauron@citadelspb.com, 2010
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Library General Public
//   License version 2 or later as published by the Free Software Foundation.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Library General Public License for more details.
//
//   You should have received a copy of the GNU Library General Public License
//   along with this library; see the file COPYING.LIB.  If not, write to
//   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
//   Boston, MA 02110-1301, USA.

#ifndef POPUP_H
#define POPUP_H

#include <QObject>
#include <QRect>
#include <QVariant>

namespace qutim_sdk_0_3
{
class Notification;
}

class QFinalState;
class QPropertyAnimation;
class QState;
class QStateMachine;
namespace Core
{
namespace KineticPopups
{
class AbstractPopupWidget;
class Popup : public QObject
{
	Q_OBJECT
public:
	Popup(const QString &id); //0 - persistant
    Popup(qutim_sdk_0_3::Notification *, QObject* parent = 0);
	virtual ~Popup();
	void setId(const QString &id);
	void setMessage(const QString &title, const QString &body = NULL, QObject *sender = NULL);
	void appendMessage(const QString &message);
	void setData(const QVariant &data);
	void updateMessage(const QString &message);
	void send();
	void update(QRect geom);
	QString getId() const;
	QRect geometry() const; //Show state geometry
signals:
	void updated();
private slots:
	void onPopupWidgetSizeChanged(const QSize &size);
	void onPopupActivated();
private:
	void updateGeometry(const QRect &newGeometry);
	inline void updateMessage();
	AbstractPopupWidget *m_popup;
	QString m_title;
	QString m_body;
	QString m_id;
	QRect m_show_geometry; //Don't use direct, change by UpdateGeometry!
	QStateMachine *m_machine;
	QState *m_show_state;
	QFinalState *m_hide_state;
	QPropertyAnimation *m_moving_animation;
	QPropertyAnimation *m_opacity_animation;
	QObject *m_sender;
	QVariant m_data;
};
}
}
#endif // POPUP_H

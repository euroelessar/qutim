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
#ifndef CONFERENCECONTACTSVIEW_H
#define CONFERENCECONTACTSVIEW_H

#include <QListView>
#include "chatlayer_global.h"

namespace Core
{
namespace AdiumChat
{

class ChatSessionImpl;
class ConferenceContactsViewPrivate;
class ADIUMCHAT_EXPORT ConferenceContactsView : public QListView
{
    Q_OBJECT
	Q_DECLARE_PRIVATE(ConferenceContactsView)
public:
    explicit ConferenceContactsView(QWidget *parent = 0);
	void setSession(ChatSessionImpl *session);
	virtual ~ConferenceContactsView();
protected:
	void mouseReleaseEvent(QMouseEvent *e);
	virtual bool event(QEvent *event);
	void changeEvent(QEvent *);
private:
	QScopedPointer<ConferenceContactsViewPrivate> d_ptr;
	Q_PRIVATE_SLOT(d_func(), void _q_activated(const QModelIndex &))
	Q_PRIVATE_SLOT(d_func(), void _q_init_scrolling())
	Q_PRIVATE_SLOT(d_func(), void _q_insert_nick())
	Q_PRIVATE_SLOT(d_func(), void _q_service_changed(const QByteArray &, QObject *))
};

}
}
#endif // CONFERENCECONTACTSVIEW_H


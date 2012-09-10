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
#include "conferencecontactsview.h"
#include "chatsessionimpl.h"
#include <qutim/conference.h>
#include <qutim/mimeobjectdata.h>
#include <QDropEvent>
#include <qutim/servicemanager.h>
#include <QAbstractItemDelegate>
#include <qutim/servicemanager.h>
#include <qutim/adiumchat/chatlayerimpl.h>
#include "chatforms/abstractchatform.h"
#include <QTextEdit>
#include <QPlainTextEdit>

namespace Core
{
namespace AdiumChat
{
using namespace qutim_sdk_0_3;

class ConferenceContactsViewPrivate
{
	Q_DECLARE_PUBLIC(ConferenceContactsView)
public:
	ConferenceContactsViewPrivate(ConferenceContactsView *q) : q_ptr(q), session(0) {}
	ConferenceContactsView *q_ptr;
	ChatSessionImpl *session;
	QAction *action;
	void _q_activated(const QModelIndex &index)
	{
		Buddy *buddy = index.data(Qt::UserRole).value<Buddy*>();
		if (buddy)
			ChatLayer::get(buddy, true)->activate();
	}
	void _q_init_scrolling()
	{
		if(QObject *scroller = ServiceManager::getByName("Scroller"))
			QMetaObject::invokeMethod(scroller,
									  "enableScrolling",
									  Q_ARG(QObject*, q_func()->viewport()));
	}
	void _q_insert_nick()
	{
		Q_Q(ConferenceContactsView);
		QModelIndex index = q->currentIndex();
		Buddy *buddy = index.data(Qt::UserRole).value<Buddy*>();
		if (buddy) {
			QString nick = buddy->title();
			AbstractChatForm *form = ServiceManager::getByName<AbstractChatForm*>("ChatForm");
			QObject *obj = form->textEdit(session);
			QTextCursor cursor;
			if (QTextEdit *edit = qobject_cast<QTextEdit*>(obj))
				cursor = edit->textCursor();
			else if (QPlainTextEdit *edit = qobject_cast<QPlainTextEdit*>(obj))
				cursor = edit->textCursor();
			else
				return;
			if(cursor.atStart())
				cursor.insertText(nick + ": ");
			else
				cursor.insertText(nick + " ");
			static_cast<QWidget*>(obj)->setFocus();
		}
	}

	void _q_service_changed(const QByteArray &name, QObject *service)
	{
		if (name == "ContactDelegate")
			q_func()->setItemDelegate(qobject_cast<QAbstractItemDelegate*>(service));
	}
};

ConferenceContactsView::ConferenceContactsView(QWidget *parent) :
	QListView(parent),
	d_ptr(new ConferenceContactsViewPrivate(this))
{
	Q_D(ConferenceContactsView);
	setItemDelegate(ServiceManager::getByName<QAbstractItemDelegate*>("ContactDelegate"));
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	setSizePolicy(sizePolicy);
	setAcceptDrops(true);
	connect(this, SIGNAL(activated(QModelIndex)), SLOT(_q_activated(QModelIndex)));

	d->action = new QAction(tr("Insert Nick"),this);
	d->action->setSoftKeyRole(QAction::NegativeSoftKey);
//	connect(this, SIGNAL(activated(QModelIndex)), SLOT(_q_insert_nick()));
	addAction(d->action);

	QTimer::singleShot(0, this, SLOT(_q_init_scrolling()));
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

	setWindowTitle(tr("Conference participants"));

	connect(ServiceManager::instance(), SIGNAL(serviceChanged(QByteArray,QObject*,QObject*)),
			SLOT(_q_service_changed(QByteArray,QObject*)));
}

void ConferenceContactsView::setSession(ChatSessionImpl *session)
{
	Q_D(ConferenceContactsView);
	d->session = session;
	setModel(session->getModel());
	bool isContactsViewVisible = session->getModel()->rowCount(QModelIndex()) > 0;
	if(qobject_cast<Conference *>(session->getUnit()))
		isContactsViewVisible = true;

	setVisible(isContactsViewVisible);
	//connect(session,SIGNAL(buddiesChanged()),SLOT(onBuddiesChanged()));
}

bool ConferenceContactsView::event(QEvent *event)
{
	Q_D(ConferenceContactsView);
	if (event->type() == QEvent::ContextMenu) {
		QContextMenuEvent *menuEvent = static_cast<QContextMenuEvent*>(event);
		QModelIndex index = indexAt(menuEvent->pos());
		Buddy *buddy = index.data(Qt::UserRole).value<Buddy*>();
		if (buddy)
			buddy->showMenu(menuEvent->globalPos());
		return true;
	} else if (event->type() == QEvent::DragEnter) {
		QDragEnterEvent *dragEvent = static_cast<QDragEnterEvent*>(event);
		if (const MimeObjectData *data = qobject_cast<const MimeObjectData*>(dragEvent->mimeData())) {
			Contact *contact = qobject_cast<Contact*>(data->object());
			Conference *conf = qobject_cast<Conference*>(d->session->getUnit());
			if (contact && conf && contact->account() == conf->account())
				dragEvent->acceptProposedAction();
		}
		return true;
	} else if (event->type() == QEvent::Drop) {
		QDropEvent *dropEvent = static_cast<QDropEvent*>(event);
		if (const MimeObjectData *mimeData
				= qobject_cast<const MimeObjectData*>(dropEvent->mimeData())) {
			if (Contact *contact = qobject_cast<Contact*>(mimeData->object())) {
				ChatUnit *unit = d->session->getUnit();
				if (Conference *conf = qobject_cast<Conference*>(unit))
					conf->invite(contact);
				dropEvent->setDropAction(Qt::CopyAction);
				dropEvent->accept();
				return true;
			}
		}
	}
	return QListView::event(event);
}

ConferenceContactsView::~ConferenceContactsView()
{
	
}

void ConferenceContactsView::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == Qt::MiddleButton) {
		d_func()->_q_insert_nick();
	} else {
		QListView::mouseReleaseEvent(e);
	}
}

void ConferenceContactsView::changeEvent(QEvent *ev)
{
	if (ev->type() == QEvent::LanguageChange) {
		d_func()->action->setText(tr("Private"));
	}
}

//move to chatforms
//void ConferenceContactsView::onBuddiesChanged()
//{
//	//
//	//if (qobject_cast<Conference*>(m_session->getUnit()))
//	//	setVisible(true);
//	//else
//	//	setVisible(model()->rowCount(QModelIndex()) > 0);
//}

}
}

#include "moc_conferencecontactsview.cpp"


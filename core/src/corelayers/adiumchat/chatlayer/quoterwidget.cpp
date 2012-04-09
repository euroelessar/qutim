/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "quoterwidget.h"
#include <QDateTime>
#include <qutim/chatunit.h>
#include <qutim/account.h>
#include <qutim/conference.h>
#include <qutim/shortcut.h>
#include <QApplication>

namespace Core {
namespace AdiumChat {

using namespace qutim_sdk_0_3;

enum Role {
	MessageRole = Qt::UserRole + 1
};

// TODO: Remove duplication of code
QString getUnitNick(const qutim_sdk_0_3::Message &message)
{
	QObject *source = 0;
	QString id = message.property("senderId", QString());
	QString title = message.property("senderName", QString());
	if (!title.isEmpty()) {
		if (!id.isEmpty())
			source = message.chatUnit()->account()->getUnit(id, false);
		return title;
	}
	if (!source && message.chatUnit()) {
		if (!message.isIncoming()) {
			const Conference *conf = qobject_cast<const Conference*>(message.chatUnit());
			if (conf && conf->me())
				source = conf->me();
			else
				source = message.chatUnit()->account();
		} else {
			source = message.chatUnit();
		}
	}
	if (!source)
		return title;
	if (ChatUnit *unit = qobject_cast<ChatUnit*>(source)) {
		id = unit->id();
		title = unit->title();
	} else if (Account *account = qobject_cast<Account*>(source)) {
		id = account->id();
		title = account->name();
	}
	return title;
}

QuoterWidget::QuoterWidget(const MessageList &messages, QObject *controller)
    : m_controller(controller)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
	foreach (const Message &message, messages) {
		QListWidgetItem *item = new QListWidgetItem(this);
		item->setData(MessageRole, qVariantFromValue(message));
		QString text = getUnitNick(message);
		text += message.time().toString(QLatin1String(" (hh:mm:ss)"));
		text += QLatin1String("\n");
		text += message.text();
		item->setText(text);
	}
	setCurrentRow(count() - 1, QItemSelectionModel::SelectCurrent);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	QWidget *activeWindow = qApp->activeWindow();
	debug() << Q_FUNC_INFO << sizeHint() << size();
	QRect rect = activeWindow->geometry();
	QSize size = sizeHint().boundedTo(activeWindow->size());
	QPoint position(rect.left() + (rect.width() - size.width()) / 2,
					rect.top() + (rect.height() - size.height()) / 2);
	setGeometry(QRect(position, size));

	Shortcut *shortcut = new Shortcut(QLatin1String("quote"), this);
	connect(shortcut, SIGNAL(activated()), SLOT(nextRow()));
	show();
	setFocus(Qt::PopupFocusReason);
	debug() << "Try to show";
	// Magic number!
	m_timer.setInterval(800);
	connect(&m_timer, SIGNAL(timeout()), SLOT(deleteLater()));
	m_timer.start();
}

QuoterWidget::~QuoterWidget()
{
	debug() << Q_FUNC_INFO << sizeHint() << size();
	debug() << "Try to die";
	if (m_controller)
		emit quoteChoosed(currentItem()->text(), m_controller.data());
}

void QuoterWidget::nextRow()
{
	setCurrentRow((currentRow() + count() - 1) % count(), QItemSelectionModel::SelectCurrent);
	m_timer.start();
}

} // namespace AdiumChat
} // namespace Core

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

#include "quickanswerbuttonaction.h"
#include "rostermanager.h"
#include <qutim/chatsession.h>
#include <QDesktopWidget>
#include <QApplication>
#include <QTimer>

namespace Control {

using namespace qutim_sdk_0_3;

QuickAnswerButtonActionGenerator::QuickAnswerButtonActionGenerator(QObject *object, const char *slot)
    : ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("Control", "Quick answer"), object, slot)
{
	setType(ActionTypeChatButton);
}

QuickAnswerButtonAction::QuickAnswerButtonAction(QObject *parent)
    : QAction(parent)
{
}

QuickAnswerMenu::QuickAnswerMenu(Contact *contact) : m_contact(contact)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
	NetworkManager *manager = RosterManager::instance()->networkManager();
	connect(manager, SIGNAL(answersChanged(QStringList)), SLOT(onAnswersChanged(QStringList)));
	onAnswersChanged(manager->answers());
	setGeometry(qApp->desktop()->availableGeometry(QCursor::pos()));
	show();
	connect(contact, SIGNAL(destroyed()), SLOT(close()));
	connect(this, SIGNAL(itemPressed(QListWidgetItem*)),
	        SLOT(onItemClicked(QListWidgetItem*)));
}

void QuickAnswerMenu::mousePressEvent(QMouseEvent *ev)
{
	QListWidget::mousePressEvent(ev);
	QTimer::singleShot(100, this, SLOT(close()));
}

void QuickAnswerMenu::onAnswersChanged(const QStringList &answers)
{
	clear();
	foreach (const QString &answer, answers)
		new QListWidgetItem(answer, this);
}

void QuickAnswerMenu::onItemClicked(QListWidgetItem *item)
{
	ChatSession *session = ChatLayer::get(m_contact);
	qDebug() << item->text() << session << session->getInputField();
	QMetaObject::invokeMethod(ChatLayer::instance(),
	                          "insertText",
	                          Q_ARG(qutim_sdk_0_3::ChatSession*, session),
	                          Q_ARG(QString, item->text()));
	close();
}

} // namespace Control

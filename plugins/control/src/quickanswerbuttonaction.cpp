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
#include <qutim/debug.h>
#include <QDesktopWidget>
#include <QToolButton>
#include <QApplication>
#include <QTimer>
#include <QVBoxLayout>
#include <QPainter>

namespace Control {

using namespace qutim_sdk_0_3;

QuickAnswerButtonActionGenerator::QuickAnswerButtonActionGenerator(QObject *object, const char *slot)
    : ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("Control", "Answer"), object, slot)
{
	setType(ActionTypeChatButton);
}

QObject *QuickAnswerButtonActionGenerator::generateHelper() const
{
	return prepareAction(new QuickAnswerButtonAction(NULL));
}

QuickAnswerButtonAction::QuickAnswerButtonAction(QObject *parent)
    : QWidgetAction(parent)
{
}

QWidget *QuickAnswerButtonAction::createWidget(QWidget *parent)
{
	QToolButton *button = new QToolButton(parent);
	button->setText(tr("Answers"));
	//simple hack
	setMenu(new QMenu);
	button->setDefaultAction(this);
	menu()->deleteLater();
	button->setToolButtonStyle(Qt::ToolButtonTextOnly);
	button->setAutoRaise(false);
	QMenu *menu = new QMenu(button);
	QAction *action = menu->addAction(tr("Update answers"));
	connect(action, SIGNAL(triggered()),
	        RosterManager::instance()->networkManager(), SLOT(updateAnswers()));
	button->setMenu(menu);
	return button;
}

void QuickAnswerButtonAction::deleteWidget(QWidget *widget)
{
	QWidgetAction::deleteWidget(widget);
}

ItemDeledate::ItemDeledate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void ItemDeledate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt4(option);
	opt4.features |= QStyleOptionViewItemV4::WrapText;
	if (opt4.state & QStyle::State_MouseOver) {
		opt4.state &= ~QStyle::State_MouseOver;
		opt4.state |= QStyle::State_Selected;
	}
	QStyledItemDelegate::paint(painter, opt4, index);
	painter->setBrush(QBrush(Qt::transparent));
	QPen pen(Qt::gray);
	pen.setStyle(Qt::DashLine);
	painter->setPen(pen);
	painter->drawLine(opt4.rect.bottomLeft(), opt4.rect.bottomRight());
}

QSize ItemDeledate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt4(option);
	opt4.features |= QStyleOptionViewItemV4::WrapText;
	
	return QStyledItemDelegate::sizeHint(opt4, index);
}

QuickAnswerMenu::QuickAnswerMenu(Contact *contact) : m_contact(contact)
{
	setDragEnabled(false);
	setVerticalScrollMode(ScrollPerPixel);
	setItemDelegate(new ItemDeledate(this));
	setAlternatingRowColors(true);
	NetworkManager *manager = RosterManager::instance()->networkManager();
	connect(manager, SIGNAL(answersChanged(QStringList)), SLOT(onAnswersChanged(QStringList)));
	onAnswersChanged(manager->answers());
	QMenu *menu = new QMenu();
	menu->setAttribute(Qt::WA_DeleteOnClose);
//	QWidgetAction *action = new QWidgetAction(menu);
//	action->setDefaultWidget(this);
//	menu->addAction(action);
	QVBoxLayout *layout = new QVBoxLayout(menu);
	layout->setMargin(0);
	layout->addWidget(this);
	menu->setLayout(layout);
	layout->addWidget(this);
	QRect geom = qApp->desktop()->availableGeometry(QCursor::pos());
	if (QCursor::pos().x() < geom.x() + geom.width() / 2)
		geom.setX(geom.x() + geom.width() / 2);
	else
		geom.setY(geom.y() - geom.width() / 2);
	menu->setGeometry(geom);
	menu->show();
	connect(contact, SIGNAL(destroyed()), SLOT(close()));
	connect(this, SIGNAL(itemPressed(QListWidgetItem*)),
	        SLOT(onItemClicked(QListWidgetItem*)));
}

void QuickAnswerMenu::mousePressEvent(QMouseEvent *ev)
{
	QListWidget::mousePressEvent(ev);
	QTimer::singleShot(100, window(), SLOT(close()));
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
	debug() << item->text() << session << session->getInputField();
	QMetaObject::invokeMethod(ChatLayer::instance(),
	                          "insertText",
	                          Q_ARG(qutim_sdk_0_3::ChatSession*, session),
	                          Q_ARG(QString, item->text()));
	close();
}

} // namespace Control

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

#include "stackedchatbehavior.h"
#include "ui_stackedchatbehavior.h"
#include <QRadioButton>
#include <QButtonGroup>
#include <qutim/config.h>
#include <qutim/debug.h>

namespace Core
{
namespace AdiumChat
{

StackedChatBehavior::StackedChatBehavior() :
	ui(new Ui::StackedChatBehavior),
	m_group(new QButtonGroup(this))
{
	ui->setupUi(this);
	QRadioButton *btn = new QRadioButton(tr("Ctrl+Enter"),this);
	ui->groupBox->layout()->addWidget(btn);
	m_group->addButton(btn,AdiumChat::SendCtrlEnter);
	btn = new QRadioButton(tr("Double Enter"),this);
	ui->groupBox->layout()->addWidget(btn);
	m_group->addButton(btn,AdiumChat::SendDoubleEnter);
	btn = new QRadioButton(tr("Enter"),this);
	ui->groupBox->layout()->addWidget(btn);
	m_group->addButton(btn,AdiumChat::SendEnter);

	connect(m_group,SIGNAL(buttonClicked(int)),SLOT(onButtonClicked(int)));
	lookForWidgetState(ui->storeBox);
	lookForWidgetState(ui->recentBox);
	lookForWidgetState(ui->groupUntil);
	lookForWidgetState(ui->autoresizeBox);
}

StackedChatBehavior::~StackedChatBehavior()
{
	delete ui;
}

void StackedChatBehavior::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void StackedChatBehavior::loadImpl()
{
	Config cfg = Config("appearance");
	Config widget = cfg.group("chat/behavior/widget");
	m_flags = widget.value("widgetFlags", DeleteSessionOnClose
						   | SwitchDesktopOnActivate
						   | SendTypingNotification
						   );

	m_send_message_key = widget.value("sendKey", AdiumChat::SendEnter);
	static_cast<QRadioButton *>(m_group->button(m_send_message_key))->setChecked(true);
	ui->autoresizeBox->setChecked(widget.value<bool>("autoResize",false));
	Config history = cfg.group("chat/history");
	ui->storeBox->setChecked(history.value<bool>("storeServiceMessages", true));
	ui->recentBox->setValue(history.value<int>("maxDisplayMessages", 5));
	Config chat = cfg.group("chat");
	ui->groupUntil->setValue(chat.value<int>("groupUntil", 900));
}

void StackedChatBehavior::saveImpl()
{
	Config appearance = Config("appearance");
	Config widget = appearance.group("chat/behavior/widget");

	widget.setValue("sendKey",m_send_message_key);
	widget.setValue("widgetFlags",m_flags);
	widget.setValue("autoResize",ui->autoresizeBox->isChecked());
	Config history = appearance.group("chat/history");
	history.setValue("storeServiceMessages",ui->storeBox->isChecked());
	history.setValue("maxDisplayMessages",ui->recentBox->value());
	Config chat = appearance.group("chat");
	chat.setValue("groupUntil",ui->groupUntil->value());
	appearance.sync();
}

void StackedChatBehavior::cancelImpl()
{

}

void StackedChatBehavior::setFlags(AdiumChat::ChatFlags type, bool set)
{
	if (set) {
		m_flags |= type;
	}
	else {
		m_flags &= ~type;
	}
}

void StackedChatBehavior::onButtonClicked(int id)
{
	m_send_message_key = static_cast<AdiumChat::SendMessageKey>(id);
	setModified(true);
}

}
}


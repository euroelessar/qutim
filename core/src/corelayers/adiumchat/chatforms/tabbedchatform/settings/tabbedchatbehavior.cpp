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
#include "tabbedchatbehavior.h"
#include "ui_tabbedchatbehavior.h"
#include <QRadioButton>
#include <QButtonGroup>
#include <qutim/config.h>
#include <qutim/debug.h>

namespace Core
{
namespace AdiumChat
{

TabbedChatBehavior::TabbedChatBehavior() :
	ui(new Ui::TabbedChatBehavior),
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

	ui->tabPositionBox->addItem(tr("North"),false);
	ui->tabPositionBox->addItem(tr("South"),true);
	ui->formLayoutBox->addItem(tr("Classic"),false);
	ui->formLayoutBox->addItem(tr("Adium-like"),true);

#if defined(Q_WS_MAC) || defined(Q_WS_WIN)
	int row;
	QFormLayout::ItemRole role;
	ui->formLayout->getWidgetPosition(ui->menuBox, &row, &role);
	ui->formLayout->removeItem(ui->formLayout->itemAt(row, role));
	ui->menuBox->setVisible(false);
#endif

#ifdef Q_WS_MAC
	ui->formLayout->getWidgetPosition(ui->formLayoutBox, &row, &role);
	ui->formLayout->removeItem(ui->formLayout->itemAt(row, QFormLayout::LabelRole));
	ui->formLayout->removeItem(ui->formLayout->itemAt(row, QFormLayout::FieldRole));
	ui->label_4->hide();
	ui->formLayoutBox->hide(); //Classic layout is really ugly
#endif

	connect(m_group,SIGNAL(buttonClicked(int)),SLOT(onButtonClicked(int)));
	lookForWidgetState(ui->storeBox);
	lookForWidgetState(ui->recentBox);
	lookForWidgetState(ui->groupUntil);
	lookForWidgetState(ui->tabPositionBox);
	lookForWidgetState(ui->formLayoutBox);
	lookForWidgetState(ui->stateBox);
	lookForWidgetState(ui->menuBox);
	lookForWidgetState(ui->customIconBox);
	lookForWidgetState(ui->autoresizeBox);
	lookForWidgetState(ui->rosterBox);
	lookForWidgetState(ui->receiverIdBox);
	lookForWidgetState(ui->fontSizeBox);
}

TabbedChatBehavior::~TabbedChatBehavior()
{
	delete ui;
}

void TabbedChatBehavior::changeEvent(QEvent *e)
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

void TabbedChatBehavior::loadImpl()
{
	Config cfg = Config("appearance");
	Config widget = cfg.group("chat/behavior/widget");
	m_flags = widget.value("widgetFlags", IconsOnTabs
						   | DeleteSessionOnClose
						   | SwitchDesktopOnActivate
						   | AdiumToolbar
						   | TabsOnBottom
						   );

	ui->tabPositionBox->setCurrentIndex(m_flags & TabsOnBottom ? 1 : 0);
	ui->formLayoutBox->setCurrentIndex(m_flags & AdiumToolbar ? 1 : 0);

	m_send_message_key = widget.value("sendKey", AdiumChat::SendCtrlEnter);
	m_autoresize = widget.value("autoResize", false);
	static_cast<QRadioButton *>(m_group->button(m_send_message_key))->setChecked(true);
	Config history = cfg.group("chat/history");
	ui->storeBox->setChecked(history.value<bool>("storeServiceMessages", true));
	ui->recentBox->setValue(history.value<int>("maxDisplayMessages",5));
	ui->stateBox->setChecked(m_flags & IconsOnTabs);
	ui->menuBox->setChecked(m_flags & MenuBar);
	ui->rosterBox->setChecked(m_flags & ShowRoster);
	ui->customIconBox->setChecked(!(m_flags & UseQutimIcon));
	ui->autoresizeBox->setChecked(m_autoresize);
	ui->receiverIdBox->setChecked(m_flags & ShowReceiverId);
	Config chat = cfg.group("chat");
	ui->groupUntil->setValue(chat.value<int>("groupUntil",900));
	ui->fontSizeBox->setValue(chat.value<int>("chatFontSize", qApp->font().pointSize()));
}

void TabbedChatBehavior::saveImpl()
{
	Config appearance = Config("appearance");
	Config widget = appearance.group("chat/behavior/widget");

	bool south = ui->tabPositionBox->itemData(ui->tabPositionBox->currentIndex()).toBool();
	setFlags(TabsOnBottom,south);
	bool adium = ui->formLayoutBox->itemData(ui->formLayoutBox->currentIndex()).toBool();
	setFlags(AdiumToolbar,adium);
	setFlags(IconsOnTabs, ui->stateBox->isChecked());
	setFlags(UseQutimIcon, !ui->customIconBox->isChecked());
	setFlags(MenuBar, ui->menuBox->isChecked());
	setFlags(ShowRoster, ui->rosterBox->isChecked());
	setFlags(ShowReceiverId, ui->receiverIdBox->isChecked());

	widget.setValue("sendKey",m_send_message_key);
	widget.setValue("widgetFlags",m_flags);
	widget.setValue("autoResize",ui->autoresizeBox->isChecked());
	Config history = appearance.group("chat/history");
	history.setValue("storeServiceMessages",ui->storeBox->isChecked());
	history.setValue("maxDisplayMessages",ui->recentBox->value());
	Config chat = appearance.group("chat");
	chat.setValue("groupUntil",ui->groupUntil->value());
	chat.setValue("chatFontSize", ui->fontSizeBox->value());
	appearance.sync();
}

void TabbedChatBehavior::cancelImpl()
{

}

void TabbedChatBehavior::setFlags(AdiumChat::ChatFlags type, bool set)
{
	if (set) {
		m_flags |= type;
	}
	else {
		m_flags &= ~type;
	}
}

void TabbedChatBehavior::onButtonClicked(int id)
{
	m_send_message_key = static_cast<AdiumChat::SendMessageKey>(id);
	setModified(true);
}

}
}


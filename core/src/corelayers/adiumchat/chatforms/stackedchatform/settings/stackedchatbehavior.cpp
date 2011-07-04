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
	connect(ui->storeBox,SIGNAL(clicked(bool)),SLOT(onValueChanged()));
	connect(ui->recentBox,SIGNAL(valueChanged(int)),SLOT(onValueChanged()));
	connect(ui->groupUntil,SIGNAL(valueChanged(int)),SLOT(onValueChanged()));
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
	emit modifiedChanged(true);
}

void StackedChatBehavior::onValueChanged()
{
	emit modifiedChanged(true);
}

}
}

#include "chatbehavior.h"
#include "ui_chatbehavior.h"
#include <QRadioButton>
#include <QButtonGroup>
#include <libqutim/config.h>

namespace Core
{

	ChatBehavior::ChatBehavior() :
			ui(new Ui::ChatBehavior),
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
		connect(ui->storeBox,SIGNAL(clicked(bool)),SIGNAL(modifiedChanged(bool)));
		connect(ui->recentBox,SIGNAL(valueChanged(int)),SLOT(onValueChanged(int)));
		connect(ui->groupUntil,SIGNAL(valueChanged(int)),SLOT(onValueChanged(int)));
	}

	ChatBehavior::~ChatBehavior()
	{
		delete ui;
	}

	void ChatBehavior::changeEvent(QEvent *e)
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

	void ChatBehavior::loadImpl()
	{
		Config appearance = Config("appearance");
		Config widget = appearance.group("chat/behavior/widget");
		m_send_message_key = widget.value("sendKey", AdiumChat::SendCtrlEnter);
		static_cast<QRadioButton *>(m_group->button(m_send_message_key))->setChecked(true);
		Config history = appearance.group("chat/history");
		ui->storeBox->setChecked(history.value<bool>("storeServiceMessages", true));
		ui->recentBox->setValue(history.value<int>("maxDisplayMessages",5));
		Config chat = appearance.group("chat");
		ui->groupUntil->setValue(chat.value<int>("groupUntil",900));
	}

	void ChatBehavior::saveImpl()
	{
		Config appearance = Config("appearance");
		Config widget = appearance.group("chat/behavior/widget");
		widget.setValue("sendKey",m_send_message_key);
		Config history = appearance.group("chat/history");
		history.setValue("storeServiceMessages",ui->storeBox->isChecked());
		history.setValue("maxDisplayMessages",ui->recentBox->value());
		Config chat = appearance.group("chat");
		chat.setValue("groupUntil",ui->groupUntil->value());
		appearance.sync();
	}

	void ChatBehavior::cancelImpl()
	{

	}

	void ChatBehavior::onButtonClicked(int id)
	{
		m_send_message_key = static_cast<AdiumChat::SendMessageKey>(id);
		emit modifiedChanged(true);
	}

	void ChatBehavior::onValueChanged(int)
	{
		emit modifiedChanged(true);
	}
}

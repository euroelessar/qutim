#include "chatbehavior.h"
#include "ui_chatbehavior.h"
#include <QRadioButton>
#include <QButtonGroup>
#include <qutim/config.h>

namespace Core
{
namespace AdiumChat
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

	ui->tabPositionBox->addItem(tr("North"),false);
	ui->tabPositionBox->addItem(tr("South"),true);
	ui->formLayoutBox->addItem(tr("Classic"),false);
	ui->formLayoutBox->addItem(tr("Adium-like"),true);

	connect(m_group,SIGNAL(buttonClicked(int)),SLOT(onButtonClicked(int)));
	connect(ui->storeBox,SIGNAL(clicked(bool)),SIGNAL(modifiedChanged(bool)));
	connect(ui->recentBox,SIGNAL(valueChanged(int)),SLOT(onValueChanged(int)));
	connect(ui->groupUntil,SIGNAL(valueChanged(int)),SLOT(onValueChanged(int)));
	connect(ui->tabPositionBox,SIGNAL(currentIndexChanged(int)),SLOT(onValueChanged(int)));
	connect(ui->formLayoutBox,SIGNAL(currentIndexChanged(int)),SLOT(onValueChanged(int)));
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
	Config cfg = Config("appearance");
	Config widget = cfg.group("chat/behavior/widget");
	m_flags = widget.value("widgetFlags", IconsOnTabs
						   | DeleteSessionOnClose
						   | SwitchDesktopOnRaise
						   | AdiumToolbar
						   | TabsOnBottom
						   );

	ui->tabPositionBox->setCurrentIndex(m_flags & TabsOnBottom ? 1 : 0);
	ui->formLayoutBox->setCurrentIndex(m_flags & AdiumToolbar ? 1 : 0);

	m_send_message_key = widget.value("sendKey", AdiumChat::SendCtrlEnter);
	static_cast<QRadioButton *>(m_group->button(m_send_message_key))->setChecked(true);
	Config history = cfg.group("chat/history");
	ui->storeBox->setChecked(history.value<bool>("storeServiceMessages", true));
	ui->recentBox->setValue(history.value<int>("maxDisplayMessages",5));
	Config chat = cfg.group("chat");
	ui->groupUntil->setValue(chat.value<int>("groupUntil",900));
}

void ChatBehavior::saveImpl()
{
	Config appearance = Config("appearance");
	Config widget = appearance.group("chat/behavior/widget");

	bool south = ui->tabPositionBox->itemData(ui->tabPositionBox->currentIndex()).toBool();
	setFlags(TabsOnBottom,south);
	bool adium = ui->formLayoutBox->itemData(ui->formLayoutBox->currentIndex()).toBool();
	setFlags(AdiumToolbar,adium);

	widget.setValue("sendKey",m_send_message_key);
	widget.setValue("widgetFlags",m_flags);
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

void ChatBehavior::setFlags(AdiumChat::ChatFlags type, bool set)
{
	if (set) {
		m_flags |= type;
	}
	else {
		m_flags &= ~type;
	}
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
}

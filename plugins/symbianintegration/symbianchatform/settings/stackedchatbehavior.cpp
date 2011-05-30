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
namespace Symbian
{

StackedChatBehavior::StackedChatBehavior() :
	ui(new Ui::StackedChatBehavior)
{
	ui->setupUi(this);

	connect(ui->storeBox,SIGNAL(clicked(bool)),SLOT(onValueChanged()));
	connect(ui->recentBox,SIGNAL(valueChanged(int)),SLOT(onValueChanged()));
	connect(ui->groupUntil,SIGNAL(valueChanged(int)),SLOT(onValueChanged()));
	connect(ui->notifyBox,SIGNAL(clicked(bool)),SLOT(onValueChanged()));
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
	Config history = cfg.group("chat/history");
	ui->storeBox->setChecked(history.value<bool>("storeServiceMessages", true));
	ui->recentBox->setValue(history.value<int>("maxDisplayMessages", 5));
	Config chat = cfg.group("chat");
	ui->groupUntil->setValue(chat.value<int>("groupUntil", 900));
	ui->notifyBox->setChecked(chat.value<bool>("notificationsInActiveChat", true));
}

void StackedChatBehavior::saveImpl()
{
	Config appearance = Config("appearance");
	Config history = appearance.group("chat/history");
	history.setValue("storeServiceMessages",ui->storeBox->isChecked());
	history.setValue("maxDisplayMessages",ui->recentBox->value());
	Config chat = appearance.group("chat");
	chat.setValue("groupUntil",ui->groupUntil->value());
	chat.setValue("notificationsInActiveChat", ui->notifyBox->isChecked());
	appearance.sync();
}

void StackedChatBehavior::cancelImpl()
{

}

void StackedChatBehavior::onValueChanged()
{
	emit modifiedChanged(true);
}

} // namespace Symbian
} // namespace AdiumChat
} // namespace Core

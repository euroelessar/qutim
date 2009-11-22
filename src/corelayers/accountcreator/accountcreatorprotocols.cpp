#include "accountcreatorprotocols.h"
#include "libqutim/extensioninfo.h"
#include "libqutim/icon.h"
#include "ui_accountcreatorprotocols.h"

namespace Core
{
	AccountCreatorProtocols::AccountCreatorProtocols(QWidget *parent) :
		QWizardPage(parent),
		m_ui(new Ui::AccountCreatorProtocols)
	{
		m_ui->setupUi(this);
		ProtocolMap protocols = allProtocols();
		QStringList keys = protocols.keys();
		keys.sort();
		foreach (const QString &name, keys) {
			Protocol *proto = protocols.value(name);
			QVariant var = proto ? proto->property("extensioninfo") : QVariant();
			if (!var.isValid())
				continue;
			ExtensionInfo info = var.value<ExtensionInfo>();
			QIcon icon = info.icon();
			if (icon.isNull())
				icon = Icon(QLatin1String("im-") + name.toLower());
			m_ui->protocolsBox->addItem(info.icon(), info.name(), QVariant::fromValue(info));
		}
	}

	AccountCreatorProtocols::~AccountCreatorProtocols()
	{
		delete m_ui;
	}

	void AccountCreatorProtocols::on_protocolsBox_currentIndexChanged(int index)
	{
		ExtensionInfo info = m_ui->protocolsBox->itemData(index).value<ExtensionInfo>();
		m_ui->protocolDescription->setText(info.description());
	}

	void AccountCreatorProtocols::changeEvent(QEvent *e)
	{
		QWizardPage::changeEvent(e);
		switch (e->type()) {
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
		}
	}
}

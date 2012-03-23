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

#include "jaccountregistrationpage.h"
#include "ui_jaccountregistrationpage.h"
#include "modules/xmlconsole/xmlconsole.h"
#include <QTextDocument>
#include <QStringBuilder>

using namespace Jreen;
using namespace qutim_sdk_0_3;

namespace Jabber
{
JAccountRegistrationPage::JAccountRegistrationPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::JAccountRegistrationPage),
    m_client(0), m_manager(0), m_jabberForm(0), m_form(0), m_registered(false)
{
	ui->setupUi(this);
	setFinalPage(false);
	setCommitPage(true);
}

JAccountRegistrationPage::~JAccountRegistrationPage()
{
	delete ui;
}

void JAccountRegistrationPage::cleanupPage()
{
	delete m_client;
	if (m_jabberForm)
		delete m_jabberForm;
	else if (m_form)
		delete m_form;
	m_client = 0;
	m_manager = 0;
	m_jabberForm = 0;
	m_form = 0;
}

void JAccountRegistrationPage::initializePage()
{
	m_client = new Client();
	connect(this, SIGNAL(destroyed()), m_client, SLOT(deleteLater()));
	QString server = field("server").toString();
#ifdef QT_DEBUG
	XmlConsole *console = new XmlConsole();
	connect(this, SIGNAL(destroyed()), console, SLOT(deleteLater()));
	console->init(m_client);
	console->show();
#endif
	m_manager = new RegistrationManager(server, m_client);
	connect(m_manager, SIGNAL(formReceived(Jreen::RegistrationData)),
	        SLOT(onFieldsReceived(Jreen::RegistrationData)));
	connect(m_manager, SIGNAL(error(Jreen::Error::Ptr)),
	        SLOT(onError(Jreen::Error::Ptr)));
	connect(m_manager, SIGNAL(success()),
	        SLOT(onSuccess()));
	m_manager->registerAtServer();
}

bool JAccountRegistrationPage::validatePage()
{
	if (m_registered)
		return true;
	if (m_form)
		m_form->setEnabled(false);
	if (m_jabberForm) {
		RegistrationData data;
		data.setForm(m_jabberForm->getDataForm());
		m_manager->send(data);
		emit completeChanged();
	} else {
		RegistrationData data;
		DataItem root = m_form->item();
		foreach (const DataItem &item, root.subitems()) {
			bool ok = true;
			int index = item.name().toInt(&ok);
			if (!ok)
				continue;
			data.setFieldValue(static_cast<Jreen::RegistrationData::FieldType>(index),
			                   item.data().toString());
		}
		m_manager->send(data);
		emit completeChanged();
	}
	return false;
}

bool JAccountRegistrationPage::isComplete() const
{
	if (m_form)
		return m_form->isEnabled() && m_form->isComplete();
	else
		return false;
}

int JAccountRegistrationPage::nextId() const
{
	return wizard()->currentId() + 1;
}

LocalizedString JAccountRegistrationPage::fieldText(RegistrationData::FieldType type)
{
	switch (type) {
	case RegistrationData::UsernameField:
		return QT_TRANSLATE_NOOP("Jreen", "Username");
	case RegistrationData::NickField:
		return QT_TRANSLATE_NOOP("Jreen", "Familiar name");
	case RegistrationData::PasswordField:
		return QT_TRANSLATE_NOOP("Jreen", "Password");
	case RegistrationData::FullNameField:
		return QT_TRANSLATE_NOOP("Jreen", "Full name");
	case RegistrationData::FirstNameField:
		return QT_TRANSLATE_NOOP("Jreen", "Given name");
	case RegistrationData::LastNameField:
		return QT_TRANSLATE_NOOP("Jreen", "Family name");
	case RegistrationData::EmailField:
		return QT_TRANSLATE_NOOP("Jreen", "Email");
	case RegistrationData::AddressField:
		return QT_TRANSLATE_NOOP("Jreen", "Street");
	case RegistrationData::CityField:
		return QT_TRANSLATE_NOOP("Jreen", "City");
	case RegistrationData::StateField:
		return QT_TRANSLATE_NOOP("Jreen", "Region");
	case RegistrationData::ZipField:
		return QT_TRANSLATE_NOOP("Jreen", "Postal code");
	case RegistrationData::PhoneField:
		return QT_TRANSLATE_NOOP("Jreen", "Phone number");
	case RegistrationData::UrlField:
		return QT_TRANSLATE_NOOP("Jreen", "Web page url");
	case RegistrationData::DateField:
		return QT_TRANSLATE_NOOP("Jreen", "Birth date");
	default:
		return LocalizedString();
	}
}

const char *knownVars[] = {
    "username",
    "nick",
    "name",
    "first",
    "last",
    "email",
    "phone"
};

void JAccountRegistrationPage::onFieldsReceived(const RegistrationData &data)
{
	ui->stackedWidget->setCurrentWidget(ui->formPage);
	if (data.form()) {
		DataForm::Ptr form = data.form();
		if (m_jabberForm) {
			DataForm::Ptr oldForm = m_jabberForm->getDataForm();
			for (uint i = 0; i < sizeof(knownVars) / sizeof(knownVars[0]); ++i) {
				QLatin1String var(knownVars[i]);
				Jreen::DataFormField oldField = oldForm->field(var);
				Jreen::DataFormField field = form->field(var);
				if (field.type() != Jreen::DataFormField::Invalid
						&& oldField.type() != Jreen::DataFormField::Invalid) {
					field.setValues(oldField.values());
				}
			}
			delete m_jabberForm;
		}
		
		m_jabberForm = new JDataForm(data.form(), data.bitsOfBinaries(), ui->formPage);
		m_form = m_jabberForm->widget();
		setSubTitle(data.form()->instructions());
		ui->verticalLayout_3->addWidget(m_jabberForm);
		
	} else {
//		if (data.hasUrl()) {
//			// Notify user about url by which he can register
//			QUrl url = data.url();
//		}
		if (data.hasFields()) {
			DataItem oldRoot;
			if (m_form)
				oldRoot = m_form->item();
			delete m_form;
			DataItem root;
			for (int i = 0; i < Jreen::RegistrationData::LastFieldType; ++i) {
				Jreen::RegistrationData::FieldType type = static_cast<Jreen::RegistrationData::FieldType>(i);
				if (!data.hasField(type))
					continue;
				QString name = QString::number(i);
				DataItem oldItem = root.subitem(name);
				DataItem item(name, fieldText(type), oldItem.data().toString());
				if (type == Jreen::RegistrationData::PasswordField)
					item.setProperty("password", true);
				item.setProperty("mandatory", true);
				root << item;
			}
			m_form = AbstractDataForm::get(root);
			setSubTitle(data.instructions());
			ui->verticalLayout_3->addWidget(m_form);
		}
	} 
	if (m_form) {
		connect(m_form, SIGNAL(completeChanged(bool)), SIGNAL(completeChanged()));
		emit completeChanged();
	}
}

void JAccountRegistrationPage::onError(const Error::Ptr &error)
{
	m_error = error ? error->text() : tr("Unknown error");
	if (m_error.isEmpty()) {
		switch (error->condition()) {
		case Error::Conflict:
			m_error = tr("The desired username is already in use by another user");
			break;
		default:
			break;
		}
	}
	if (!m_error.isEmpty()) {
		m_error = QLatin1Literal("<span style=\"color: red\">")
		          % Qt::escape(m_error).replace(QLatin1String("\n"), QLatin1String("<br>"))
		          % QLatin1Literal("</span>");
	}
	ui->errorLabel->setText(m_error);
	m_manager->fetchFields();
}

void JAccountRegistrationPage::onSuccess()
{
	ui->errorLabel->setText(QString());
	m_registered = true;
	if (m_jabberForm) {
		DataForm::Ptr form = m_jabberForm->getDataForm();
		DataFormField jidField = form->field(QLatin1String("username"));
		QString server = field(QLatin1String("server")).toString();
		JID jid = jidField.value() + QLatin1Char('@') + server;
		setField(QLatin1String("jid"), jid.bare());
		setField(QLatin1String("password"), QString());
		setField(QLatin1String("savePassword"), false);
	} else {
		DataItem item = m_form->item();
		DataItem jidItem = item.subitem(QString::number(Jreen::RegistrationData::UsernameField));
		QString server = field(QLatin1String("server")).toString();
		JID jid = jidItem.data().toString() + QLatin1Char('@') + server;
		setField(QLatin1String("jid"), jid.bare());
		setField(QLatin1String("password"), QString());
		setField(QLatin1String("savePassword"), false);
	}
	emit completeChanged();
	wizard()->next();
}

}

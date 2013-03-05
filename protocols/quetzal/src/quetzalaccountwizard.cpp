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
#include "quetzalaccountwizard.h"
#include "quetzalprotocol.h"
#include "quetzalaccountsettings.h"
#include <qutim/debug.h>
#include <qutim/metaobjectbuilder.h>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>

using namespace qutim_sdk_0_3;

QuetzalProtocol *quetzal_find_protocol(const QMetaObject *meta)
{
	const char *name = MetaObjectBuilder::info(meta, "DependsOn");
	foreach (Protocol *proto, Protocol::all()) {
		if (!qstrcmp(proto->metaObject()->className(), name)) {
			return static_cast<QuetzalProtocol*>(proto);
		}
	}
	return 0;
}

QuetzalAccountWizardPage::QuetzalAccountWizardPage(QuetzalProtocol *proto, QWidget *parent) :
		QWizardPage(parent), m_proto(proto)
{
	QVBoxLayout *scrollAreaLayout = new QVBoxLayout(this);
	QScrollArea *scrollArea = new QScrollArea(this);
	scrollAreaLayout->addWidget(scrollArea);
	scrollArea->setWidgetResizable(true);
	QWidget *scrollAreaWidget = new QWidget(scrollArea);
	scrollArea->setWidget(scrollAreaWidget);
	QVBoxLayout *layout = new QVBoxLayout(scrollAreaWidget);
	scrollAreaWidget->setLayout(layout);

	DataItem general;
	{
		DataItem userName(QLatin1String("username"), tr("Username"), QString());
		userName.setDataChangedHandler(this, SLOT(onDataChanged(QString,QVariant)));
		general.addSubitem(userName);
	}
	PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(proto->plugin());
	GList *it = info->user_splits;
	for (; it; it = it->next) {
		PurpleAccountUserSplit *split = reinterpret_cast<PurpleAccountUserSplit*>(it->data);
		QByteArray label = purple_account_user_split_get_text(split);
		QString text = purple_account_user_split_get_default_value(split);
		DataItem item(QLatin1String("split"), label, text);
		general.addSubitem(item);
	}
	if (!(info->options & OPT_PROTO_NO_PASSWORD)) {
		DataItem password(QLatin1String("password"), tr("Password"), QString());
		if (!(info->options & OPT_PROTO_PASSWORD_OPTIONAL))
			password.setProperty("mandatory", true);
		password.setProperty("password", true);
		general.addSubitem(password);
		DataItem savePassword(QLatin1String("savePassword"), tr("Save password"), false);
		general.addSubitem(savePassword);
	}
	m_generalWidget = AbstractDataForm::get(general);
	m_generalWidget->setParent(this);
	m_settingsWidget = new QuetzalAccountSettings();
	m_settingsWidget->setParent(this);
	m_settingsWidget->setAccount(NULL, proto->plugin());
	layout->addWidget(m_generalWidget);
	layout->addWidget(m_settingsWidget);
	layout->addStretch();
//	connect(m_generalWidget, SIGNAL(completeChanged()), SIGNAL(completeChanged()));
	m_registerButton = 0;
	m_isRegistering = false;
	m_account = 0;
}

bool QuetzalAccountWizardPage::validatePage()
{
	if (!m_generalWidget->isComplete())
		return false;
	if (!m_account || m_generalWidget->isEnabled()) {
		if (!createAccount())
			return false;
	}
	m_proto->addAccount(m_account);
	return true;
}

PurpleAccount *QuetzalAccountWizardPage::createAccount()
{
	QByteArray id;
	QString password;
	bool savePassword = false;
	bool registerUser = false;
	PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(m_proto->plugin());
	GList *it = info->user_splits;
	const QList<DataItem> items = m_generalWidget->item().subitems();
	for (int i = 0; i < items.size(); i++) {
		const DataItem &item = items.at(i);
		if (item.name() == QLatin1String("username")) {
			id = item.data().toString().toUtf8();
		} else if (item.name() == QLatin1String("split")) {
			PurpleAccountUserSplit *split = reinterpret_cast<PurpleAccountUserSplit*>(it->data);
			it = it->next;
			char sep = purple_account_user_split_get_separator(split);
			id += sep;
			id += item.data().toString().toUtf8();
		} else if (item.name() == QLatin1String("password")) {
			password = item.data().toString();
		} else if (item.name() == QLatin1String("savePassword")) {
			savePassword = item.data(false);
		} else if (item.name() == QLatin1String("register")) {
			registerUser = item.data(false);
		}
	}
	if (m_proto->account(id)) {
		purple_notify_error(NULL, NULL, tr("Unable to save new account").toUtf8().constData(),
							tr("An account already exists with the specified criteria.").toUtf8().constData());
		return 0;
	}
	if (!m_account) {
		m_account = purple_account_new(id.constData(), m_proto->plugin()->info->id);
		m_settingsWidget->setAccount(m_account, m_proto->plugin());
	}
	purple_account_set_remember_password(m_account, savePassword);
	purple_account_set_password(m_account, password.toUtf8().constData());
	purple_account_set_username(m_account, id.constData());
	m_settingsWidget->save();
	return m_account;
}

void QuetzalAccountWizardPage::initializePage()
{
	PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(m_proto->plugin());
	if (info->register_user) {
		setButtonText(QWizard::CustomButton1, tr("Register"));
		m_registerButton = wizard()->button(QWizard::CustomButton1);
		m_registerButton->setEnabled(info->options & OPT_PROTO_REGISTER_NOSCREENNAME);
		connect(m_registerButton, SIGNAL(clicked()),
				this, SLOT(onRegisterButtonClicked()));
		wizard()->setOption(QWizard::HaveCustomButton1, true);
	}
	m_settingsWidget->load();
}

void QuetzalAccountWizardPage::cleanupPage()
{
	if (m_registerButton) {
		wizard()->setOption(QWizard::HaveCustomButton1, false);
		disconnect(m_registerButton, 0, this, 0);
		m_registerButton = 0;
	}
	setSubTitle(QString());
}

bool QuetzalAccountWizardPage::isComplete() const
{
	return !m_isRegistering && m_isUsernameFilled;
}

void quetzal_register_callback(PurpleAccount *account, gboolean succeeded, void *user_data)
{
	debug() << Q_FUNC_INFO << bool(succeeded);
	QPointer<QObject> *pointer = reinterpret_cast<QPointer<QObject>*>(user_data);
	QuetzalAccountWizardPage *page = qobject_cast<QuetzalAccountWizardPage*>(pointer->data());
	delete pointer;
	if (page)
		page->handleRegisterResult(account, succeeded);
}

void QuetzalAccountWizardPage::handleRegisterResult(PurpleAccount *account, bool succeeded)
{
	debug() << Q_FUNC_INFO << succeeded;
	Q_ASSERT(m_account == account);
	if (succeeded) {
		m_isRegistering = false;
		setSubTitle(tr("Complete account creation by clicking on Finish button\nComplete creation of account by clicking on Finish button"));
		// FIXME: Check for spacing elements
		m_generalWidget->setData(QLatin1String("username"), QString::fromUtf8(account->username));
	} else {
		wizard()->button(QWizard::CustomButton1)->setEnabled(true);
		wizard()->button(QWizard::BackButton)->setEnabled(true);
		m_generalWidget->setEnabled(true);
		m_settingsWidget->setEnabled(true);
		setSubTitle(tr("Registration failed"));
	}
	completeChanged();
}

void QuetzalAccountWizardPage::onRegisterButtonClicked()
{
	m_isRegistering = true;
	createAccount();
	emit completeChanged();
	purple_account_set_register_callback(m_account, quetzal_register_callback,
										 new QPointer<QObject>(this));
	purple_account_register(m_account);
	wizard()->button(QWizard::BackButton)->setEnabled(false);
	m_generalWidget->setEnabled(false);
	m_settingsWidget->setEnabled(false);
}

void QuetzalAccountWizardPage::onDataChanged(const QString &name, const QVariant &data)
{
	bool oldComplete = isComplete();
	if (name == QLatin1String("username")) {
		m_isUsernameFilled = !data.toString().isEmpty();
		PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(m_proto->plugin());
		if (m_registerButton && !(info->options & OPT_PROTO_REGISTER_NOSCREENNAME))
			m_registerButton->setEnabled(m_isUsernameFilled);
	}
	if (isComplete() != oldComplete)
		emit completeChanged();
}

QuetzalAccountWizard::QuetzalAccountWizard(const QuetzalMetaObject *meta) :
		AccountCreationWizard(m_proto = quetzal_find_protocol(meta))
{
}

QList<QWizardPage *> QuetzalAccountWizard::createPages(QWidget *parent)
{
	QList<QWizardPage*> pages;
	pages << new QuetzalAccountWizardPage(m_proto, parent);
	return pages;
}


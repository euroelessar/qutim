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

#include "quetzalaccountsettings.h"
#include "quetzalaccount.h"
#include "quetzalprotocol.h"
#include <QVBoxLayout>

using namespace qutim_sdk_0_3;

QuetzalAccountSettings::QuetzalAccountSettings()
{
	m_account = 0;
	m_info = 0;
	m_layout = new QVBoxLayout(this);
	m_layout->setMargin(0);
}

QuetzalAccountSettings::~QuetzalAccountSettings()
{
}

QVariant QuetzalAccountSettings::fromOption(PurpleAccountOption *option)
{
	switch (option->type) {
	case PURPLE_PREF_BOOLEAN:
		if (m_account) {
			return bool(purple_account_get_bool(m_account,
												purple_account_option_get_setting(option),
												purple_account_option_get_default_bool(option)));
		} else {
			return bool(purple_account_option_get_default_bool(option));
		}
		break;
	case PURPLE_PREF_INT:
		if (m_account) {
			return purple_account_get_int(m_account,
										  purple_account_option_get_setting(option),
										  purple_account_option_get_default_int(option));
		} else {
			return purple_account_option_get_default_int(option);
		}
		break;
	case PURPLE_PREF_STRING:
		if (m_account) {
			return purple_account_get_string(m_account,
											 purple_account_option_get_setting(option),
											 purple_account_option_get_default_string(option));
		} else {
			return purple_account_option_get_default_string(option);
		}
		break;
	case PURPLE_PREF_STRING_LIST:
		if (m_account) {
			return purple_account_get_string(m_account,
											 purple_account_option_get_setting(option),
											 purple_account_option_get_default_list_value(option));
		} else {
			return purple_account_option_get_default_list_value(option);
		}
		break;
	case PURPLE_PREF_PATH:
	case PURPLE_PREF_PATH_LIST:
	default:
		// I don't know how to handle this
		return QVariant();
	}
}

void QuetzalAccountSettings::loadImpl()
{
	if (!m_info)
		return;
	GList *it = m_info->protocol_options;
	DataItem root;
	for (; it; it = it->next) {
		PurpleAccountOption *option = reinterpret_cast<PurpleAccountOption*>(it->data);
		DataItem item(purple_account_option_get_setting(option),
					  purple_account_option_get_text(option),
					  fromOption(option));
		GList *list;
		QStringList alts;
		QVariantList ids;
		switch (option->type) {
		case PURPLE_PREF_BOOLEAN:
			break;
		case PURPLE_PREF_INT:
			item.setProperty("minValue", G_MININT);
			item.setProperty("maxValue", G_MAXINT);
			break;
		case PURPLE_PREF_STRING:
			item.setProperty("password", bool(purple_account_option_get_masked(option)));
			break;
		case PURPLE_PREF_STRING_LIST:
			list = purple_account_option_get_list(option);
			for (; list; list = list->next) {
				PurpleKeyValuePair *pair = reinterpret_cast<PurpleKeyValuePair*>(list->data);
				alts << reinterpret_cast<char*>(pair->value);
				ids << pair->key;
			}
			break;
		case PURPLE_PREF_PATH:
		case PURPLE_PREF_PATH_LIST:
		default:
			// I don't know how to handle this
			continue;
		}
		if (!alts.isEmpty()) {
			item.setProperty("alternatives", alts);
			item.setProperty("identificators", ids);
		}
		root << item;
	}
	m_form.reset(AbstractDataForm::get(root));
	m_layout->addWidget(m_form.data());
	connect(m_form.data(), SIGNAL(changed()), this, SLOT(onChanged()));
}

void QuetzalAccountSettings::cancelImpl()
{
	if (!m_info)
		return;
	GList *it = m_info->protocol_options;
	for (; it; it = it->next) {
		PurpleAccountOption *option = reinterpret_cast<PurpleAccountOption*>(it->data);
		QVariant data;
		switch (option->type) {
		case PURPLE_PREF_BOOLEAN:
		case PURPLE_PREF_INT:
		case PURPLE_PREF_STRING:
		case PURPLE_PREF_STRING_LIST:
			data = fromOption(option);
			break;
		case PURPLE_PREF_PATH:
		case PURPLE_PREF_PATH_LIST:
		default:
			// I don't know how to handle this
			continue;
		}
		m_form->setData(purple_account_option_get_setting(option), data);
	}
}

void QuetzalAccountSettings::saveImpl()
{
	if (!m_account)
		return;
	const DataItem root = m_form->item();
	GList *it = m_info->protocol_options;
	for (; it; it = it->next) {
		PurpleAccountOption *option = reinterpret_cast<PurpleAccountOption*>(it->data);
		const char *name = purple_account_option_get_setting(option);
		switch (option->type) {
		case PURPLE_PREF_BOOLEAN:
			purple_account_set_bool(m_account, name, root.subitem(name).data().toBool());
			break;
		case PURPLE_PREF_INT:
			purple_account_set_int(m_account, name, root.subitem(name).data().toInt());
			break;
		case PURPLE_PREF_STRING:
			purple_account_set_string(m_account, name, root.subitem(name).data()
									  .toString().toUtf8().constData());
			break;
		case PURPLE_PREF_STRING_LIST:
			purple_account_set_string(m_account, name, root.subitem(name).property("identificator")
									  .toString().toUtf8().constData());
			break;
		case PURPLE_PREF_PATH:
		case PURPLE_PREF_PATH_LIST:
		default:
			// I don't know how to handle this
			continue;
		}
	}
}

void QuetzalAccountSettings::setController(QObject *controller)
{
	QuetzalAccount *account = qobject_cast<QuetzalAccount*>(controller);
	if (account) {
		PurplePlugin *prpl = static_cast<QuetzalProtocol*>(account->protocol())->plugin();
		setAccount(account->purple(), prpl);
	} else {
		m_info = 0;
	}
}

void QuetzalAccountSettings::setAccount(PurpleAccount *account, PurplePlugin *prpl)
{
	m_account = account;
	m_info = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);
}

void QuetzalAccountSettings::onChanged()
{
	setModified(true);
}


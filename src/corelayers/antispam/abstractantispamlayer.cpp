/*
    AbstractAntiSpamLayer

	Copyright (c) 2008-2009 by Rustam Chakin <qutim.develop@gmail.com>
				  2009 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "abstractantispamlayer.h"
#include <QSettings>
#include <QDebug>
#include <QObject>
#include "src/pluginsystem.h"
#include "src/abstractlayer.h"
#include "antispamlayersettings.h"
#include "src/iconmanager.h"

AbstractAntiSpamLayer::AbstractAntiSpamLayer()
{
}

AbstractAntiSpamLayer::~AbstractAntiSpamLayer()
{
}

bool AbstractAntiSpamLayer::init(PluginSystemInterface *plugin_system)
{
	Q_UNUSED(plugin_system);
	return true;
}

void AbstractAntiSpamLayer::saveLayerSettings()
{
	foreach(const SettingsStructure &settings, m_settings)
	{
		AntiSpamLayerSettings *widget = dynamic_cast<AntiSpamLayerSettings *>(settings.settings_widget);
		if(widget)
			widget->saveSettings();
	}
	loadSettings();
}

QList<SettingsStructure> AbstractAntiSpamLayer::getLayerSettingsList()
{
	SettingsStructure settings;
	settings.settings_widget = new AntiSpamLayerSettings(m_profile_name);
	settings.settings_item = new QTreeWidgetItem();
	settings.settings_item->setText(0, QObject::tr("Anti-spam"));
	settings.settings_item->setIcon(0, IconManager::instance().getIcon("antispam"));
	m_settings.append(settings);
	return m_settings;
}

void AbstractAntiSpamLayer::removeLayerSettings()
{
	foreach(const SettingsStructure &settings, m_settings)
	{
		delete settings.settings_widget;
		delete settings.settings_item;
	}
	m_settings.clear();
}

void AbstractAntiSpamLayer::setProfileName(const QString &profile_name)
{
	m_profile_name = profile_name;
	loadSettings();
}

void AbstractAntiSpamLayer::loadSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("antispam");
	m_accept_from_cl_only = settings.value("fromcl", false).toBool();
	m_notify_on_block = settings.value("notify", true).toBool();
	m_block_authorization_from_nil = settings.value("authorization", false).toBool();
	m_block_nil_urls = settings.value("urls", false).toBool();
	m_anti_spam_bot_enabled = settings.value("botenabled", false).toBool();
	m_bot_question = settings.value("question", "").toString();
	m_bot_answer = settings.value("answer", "").toString();
	m_bot_after_answer = settings.value("afteranswer", "").toString();
	m_do_not_answer_if_invisible = settings.value("oninvisible", false).toBool();
	settings.endGroup();
}

bool AbstractAntiSpamLayer::checkForMessageValidation(const TreeModelItem &item,
		const QString &message, MessageType message_type, bool special_status)
{
	if ( m_accept_from_cl_only )
	{
		notifyAboutBlock(item, message);
		return false;
	}
	if ( m_block_authorization_from_nil && message_type == 1 )
	{
		notifyAboutBlock(item, QObject::tr("Authorization blocked"));
		return false;
	}
	if ( m_do_not_answer_if_invisible && special_status )
	{
		notifyAboutBlock(item, message);
		return false;
	}
	if ( m_block_nil_urls && checkForUrls(message) )
	{
		notifyAboutBlock(item, message);
		return false;
	}
	if ( m_anti_spam_bot_enabled )
	{
		if ( message != m_bot_answer )
		{
			QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
			.arg(item.m_account_name).arg(item.m_item_name);
			if ( !m_banned_contacts.contains(identification) )
			{
				notifyAboutBlock(item, message);
				answerToContact(item, m_bot_question);
				m_banned_contacts<<identification;
			}
			return false;
		}
		else
		{
			QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
			.arg(item.m_account_name).arg(item.m_item_name);
			answerToContact(item, m_bot_after_answer);
			m_banned_contacts.removeAll(identification);
			return true;
		}
	}
	
	return true;
}

void AbstractAntiSpamLayer::notifyAboutBlock(const TreeModelItem &item, const QString &message)
{
	if ( m_notify_on_block )
	{
		AbstractLayer::Notification()->userMessage(item, message, NotifyBlockedMessage);
	}
}

void AbstractAntiSpamLayer::answerToContact(const TreeModelItem &item, QString &message)
{
		PluginSystem &ps = PluginSystem::instance();
		ps.sendMessageToContact(item, message, 0);
}

bool AbstractAntiSpamLayer::checkForUrls(const QString &message)
{
	bool containsURLs = false;
	containsURLs = message.contains("http:", Qt::CaseInsensitive) ? true : containsURLs;
	containsURLs = message.contains("ftp:", Qt::CaseInsensitive) ? true : containsURLs;
	containsURLs = message.contains("www.", Qt::CaseInsensitive) ? true : containsURLs;
	return containsURLs;
}

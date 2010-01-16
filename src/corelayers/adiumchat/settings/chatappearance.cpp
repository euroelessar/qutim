/****************************************************************************
 *  chatappearance.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/
#include "chatappearance.h"
#include "ui_chatappearance.h"
#include "../chatstyle.h"
#include "../chatstyleoutput.h"
#include <QDebug>
#include <libqutim/configbase.h>
#include <QWebFrame>
#include <libqutim/protocol.h>
#include <libqutim/account.h>
#include <libqutim/notificationslayer.h>
#include <QDateTime>
#include <libqutim/libqutim_global.h>

namespace Core
{
	class FakeChatUnit : public ChatUnit
	{
	public:
		FakeChatUnit(Account* account) : ChatUnit(account) {}

		virtual QString title() const
		{
			return ChatAppearance::tr("Vasya Pupkin");
		}

		virtual void sendMessage(const qutim_sdk_0_3::Message& message) {}

		virtual QString id() const
		{
			return ChatAppearance::tr("Noname");
		}

		virtual void setChatState(ChatState state) {}
	};
	class FakeAccount : public Account
	{
	public:
		FakeAccount(const QString& id, Protocol* protocol) : Account(id,protocol)
		{
			m_unit = new FakeChatUnit(this);
		}

		virtual ~FakeAccount()
		{
			m_unit->deleteLater();
		}

		virtual ChatUnit* getUnit(const QString& unitId, bool create = true)
		{
			return m_unit;
		}
	private:
		FakeChatUnit *m_unit;
	};

	ChatAppearance::ChatAppearance(): ui(new Ui::chatAppearance),m_page(0),m_chat_session(0)
	{
		ui->setupUi(this);
		if (allProtocols().isEmpty())
			m_chat_session = 0;
		else
		{
			FakeAccount *account = new FakeAccount("Noname",allProtocols().begin().value());
			FakeChatUnit *unit = new FakeChatUnit(account);
			m_chat_session = new ChatSessionImpl(unit,ChatLayer::instance());
			connect(ui->chatBox,SIGNAL(currentIndexChanged(int)),SLOT(onCurrentIndexChanged(int)));
			m_page = m_chat_session->getPage();
			ui->chatPreview->setPage(m_page);
			makePage();
			connect(this, SIGNAL(destroyed()), account, SLOT(deleteLater()));
			connect(this, SIGNAL(destroyed()), m_chat_session, SLOT(deleteLater()));
		}
	}

	ChatAppearance::~ChatAppearance()
	{
		if (m_page)
			m_page->deleteLater();
		delete ui;
		if (m_chat_session)
			m_chat_session->deleteLater();
	}

	void ChatAppearance::cancelImpl()
	{

	}

	void ChatAppearance::loadImpl()
	{
		if (!m_chat_session) {
			Notifications::sendNotification(Notifications::System,this,tr("Unable to create chat session"));
			return;
		}
		ConfigGroup adium_chat = Config("appearance/adiumChat").group("style");
		m_current_style_name = adium_chat.value<QString>("name","default");
		m_current_variant = m_chat_session->getVariant();
		getThemes();
	}

	void ChatAppearance::saveImpl()
	{
		ConfigGroup adium_chat = Config("appearance/adiumChat").group("style");
		adium_chat.setValue("name",m_current_style_name);
		adium_chat.setValue("variant",m_current_variant);
		adium_chat.sync();
	}

	void ChatAppearance::getThemes()
	{
		ui->chatBox->blockSignals(true);
		QString category = "webkitstyle";
		int default_index = -1;
		bool index_found = false;
		QStringList themes = listThemes(category);
		ui->chatBox->clear();
		foreach (QString name, themes)
		{
			//FIXME optimize!
			StyleVariants variants = ChatStyleGenerator::listVariants(getThemePath(category,name).append("/Contents/Resources/Variants"));
			QVariantMap data;
			data["name"] = name;
 			if (variants.isEmpty()) {
				ui->chatBox->addItem(name,data);
				if (!index_found && name == m_current_style_name)
				{
					index_found = true;
					default_index = ui->chatBox->count() - 1;
				}
			}
			else
			{
				StyleVariants::const_iterator it;
				for (it=variants.begin();it!=variants.end();it++) {
					data["variant"] = it.key();
					ui->chatBox->addItem(tr("%1 (%2)").arg(name).arg(it.key()),data);
					if (!index_found && name == m_current_style_name && it.key() == m_current_variant)
					{
						index_found = true;
						default_index = ui->chatBox->count() - 1;
					}
				}
			}
		}
		ui->chatBox->setCurrentIndex(default_index == -1 ? 0 : default_index);
		ui->chatBox->blockSignals(false);
	}

	void ChatAppearance::onCurrentIndexChanged(int index)
	{
		QVariantMap map = ui->chatBox->itemData(index).toMap();
		m_current_variant = map.value("variant").toString();
		if (m_current_style_name == map.value("name").toString()) {
			m_chat_session->setVariant(m_current_variant);
		}
		else {
			m_current_style_name = map.value("name").toString();
			m_chat_session->loadTheme(getThemePath("webkitstyle",m_current_style_name) ,m_current_variant);
			makePage();
		}
		emit modifiedChanged(true);
	}

	void ChatAppearance::makePage()
	{
		if (!m_chat_session) {
			Notifications::sendNotification(Notifications::System,this,tr("Unable to create chat session"));
			return;
		}
		Message message(tr("Preview message"));
		message.setProperty("silent",true);
		message.setProperty("history",true);
		message.setProperty("store",false);
		message.setTime(QDateTime::currentDateTime());
		message.setIncoming(true);
		message.setChatUnit(m_chat_session->getUnit());
		message.setText(tr("Hello!"));
		m_chat_session->appendMessage(message);
		message.setProperty("history",false);
		message.setText(tr("How are you?"));
		m_chat_session->appendMessage(message);
		message.setTime(QDateTime::currentDateTime());
		message.setText(tr("I am fine!"));
		message.setIncoming(false);
		m_chat_session->appendMessage(message);
		message.setText(tr("/me is thinking!"));
		m_chat_session->appendMessage(message);
		message.setProperty("service",true);
		message.setText(tr("Vasya is reading you mind"));
		m_chat_session->appendMessage(message);
	}

}

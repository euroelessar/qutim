/****************************************************************************
 *  chatappearance.cpp
 *
 *  Copyright (c) 2009 by Sidorov Aleksey <sauron@citadelspb.com>
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

namespace AdiumChat
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

	ChatAppearance::ChatAppearance(): ui(new Ui::chatAppearance)
	{
		ui->setupUi(this);
		settingsWidget = 0;
		if (allProtocols().isEmpty())
			m_chat_session = 0;
		else
		{
			FakeAccount *account = new FakeAccount("Noname",allProtocols().begin().value());
			FakeChatUnit *unit = new FakeChatUnit(account);
			m_chat_session = new ChatSessionImpl(unit,ChatLayer::instance());
			m_page = m_chat_session->getPage();
			ui->chatPreview->setPage(m_page);
			//makePage();
		}
	}

	ChatAppearance::~ChatAppearance()
	{
		delete ui;
		if (m_chat_session)
			m_chat_session->deleteLater();
	}

	void ChatAppearance::cancelImpl()
	{

	}

	void ChatAppearance::loadImpl()
	{
		is_load = true;
		ConfigGroup adium_chat = Config("appearance/adiumChat").group("style");
		m_current_style_name = adium_chat.value<QString>("name","default");
		m_current_variant = m_chat_session->getVariant();
		disconnect(ui->chatBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onCurrentIndexChanged(int)));
		int default_index = getThemes();
		connect(ui->chatBox,SIGNAL(currentIndexChanged(int)),SLOT(onCurrentIndexChanged(int)));
		ui->chatBox->setCurrentIndex(default_index == -1 ? 0 : default_index);
	}

	void ChatAppearance::saveImpl()
	{
		ConfigGroup adium_chat = Config("appearance/adiumChat").group("style");
		adium_chat.setValue("name",m_current_style_name);
		adium_chat.setValue("variant",m_current_variant);
		//adium_chat.setValue("variables",m_current_style_variables);
		adium_chat.sync();
	}

	int ChatAppearance::getThemes()
	{
		int default_index = -1;
		QString category = "qutim/webkitstyle";
		bool index_found = false;
		QStringList themes = listThemes(category);
		ui->chatBox->clear();
		foreach (QString name, themes)
		{
			ui->chatBox->addItem(name);
			if (!index_found && name == m_current_style_name)
			{
				index_found = true;
				default_index = ui->chatBox->count() - 1;
			}
		}
		return default_index;
	}

	void ChatAppearance::makeSettings(const QString &theme)
	{
		/*int count = ui->scrollAreaLayout->count();
		while (count > 1)
		{
			QLayoutItem *item = ui->scrollAreaLayout->itemAt(0);
			//ui->scrollAreaLayout->removeItem(item);
			delete item;
			qDebug() << "*******************************" << count;
			count--;
		}*/
		//QLayoutItem *item = ui->scrollAreaLayout->itemAt(0);
			//ui->scrollAreaLayout->removeItem(item);
		if (settingsWidget)
			delete settingsWidget;
		settingsWidget = new QWidget();
		QVBoxLayout *layout = new QVBoxLayout();
		settingsWidget->setLayout(layout);
		QString category = "qutim/webkitstyle";
		StyleVariants variants = ChatStyleGenerator::listVariants(getThemePath(category,theme).append("/Contents/Resources/Variants"));
		if (!variants.isEmpty())
		{
			QHBoxLayout *variantLayout = new QHBoxLayout();
			QLabel *label = new QLabel(tr("Style variant:"));
			QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			label->setSizePolicy(sizePolicy);
			QComboBox *variantBox = new QComboBox();
			variantLayout->addWidget(label);
			variantLayout->addWidget(variantBox);
			layout->addLayout(variantLayout);
			int default_index = -1;
			bool index_found = false;
			StyleVariants::const_iterator it;
			for (it=variants.begin();it!=variants.end();it++)
			{
				variantBox->addItem(it.key());
				if (!index_found && it.key() == m_current_variant)
				{
					index_found = true;
					default_index = variantBox->count() - 1;
				}
			}
			variantBox->setCurrentIndex(default_index == -1 ? 0 : default_index);
			connect(variantBox,SIGNAL(currentIndexChanged(QString)),SLOT(changeVariant(QString)));
		}
		//m_current_style_variables = Config("appearance/adiumChat").group("style");
		foreach (StyleVariable style, m_current_style_variables)
		{
			switch (style.type)
			{
				case COLOR:
					break;
				case FONT:
					break;
				case BACKGROUND:
					break;
			}
		}
		QSpacerItem *space = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
		layout->addItem(space);
		ui->scrollAreaLayout->addWidget(settingsWidget);
	}

	void ChatAppearance::changeVariant(const QString &variant)
	{
		qDebug() << "change variant" << variant;
		m_chat_session->setVariant(m_current_variant);
		emit modifiedChanged(true);
	}

	void ChatAppearance::onCurrentIndexChanged(int index)
	{
		m_current_style_name = ui->chatBox->itemText(index);
		if (!is_load)
			m_current_variant = "";
		is_load = false;
		makeSettings(m_current_style_name);
		m_chat_session->loadTheme(getThemePath("qutim/webkitstyle",m_current_style_name) ,m_current_variant);
		makePage();
		emit modifiedChanged(true);
	}

	void ChatAppearance::makePage()
	{
		if (!m_chat_session)
		{
			Notifications::sendNotification(Notifications::System,this,tr("Unable to create chat session"));
			return;
		}
		Message message(tr("Preview message"));
		message.setProperty("silent",true);
		message.setProperty("history",true);
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
		message.setText("Vasya is reading you mind");
		m_chat_session->appendMessage(message);
	}

}

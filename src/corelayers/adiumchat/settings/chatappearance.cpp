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
		if (!m_chat_session) {
			Notifications::sendNotification(Notifications::System,this,tr("Unable to create chat session"));
			return;
		}		
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
		QString category = "webkitstyle";
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
		if (settingsWidget)
			delete settingsWidget;
		settingsWidget = new QWidget();
		QFormLayout *layout = new QFormLayout();
		settingsWidget->setLayout(layout);
		QString category = "webkitstyle";
		StyleVariants variants = ChatStyleGenerator::listVariants(getThemePath(category,theme).append("/Contents/Resources/Variants"));
		if (!variants.isEmpty())
		{
			QLabel *label = new QLabel(tr("Style variant:"));
			//QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			//label->setSizePolicy(sizePolicy);
			QComboBox *variantBox = new QComboBox();
			layout->addRow(label, variantBox);
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
			connect(variantBox, SIGNAL(currentIndexChanged(QString)), SLOT(changeVariant(QString)));
		}
		m_current_style_variables.clear();
		StyleVariable a, b;
		a.desc = "test test";
		a.label = "Font:";
		a.name = "messageFont";
		a.type = FONT;
		a.value = "bold 12px Times New Roman";
		b.desc = "tost tost";
		b.label = "Animation duration:";
		b.name = "animationDuration";
		b.type = NUMERIC;
		b.value = "1.2";
		m_current_style_variables << a << b;
		//m_current_style_variables = Config("appearance/adiumChat").group("style");
		foreach (StyleVariable style, m_current_style_variables)
		{
			switch (style.type)
			{
				case COLOR:
					break;
				case FONT:
				{
					int sec = 0;
					QString value = style.value.section(" ", sec, sec);
					QFont fvalue;
					QRegExp rxSize("(\\d+)(pt|px)");
					while (!value.isEmpty())
					{
						qDebug() << sec << value;
						if (value == "bold")
							fvalue.setBold(true);
						else if (value == "italic")
							fvalue.setItalic(true);
						else if (value == "small-caps")
							fvalue.setCapitalization(QFont::SmallCaps);
						else if (value.contains(rxSize))
						{
							if (rxSize.cap(2) == "pt")
								fvalue.setPointSize(rxSize.cap(1).toInt());
							else
								fvalue.setPixelSize(rxSize.cap(1).toInt());
							break;
						}
						value = style.value.section(" ", ++sec, sec);
					}
					fvalue.setFamily(style.value.section(" ", ++sec));
					/*QString fvalue(style.value);
					QString fstyle, fvariant, fweight, fsize, fline, ffamily;
					QRegExp rx("(italic)");
					if (fvalue.contains(rx))
					{
						fstyle = rx.cap(1);
						fvalue.remove(rx);
					}
					rx = QRegExp("(small-caps)");
					if (fvalue.contains(rx))
					{
						fvariant = rx.cap(1);
						fvalue.remove(rx);
					}
					rx = QRegExp("^\\s*(normal|bold)");
					if (fvalue.contains(rx))
					{
						fweight = rx.cap(1);
						fvalue.remove(rx);
					}
					rx = QRegExp("^\\s*(\\d+)(pt|em|ex|px|%)");
					if (fvalue.contains(rx))
					{
						fsize = rx.cap(1)+rx.cap(2);
						fvalue.remove(rx);
						rx = QRegExp("^/(\\d+)(pt|em|ex|px|%)");
						if (fvalue.contains(rx))
						{
							fline = rx.cap(1)+rx.cap(2);
						}
					}
					else
						continue;
					rx = QRegExp("^\\s*\w*");
					if (fvalue.contains(rx))
					{
						ffamily = rx.cap(1);
						fvalue.remove(rx);
					}
					else
						continue;
					QFont ffont;
					ffont.setBold(fweight == "normal" ? false : true);
					ffont.setCapitalization(fvariant == "normal" ? false : true);
					ffont.setFamily(ffamily);
					ffont.setItalic(fstyle == "normal" ? false : true);
					*/
					QLabel *label = new QLabel(tr("Font:"));
					//label->setSizePolicy(sizePolicy);
					QLabel *fontLabel = new QLabel();
					fontLabel->setFont(fvalue);
					//QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
					fontLabel->setText(QString("%1").arg(fvalue.pointSize())+" "+fvalue.family());
					layout->addRow(label, fontLabel);
					qDebug() << fontLabel->font().toString();
				}
					break;
				case BACKGROUND:
					break;
				case NUMERIC:
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
		m_chat_session->loadTheme(getThemePath("webkitstyle",m_current_style_name) ,m_current_variant);
		makePage();
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

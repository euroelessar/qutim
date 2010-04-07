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
#include <QLabel>
#include <QSpacerItem>
#include <QFormLayout>
#include <libqutim/protocol.h>
#include <libqutim/account.h>
#include <libqutim/notificationslayer.h>
#include <QDateTime>
#include <libqutim/libqutim_global.h>
#include <QDebug>
#include "chatvariable.h"

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
		settingsWidget = 0;
		if (allProtocols().isEmpty())
			m_chat_session = 0;
		else
		{
			FakeAccount *account = new FakeAccount("Noname", allProtocols().begin().value());
			FakeChatUnit *unit = new FakeChatUnit(account);
			m_chat_session = new ChatSessionImpl(unit, ChatLayer::instance());
			connect(ui->chatBox,SIGNAL(currentIndexChanged(int)),SLOT(onThemeChanged(int)));
			m_page = m_chat_session->getPage();
			ui->chatPreview->setPage(m_page);
			//makePage();
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
		int index = ui->chatBox->findText(m_current_style_name);
		isLoad = true;
		ui->chatBox->setCurrentIndex(index == -1 ? 0 : index);
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
		QStringList themes = listThemes(category);
		ui->chatBox->clear();
		foreach (QString name, themes)
			ui->chatBox->addItem(name);
		ui->chatBox->blockSignals(false);
	}

	void ChatAppearance::onThemeChanged(int index)
	{
		m_current_style_name = ui->chatBox->itemText(index);
		m_chat_session->loadTheme(getThemePath("webkitstyle",m_current_style_name), QString());
		makePage();
		makeSettings();
		if (!isLoad)
			emit modifiedChanged(true);
		isLoad = false;
	}

	void ChatAppearance::onVariantChanged(const QString variant)
	{
		m_current_variant = variant;
		m_chat_session->setVariant(m_current_variant);
		emit modifiedChanged(true);
	}

	void ChatAppearance::onVariableChanged()
	{
		QString css;
		foreach (ChatVariable *widget, m_current_variables)
			if (widget)
				css.append(QString("%1 { %2: %3; }")
						.arg(widget->style().selectors)
						.arg(widget->style().parameter)
						.arg(widget->style().value));
		//m_chat_session->setCustomStyle(css);
		qDebug() << css;
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

	void ChatAppearance::makeSettings() {
		if (settingsWidget)
			delete settingsWidget;
		settingsWidget = new QWidget();
		QFormLayout *layout = new QFormLayout();
		layout->setLabelAlignment(Qt::AlignLeft|Qt::AlignVCenter);
		QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
		settingsWidget->setLayout(layout);
		QString category = "webkitstyle";
		StyleVariants variants = ChatStyleGenerator::listVariants(getThemePath(category, m_current_style_name)
				.append("/Contents/Resources/Variants"));
		if (!variants.isEmpty()) {
			QLabel *label = new QLabel(tr("Style variant:"));
			label->setSizePolicy(sizePolicy);
			QComboBox *variantBox = new QComboBox();
			layout->addRow(label, variantBox);
			StyleVariants::const_iterator it;
			for (it=variants.begin(); it!=variants.end(); it++)
				variantBox->addItem(it.key());
			connect(variantBox, SIGNAL(currentIndexChanged(QString)), SLOT(onVariantChanged(QString)));
			int index = isLoad ? variantBox->findText(m_current_variant) : 0;
			m_current_variant = variantBox->itemText(index);
			variantBox->setCurrentIndex(index);
			onVariantChanged(m_current_variant);
		}
		Config achat(QStringList() << "appearance/adiumChat" << getThemePath(category,m_current_style_name)
				.append("/Contents/Resources/custom.json"));
		qDebug() << getThemePath(category,m_current_style_name).append("/Contents/Resources/custom.json");
		ConfigGroup variables = achat.group(m_current_style_name);
		int count = variables.arraySize();
		qDebug() << m_current_style_name << achat.groupList();
		try {
			qDebug() << achat.group("default").groupList();
		} catch (int e) {
			qDebug() << e;
		}

		for (int num = 0; num < count; num++) {
			ConfigGroup parameter = variables.at(num);
			QString type = parameter.value("type", QString());
			CustomChatStyle style;
			style.parameter = parameter.value("parameter", QString());
			style.selectors = parameter.value("selectors", QString());
			style.value = parameter.value("value", QString());
			if (type == "font") {
				QLabel *label = new QLabel(parameter.value("label", QString()));
				label->setSizePolicy(sizePolicy);
				ChatFont *fontField = new ChatFont(style);
				layout->addRow(label, fontField);
				connect(fontField, SIGNAL(changeValue()), SLOT(onVariableChanged()));
				if (ChatVariable *widget = qobject_cast<ChatVariable*>(fontField))
					m_current_variables.append(widget);
			} else if (type == "color") {
				QLabel *label = new QLabel(parameter.value("label", QString()));
				label->setSizePolicy(sizePolicy);
				ChatColor *colorField = new ChatColor(style);
				layout->addRow(label, colorField);
				connect(colorField, SIGNAL(changeValue()), SLOT(onVariableChanged()));
				if (ChatVariable *widget = qobject_cast<ChatVariable*>(colorField))
					m_current_variables.append(widget);
			} else if (type == "numeric") {
				QLabel *label = new QLabel(parameter.value("label", QString()));
				label->setSizePolicy(sizePolicy);
				double min = parameter.value<double>("min", 0);
				double max = parameter.value<double>("min", 0);
				double step = parameter.value<double>("min", 1);
				ChatNumeric *numField = new ChatNumeric(style, min, max, step);
				layout->addRow(label, numField);
				connect(numField, SIGNAL(changeValue()), SLOT(changeVariable()));
				if (ChatVariable *widget = qobject_cast<ChatVariable*>(numField))
					m_current_variables.append(widget);
			}
		}
		/*m_current_style_variables.clear();
		StyleVariables style_variables;
		StyleVariable a, b, c;
		a.desc = "test test";
		a.label = "Font:";
		a.name = "messageFont";
		a.type = FONT;
		a.value = "italic 12pt Times New Roman";
		b.desc = "tost tost";
		b.label = "Animation duration:";
		b.name = "animationDuration";
		b.type = NUMERIC;
		b.value = "1.2";
		b.range = "0;5;0.1";
		c.desc = "ti ti";
		c.label = "Animation color:";
		c.name = "animationColor";
		c.type = COLOR;
		c.value = "#A0FFFF";
		style_variables << a << b << c;
		//m_current_style_variables = Config("appearance/adiumChat").group("style");
		foreach (StyleVariable style, style_variables)
			{
				switch (style.type)
				{
					case COLOR:
						{
							QLabel *label = new QLabel(style.label);
							label->setSizePolicy(sizePolicy);
							ChatColor *colorField = new ChatColor(style);
							layout->addRow(label, colorField);
							connect(colorField, SIGNAL(changeValue()), SLOT(changeVariable()));
							ChatVariable *widget = qobject_cast<ChatVariable*>(colorField);
							if (widget)
								m_current_style_variables.append(widget);
						}
						break;
					case FONT:
						{
							QLabel *label = new QLabel(style.label);
							label->setSizePolicy(sizePolicy);
							ChatFont *fontField = new ChatFont(style);
							layout->addRow(label, fontField);
							connect(fontField, SIGNAL(changeValue()), SLOT(changeVariable()));
							ChatVariable *widget = qobject_cast<ChatVariable*>(fontField);
							if (widget)
								m_current_style_variables.append(widget);
							//m_current_style_variables << qobject_cast<QWidget*>(fontField);
						}
						break;
					case BACKGROUND:
						break;
					case NUMERIC:
						{
							QLabel *label = new QLabel(style.label);
							label->setSizePolicy(sizePolicy);
							ChatNumeric *numField = new ChatNumeric(style);
							layout->addRow(label, numField);
							connect(numField, SIGNAL(changeValue()), SLOT(changeVariable()));
							ChatVariable *widget = qobject_cast<ChatVariable*>(numField);
							if (widget)
								m_current_style_variables.append(widget);
							//m_current_style_variables << qobject_cast<QWidget>(numField);
						}
						break;
				}
			}*/
			QSpacerItem *space = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
			layout->addItem(space);
			ui->scrollAreaLayout->addWidget(settingsWidget);
		}




}

/*StyleVariants variants = ChatStyleGenerator::listVariants(getThemePath(category,name)
					.append("/Contents/Resources/Variants"));
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
			}*/


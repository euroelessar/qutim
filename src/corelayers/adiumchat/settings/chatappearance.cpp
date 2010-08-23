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
#include "chatvariable.h"
#include "../chatlayer/chatstyle.h"
#include "../chatlayer/chatstyleoutput.h"
#include <libqutim/configbase.h>
#include <libqutim/protocol.h>
#include <libqutim/account.h>
#include <libqutim/notificationslayer.h>
#include <libqutim/libqutim_global.h>
#include <QWebFrame>
#include <QLabel>
#include <QSpacerItem>
#include <QFormLayout>
#include <QDateTime>
#include <QStringBuilder>
#include <QDebug>

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

		virtual bool sendMessage(const qutim_sdk_0_3::Message& message)
		{
			Q_UNUSED(message);
			return true;
		}

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

		virtual ChatUnit* getUnit(const QString &unitId, bool create = true)
		{
			Q_UNUSED(unitId);
			Q_UNUSED(create);
			return m_unit;
		}

		virtual FakeChatUnit *getFakeUnit()
		{
			return m_unit;
		}

	private:
		FakeChatUnit *m_unit;
	};

	ChatAppearance::ChatAppearance(): ui(new Ui::chatAppearance),m_page(0),m_chat_session(0)
	{
		ui->setupUi(this);
		QList<int> sizes;
		sizes << 150 << 50;
		ui->splitter->setSizes(sizes);
		settingsWidget = 0;
		if (allProtocols().isEmpty())
			m_chat_session = 0;
		else
		{
			FakeAccount *account = new FakeAccount("Noname", allProtocols().begin().value());
			FakeChatUnit *unit = account->getFakeUnit();
			m_chat_session = new ChatSessionImpl(unit, ChatLayer::instance());
			connect(ui->chatBox,SIGNAL(currentIndexChanged(int)),SLOT(onThemeChanged(int)));
			m_page = m_chat_session->getPage();
			ui->chatPreview->setPage(m_page);
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
			Notifications::send(Notifications::System,this,tr("Unable to create chat session"));
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
		Config config("appearance/adiumChat");
		config.beginGroup("style");
		config.setValue("name",m_current_style_name);
		config.setValue("variant",m_current_variant);
		config.endGroup();
		int oldSize = config.beginArray(m_current_style_name);
		for (int i = 0; i < m_current_variables.size(); i++) {
			config.setArrayIndex(i);
			config.setValue("value", m_current_variables.at(i)->chatStyle().value);
		}
		for (int size = m_current_variables.size(); size < oldSize; size++)
			config.remove(size);
		config.endArray();
		config.sync();
	}

	void ChatAppearance::getThemes()
	{
		ui->chatBox->blockSignals(true);
		QString category = "webkitstyle";
		QStringList themes = listThemes(category);
		ui->chatBox->clear();
		foreach (const QString &name, themes)
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
				css.append(QString("%1 { %2: %3; } ")
						.arg(widget->chatStyle().selector)
						.arg(widget->chatStyle().parameter)
						.arg(widget->chatStyle().value));
		m_chat_session->setCustomCSS(css);
		emit modifiedChanged(true);
	}

	void ChatAppearance::makePage()
	{
		if (!m_chat_session) {
			Notifications::send(Notifications::System,this,tr("Unable to create chat session"));
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
		message.setText(tr("Vasya Pupkin is reading you mind"));
		m_chat_session->appendMessage(message);
	}

	void ChatAppearance::makeSettings() {
		m_current_variables.clear();
		if (settingsWidget)
			delete settingsWidget;
		settingsWidget = new QWidget(this);
		QFormLayout *layout = new QFormLayout(settingsWidget);
		layout->setLabelAlignment(Qt::AlignLeft|Qt::AlignVCenter);
		QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
		settingsWidget->setLayout(layout);
		QString category = "webkitstyle";
		StyleVariants variants = ChatStyleGenerator::listVariants(getThemePath(category, m_current_style_name)
				.append("/Contents/Resources/Variants"));
		if (!variants.isEmpty()) {
			QLabel *label = new QLabel(tr("Style variant:"), settingsWidget);
			label->setSizePolicy(sizePolicy);
			QComboBox *variantBox = new QComboBox(settingsWidget);
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
		Config achat(getThemePath(category,m_current_style_name).append("/Contents/Resources/custom.json"));
//		QStringList() << "appearance/adiumChat" << getThemePath(category,m_current_style_name)
//				.append("/Contents/Resources/custom.json"));
		ConfigGroup variables = achat;
		int count = variables.beginArray(m_current_style_name);
		for (int num = 0; num < count; num++) {
			ConfigGroup parameter = variables.arrayElement(num);
			QString type = parameter.value("type", QString());
			QString text = parameter.value("label", QString());
			text = parameter.value(QString("label-").append(QLocale().name()), text);
			CustomChatStyle style;
			style.parameter = parameter.value("parameter", QString());
			style.selector = parameter.value("selector", QString());
			style.value = parameter.value("value", QString());
			if (type == "font") {
				QLabel *label = new QLabel(text % ":", settingsWidget);
				label->setSizePolicy(sizePolicy);
				ChatFont *fontField = new ChatFont(style, settingsWidget);
				layout->addRow(label, fontField);
				connect(fontField, SIGNAL(changeValue()), SLOT(onVariableChanged()));
				if (ChatVariable *widget = qobject_cast<ChatVariable*>(fontField))
					m_current_variables.append(widget);
			} else if (type == "color") {
				QLabel *label = new QLabel(text % ":", settingsWidget);
				label->setSizePolicy(sizePolicy);
				ChatColor *colorField = new ChatColor(style, settingsWidget);
				layout->addRow(label, colorField);
				connect(colorField, SIGNAL(changeValue()), SLOT(onVariableChanged()));
				if (ChatVariable *widget = qobject_cast<ChatVariable*>(colorField))
					m_current_variables.append(widget);
			} else if (type == "numeric") {
				QLabel *label = new QLabel(text % ":", settingsWidget);
				label->setSizePolicy(sizePolicy);
				double min = parameter.value<double>("min", 0);
				double max = parameter.value<double>("max", 0);
				double step = parameter.value<double>("step", 1);
				ChatNumeric *numField = new ChatNumeric(style, min, max, step, settingsWidget);
				layout->addRow(label, numField);
				connect(numField, SIGNAL(changeValue()), SLOT(onVariableChanged()));
				if (ChatVariable *widget = qobject_cast<ChatVariable*>(numField))
					m_current_variables.append(widget);
			} else if (type == "boolean") {
				QString trueValue = parameter.value("true", QString());
				QString falseValue = parameter.value("false", QString());
				ChatBoolean *boolField = new ChatBoolean(style, trueValue, falseValue, settingsWidget);
				boolField->setText(text);
				layout->addRow(boolField);
				connect(boolField, SIGNAL(changeValue()), SLOT(onVariableChanged()));
				if (ChatVariable *widget = qobject_cast<ChatVariable*>(boolField))
					m_current_variables.append(widget);
			}
		}
		onVariableChanged();
		QSpacerItem *space = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
		layout->addItem(space);
		ui->scrollAreaLayout->addWidget(settingsWidget);
	}




}

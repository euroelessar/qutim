/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
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
#include "adiumwebviewappearance.h"
#include "ui_adiumwebviewappearance.h"
#include "adiumwebviewvariable.h"
#include "adiumwebviewcontroller.h"
#include <qutim/configbase.h>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/notification.h>
#include <qutim/thememanager.h>
#include <qutim/adiumchat/chatsessionimpl.h>
#include <QWebFrame>
#include <QLabel>
#include <QSpacerItem>
#include <QFormLayout>
#include <QDateTime>
#include <QStringBuilder>
#include <QDebug>

namespace Adium
{

using namespace qutim_sdk_0_3;

WebViewAppearance::WebViewAppearance(): ui(new Ui::chatAppearance), m_controller(0)
{
	ui->setupUi(this);
	QList<int> sizes;
	sizes << 150 << 50;
	ui->splitter->setSizes(sizes);
	m_settingsWidget = 0;
	QDir shareDir = ThemeManager::path(QLatin1String("data"), QLatin1String("webview"));
	Q_ASSERT(shareDir.exists(QLatin1String("Preview.plist")));
	m_preview = WebKitPreviewLoader().loadPreview(shareDir.filePath(QLatin1String("Preview.plist")));
	m_controller = new WebViewController(true);
	m_style = m_controller->style();
	m_controller->setChatSession(static_cast<Core::AdiumChat::ChatSessionImpl*>(m_preview->session.data()));
	ui->chatPreview->setPage(m_controller);
	connect(ui->chatBox, SIGNAL(currentIndexChanged(int)), SLOT(onThemeChanged(int)));
}

WebViewAppearance::~WebViewAppearance()
{
	delete m_controller;
}

void WebViewAppearance::cancelImpl()
{
	loadImpl();
}

void WebViewAppearance::loadImpl()
{
	Config config("appearance/adiumChat");
	config.beginGroup("style");
	m_styleName = config.value(QLatin1String("name"), QLatin1String("default"));
	m_style->setStylePath(ThemeManager::path(QLatin1String("webkitstyle"), m_styleName));
	QString variant = config.value(QLatin1String("variant"), m_style->defaultVariant());
	m_style->setActiveVariant(variant);
//	qDebug() << m_style->baseTemplateForChat(m_preview->session.data());
	
	fillThemesComboBox();
	
	int index = qMax(0, ui->chatBox->findText(m_styleName));
	m_isLoad = true;

	if (index == ui->chatBox->currentIndex())
		onThemeChanged(index);
	else
		ui->chatBox->setCurrentIndex(index);
}

void WebViewAppearance::saveImpl()
{
	Config config("appearance/adiumChat");
	config.beginGroup("style");
	config.setValue(QLatin1String("name"), m_styleName);
	config.setValue(QLatin1String("variant"), m_style->activeVariant());
	config.endGroup();
	int oldSize = config.beginArray(m_styleName);
	for (int i = 0; i < m_variables.size(); i++) {
		config.setArrayIndex(i);
		config.setValue("value", m_variables.at(i)->chatStyle().value);
	}
	for (int size = m_variables.size(); size < oldSize; size++)
		config.remove(size);
	config.endArray();
}

void WebViewAppearance::fillThemesComboBox()
{
	ui->chatBox->blockSignals(true);
	ui->chatBox->clear();
	foreach (const QString &name, ThemeManager::list(QLatin1String("webkitstyle")))
		ui->chatBox->addItem(name);
	ui->chatBox->blockSignals(false);
}

void WebViewAppearance::onThemeChanged(int index)
{
	m_styleName = ui->chatBox->itemText(index);
	m_style->setStylePath(ThemeManager::path(QLatin1String("webkitstyle"), m_styleName));
	m_controller->clearChat();
	foreach (const Message &message, m_preview->messages)
		m_controller->appendMessage(message);
//	makePage();
	makeSettings();
	if (!m_isLoad)
		emit modifiedChanged(true);
	m_isLoad = false;
}

void WebViewAppearance::onVariantChanged(const QString variant)
{
	m_style->setActiveVariant(variant);
	m_controller->evaluateJavaScript(m_style->scriptForChangingVariant());
	emit modifiedChanged(true);
}

void WebViewAppearance::onVariableChanged()
{
	QString css;
	foreach (WebViewVariable *widget, m_variables) {
		if (widget) {
			css.append(QString("%1 { %2: %3; } ")
					   .arg(widget->chatStyle().selector)
					   .arg(widget->chatStyle().parameter)
					   .arg(widget->chatStyle().value));
		}
	}
//	m_controller->setCustomCSS(css);
	emit modifiedChanged(true);
}

void WebViewAppearance::makePage()
{
//	if (!m_controller) {
//		NotificationRequest request(Notification::System);
//		request.setObject(this);
//		request.setText(tr("Unable to create chat session"));
//		request.send();
//		return;
//	}
//	Message message(tr("Preview message"));
//	message.setProperty("silent",true);
//	message.setProperty("history",true);
//	message.setProperty("store",false);
//	message.setTime(QDateTime::currentDateTime());
//	message.setIncoming(true);
//	message.setChatUnit(m_controller->getSession()->getUnit());
//	message.setText(tr("Hello!"));
//	m_controller->appendMessage(message);
//	message.setProperty("history",false);
//	message.setText(tr("How are you?"));
//	m_controller->appendMessage(message);
//	message.setTime(QDateTime::currentDateTime());
//	message.setText(tr("I am fine!"));
//	message.setIncoming(false);
//	m_controller->appendMessage(message);
//	message.setText(tr("/me is thinking!"));
//	m_controller->appendMessage(message);
//	message.setProperty("service",true);
//	message.setText(tr("Vasya Pupkin is reading you mind"));
//	m_controller->appendMessage(message);
}

void WebViewAppearance::makeSettings() {
	m_variables.clear();
	if (m_settingsWidget)
		delete m_settingsWidget;
	m_settingsWidget = new QWidget(this);
	QFormLayout *layout = new QFormLayout(m_settingsWidget);
	layout->setLabelAlignment(Qt::AlignLeft|Qt::AlignVCenter);
	QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	m_settingsWidget->setLayout(layout);
	QString category = "webkitstyle";
	QStringList variants = m_style->variants();
	
	if (!variants.isEmpty()) {
		QLabel *label = new QLabel(tr("Style variant:"), m_settingsWidget);
		label->setSizePolicy(sizePolicy);
		QComboBox *variantBox = new QComboBox(m_settingsWidget);
		layout->addRow(label, variantBox);
		variantBox->addItems(variants);
		connect(variantBox, SIGNAL(currentIndexChanged(QString)), SLOT(onVariantChanged(QString)));
		int index = m_isLoad ? variantBox->findText(m_style->activeVariant()) : 0;
		m_style->setActiveVariant(variantBox->itemText(index));
		variantBox->setCurrentIndex(index);
		onVariantChanged(m_style->activeVariant());
	}
	Config achat(QStringList()
				 << "appearance/adiumChat"
				 << ThemeManager::path(category,m_styleName)
				 .append("/Contents/Resources/custom.json"));
	ConfigGroup variables = achat;
	int count = variables.beginArray(m_styleName);
	for (int num = 0; num < count; num++) {
		ConfigGroup parameter = variables.arrayElement(num);
		QString type = parameter.value("type", QString());
		QString text = parameter.value("label", QString());
		text = parameter.value(QString("label-").append(QLocale().name()), text);
		WebViewCustomStyle style;
		style.parameter = parameter.value("parameter", QString());
		style.selector = parameter.value("selector", QString());
		style.value = parameter.value("value", QString());
		if (type == "font") {
			QLabel *label = new QLabel(text % ":", m_settingsWidget);
			label->setSizePolicy(sizePolicy);
			WebViewFont *fontField = new WebViewFont(style, m_settingsWidget);
			layout->addRow(label, fontField);
			connect(fontField, SIGNAL(changeValue()), SLOT(onVariableChanged()));
			if (WebViewVariable *widget = qobject_cast<WebViewVariable*>(fontField))
				m_variables.append(widget);
		} else if (type == "color") {
			QLabel *label = new QLabel(text % ":", m_settingsWidget);
			label->setSizePolicy(sizePolicy);
			WebViewColor *colorField = new WebViewColor(style, m_settingsWidget);
			layout->addRow(label, colorField);
			connect(colorField, SIGNAL(changeValue()), SLOT(onVariableChanged()));
			if (WebViewVariable *widget = qobject_cast<WebViewVariable*>(colorField))
				m_variables.append(widget);
		} else if (type == "numeric") {
			QLabel *label = new QLabel(text % ":", m_settingsWidget);
			label->setSizePolicy(sizePolicy);
			double min = parameter.value<double>("min", 0);
			double max = parameter.value<double>("max", 0);
			double step = parameter.value<double>("step", 1);
			WebViewNumeric *numField = new WebViewNumeric(style, min, max, step, m_settingsWidget);
			layout->addRow(label, numField);
			connect(numField, SIGNAL(changeValue()), SLOT(onVariableChanged()));
			if (WebViewVariable *widget = qobject_cast<WebViewVariable*>(numField))
				m_variables.append(widget);
		} else if (type == "boolean") {
			QString trueValue = parameter.value("true", QString());
			QString falseValue = parameter.value("false", QString());
			WebViewBoolean *boolField = new WebViewBoolean(style, trueValue, falseValue, m_settingsWidget);
			boolField->setText(text);
			layout->addRow(boolField);
			connect(boolField, SIGNAL(changeValue()), SLOT(onVariableChanged()));
			if (WebViewVariable *widget = qobject_cast<WebViewVariable*>(boolField))
				m_variables.append(widget);
		}
	}
	onVariableChanged();
	QSpacerItem *space = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
	layout->addItem(space);
	ui->scrollAreaLayout->addWidget(m_settingsWidget);
}

}

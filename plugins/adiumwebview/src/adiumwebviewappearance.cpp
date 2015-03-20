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
#include "adiumwebviewappearance.h"
#include "ui_adiumwebviewappearance.h"
#include "adiumwebviewvariable.h"
#include "adiumwebviewcontroller.h"
#include <qutim/configbase.h>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/notification.h>
#include <qutim/thememanager.h>
#include <QWebFrame>
#include <QLabel>
#include <QSpacerItem>
#include <QFormLayout>
#include <QFileDialog>
#include <QDateTime>
#include <QStringBuilder>
#include <QFontInfo>
#include <QDebug>

namespace Adium
{

using namespace qutim_sdk_0_3;

class SignalBlocker
{
public:
	SignalBlocker(QObject *object) : m_object(object), m_blockValue(object->blockSignals(true)) {}
	~SignalBlocker() { m_object->blockSignals(m_blockValue); }
	
private:
	QObject *m_object;
	bool m_blockValue;
};

WebViewAppearance::WebViewAppearance(): ui(new Ui::chatAppearance), m_isFillingState(false)
{
	ui->setupUi(this);
	QDir shareDir = ThemeManager::path(QLatin1String("data"), QLatin1String("webview"));
	Q_ASSERT(shareDir.exists(QLatin1String("Preview.plist")));
	m_preview = WebKitPreviewLoader().loadPreview(shareDir.filePath(QLatin1String("Preview.plist")));
	m_controller = new WebViewController(true);
	m_controller->setChatSession(m_preview->session.data());
    ui->chatPreview->setPage(m_controller);
	m_style = m_controller->style();
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
	SignalBlocker styleBlocker(ui->styleNameBox);
	fillStylesComboBox();
	
	Config config("appearance/adiumChat");
	config.beginGroup("style");
	ui->showUserIconBox->setChecked(config.value(QLatin1String("showUserIcons"), true));
	m_style->setShowUserIcons(ui->showUserIconBox->isChecked());
	ui->showHeaderBox->setChecked(config.value(QLatin1String("showHeader"), true));
	m_style->setShowHeader(ui->showHeaderBox->isChecked());
	QString styleName = config.value(QLatin1String("name"), QLatin1String("default"));
	int index = qMax(0, ui->styleNameBox->findText(styleName));
	ui->styleNameBox->setCurrentIndex(index);
	on_styleNameBox_currentIndexChanged(ui->styleNameBox->itemText(index));
}

void WebViewAppearance::saveImpl()
{
	Config config(QLatin1String("appearance/adiumChat"));
	
	config.beginGroup(QLatin1String("style"));
	config.setValue(QLatin1String("showUserIcons"), ui->showUserIconBox->isChecked());
	config.setValue(QLatin1String("showHeader"), ui->showHeaderBox->isChecked());
	config.setValue(QLatin1String("name"), m_styleName);
	config.beginGroup(m_styleName);
	config.setValue(QLatin1String("variant"), m_style->activeVariant());
	config.setValue(QLatin1String("customBackground"), ui->customBackgroundBox->isChecked());
	config.setValue(QLatin1String("backgroundType"), m_style->customBackgroundType());
	config.setValue(QLatin1String("backgroundPath"), backgroundPath());
	config.setValue(QLatin1String("backgroundColor"), backgroundColor());
	config.setValue(QLatin1String("fontFamily"), m_controller->defaultFontFamily());
	config.setValue(QLatin1String("fontSize"), m_controller->defaultFontSize());
	QVariantList variables;
	for (int i = 0; i < m_variables.size(); i++)
		variables << m_variables.at(i)->chatStyle().value;
	config.setValue(QLatin1String("customStyle"), variables);
}

void WebViewAppearance::fillStylesComboBox()
{
	ui->styleNameBox->clear();
	foreach (const QString &name, ThemeManager::list(QLatin1String("webkitstyle")))
		ui->styleNameBox->addItem(name);
}

void WebViewAppearance::rebuildChatView()
{
	m_controller->clearChat();
	foreach (const Message &message, m_preview->messages)
		m_controller->appendMessage(message);
}

void WebViewAppearance::updateCustomScript()
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
	m_style->setCustomStyle(css);
}

void WebViewAppearance::setBackgroundPath(const QString &path)
{
	ui->imageButton->setIcon(QIcon(path));
	ui->imageButton->setProperty("_webview_path", path);
}

QString WebViewAppearance::backgroundPath() const
{
	return ui->imageButton->property("_webview_path").toString();
}

void WebViewAppearance::setBackgroundColor(const QColor &color)
{
	ui->colorButton->setStyleSheet(QLatin1String("background: ") + color.name());
	ui->colorButton->setProperty("_webview_color", qVariantFromValue(color));
}

QColor WebViewAppearance::backgroundColor() const
{
	return ui->colorButton->property("_webview_color").value<QColor>();
}

void WebViewAppearance::setDefaultFont(const QString &family, int size)
{
	m_controller->setDefaultFont(family, size);
	ui->fontLabel->setFont(QFont(m_controller->defaultFontFamily(), m_controller->defaultFontSize()));
	ui->fontLabel->setText(QString("%1 %2pt").arg(m_controller->defaultFontFamily(),
	                                              QString::number(m_controller->defaultFontSize())));
}

void WebViewAppearance::onVariableChanged()
{
	updateCustomScript();
	m_controller->evaluateJavaScript(m_style->scriptForSettingCustomStyle());
	setModified(true);
}

void WebViewAppearance::makeSettings()
{
	onVariableChanged();
}

void WebViewAppearance::on_styleNameBox_currentIndexChanged(const QString &styleName)
{
	// We don't want to receive some of the events
	SignalBlocker variantBlocker(ui->variantBox);
	SignalBlocker backgroundTypeBlocker(ui->backgroundTypeBox);
	
	if (!m_styleName.isEmpty())
		setModified(true);
	m_styleName = styleName;
	m_style->setStylePath(ThemeManager::path(QLatin1String("webkitstyle"), m_styleName));
	
	ui->customBackgroundBox->setEnabled(m_style->allowsCustomBackground());
	ui->showUserIconBox->setEnabled(m_style->allowsUserIcons());
	ui->showHeaderBox->setEnabled(m_style->hasHeader() || m_style->hasTopic());
	
	qDeleteAll(m_variables);
	m_variables.clear();
	
	Config config(QLatin1String("appearance/adiumChat"));
	config.beginGroup(QLatin1String("style"));
	config.beginGroup(m_styleName);
	m_style->setCustomBackgroundType(config.value(QLatin1String("backgroundType"),
	                                              WebKitMessageViewStyle::BackgroundNormal));
	ui->backgroundTypeBox->setCurrentIndex(m_style->customBackgroundType());
	ui->customBackgroundBox->setChecked(config.value(QLatin1String("customBackground"), false));
	QString customBackgroundPath = config.value(QLatin1String("backgroundPath"), QString());
	QColor customBackgroundColor = config.value(QLatin1String("backgroundColor"), QColor());
	QString fontFamily = config.value(QLatin1String("fontFamily"), m_style->defaultFontFamily());
	int fontSize = config.value(QLatin1String("fontSize"), m_style->defaultFontSize());
	setDefaultFont(fontFamily, fontSize);
	if (!customBackgroundPath.isEmpty())
		setBackgroundPath(customBackgroundPath);
	setBackgroundColor(customBackgroundColor);
	if (ui->customBackgroundBox->isChecked()) {
		m_style->setCustomBackgroundPath(customBackgroundPath);
		m_style->setCustomBackgroundColor(customBackgroundColor);
	}
	
	ui->variantBox->setEnabled(!m_style->variants().isEmpty());
	ui->variantBox->clear();
	if (ui->variantBox->isEnabled()) {
		ui->variantBox->addItems(m_style->variants());
		QString variant = config.value(QLatin1String("variant"), m_style->activeVariant());
		m_style->setActiveVariant(variant);
		int index = qMax(0, ui->variantBox->findText(m_style->activeVariant()));
		ui->variantBox->setCurrentIndex(index);
	}
	
	QString customFile = m_style->pathForResource(QLatin1String("Custom.json"));
	if (!customFile.isEmpty()) {
		QVariantList values = config.value(QLatin1String("customStyle")).toList();
		Config variables(customFile);
		const int count = variables.arraySize();
		for (int index = 0; index < count; index++) {
			variables.setArrayIndex(index);
			QString type = variables.value(QLatin1String("type"), QString());
			QString text = variables.value(QLatin1String("label"), QString());
			QString localizedText = QCoreApplication::translate("Style", text.toUtf8());
			if (localizedText == text)
				localizedText = variables.value(QLatin1String("label-") + QLocale().name(), text);
			text = localizedText;
			WebViewCustomStyle style;
			style.parameter = variables.value(QLatin1String("parameter"), QString());
			style.selector = variables.value(QLatin1String("selector"), QString());
			style.value = values.value(index, variables.value(QLatin1String("value"))).toString();
			if (type == QLatin1String("font")) {
				QLabel *label = new QLabel(text % ":", this);
				WebViewFont *fontField = new WebViewFont(style, this);
				ui->formLayout->addRow(label, fontField);
				connect(fontField, SIGNAL(changeValue()), SLOT(onVariableChanged()));
				connect(fontField, SIGNAL(destroyed()), label, SLOT(deleteLater()));
				m_variables.append(fontField);
			} else if (type == QLatin1String("color")) {
				QLabel *label = new QLabel(text % ":", this);
				WebViewColor *colorField = new WebViewColor(style, this);
				ui->formLayout->addRow(label, colorField);
				connect(colorField, SIGNAL(changeValue()), SLOT(onVariableChanged()));
				connect(colorField, SIGNAL(destroyed()), label, SLOT(deleteLater()));
				m_variables.append(colorField);
			} else if (type == QLatin1String("numeric")) {
				QLabel *label = new QLabel(text % ":", this);
				double min = variables.value("min", 0.0);
				double max = variables.value("max", 0.0);
				double step = variables.value("step", 1.0);
				WebViewNumeric *numField = new WebViewNumeric(style, min, max, step, this);
				ui->formLayout->addRow(label, numField);
				connect(numField, SIGNAL(changeValue()), SLOT(onVariableChanged()));
				connect(numField, SIGNAL(destroyed()), label, SLOT(deleteLater()));
				m_variables.append(numField);
			} else if (type == QLatin1String("boolean")) {
				QString trueValue = variables.value("true", QString());
				QString falseValue = variables.value("false", QString());
				WebViewBoolean *boolField = new WebViewBoolean(style, trueValue, falseValue, this);
				boolField->setText(text);
				ui->formLayout->addRow(boolField);
				connect(boolField, SIGNAL(changeValue()), SLOT(onVariableChanged()));
				m_variables.append(boolField);
			}
		}
	}
	updateCustomScript();
	rebuildChatView();
}

void WebViewAppearance::on_variantBox_currentIndexChanged(const QString &variant)
{
	m_style->setActiveVariant(variant);
	m_controller->evaluateJavaScript(m_style->scriptForChangingVariant());
	setModified(true);
}

void WebViewAppearance::on_showUserIconBox_clicked(bool checked)
{
    m_style->setShowUserIcons(checked);
	rebuildChatView();
	setModified(true);
}

void WebViewAppearance::on_showHeaderBox_clicked(bool checked)
{
    m_style->setShowHeader(checked);
	rebuildChatView();
	setModified(true);
}

void WebViewAppearance::on_fontButton_clicked()
{
	bool ok;
	QFont oldFont(m_controller->defaultFontFamily(), m_controller->defaultFontSize());
	QFont newFont = QFontDialog::getFont(&ok, oldFont, this);
	QFontInfo info(newFont);
	if (ok && newFont != oldFont) {
		setDefaultFont(newFont.family(), info.pointSize());
		setModified(true);
	}
}

void WebViewAppearance::on_defaultFontButton_clicked()
{
	setDefaultFont(m_style->defaultFontFamily(), m_style->defaultFontSize());
	setModified(true);
}

void WebViewAppearance::on_customBackgroundBox_clicked(bool checked)
{
	m_style->setCustomBackgroundPath(checked ? backgroundPath() : QString());
	m_style->setCustomBackgroundColor(checked ? backgroundColor() : QColor());
	rebuildChatView();
	setModified(true);
}

void WebViewAppearance::on_backgroundTypeBox_currentIndexChanged(int index)
{
    m_style->setCustomBackgroundType(static_cast<WebKitMessageViewStyle::WebkitBackgroundType>(index));
	rebuildChatView();
	setModified(true);
}

void WebViewAppearance::on_imageButton_clicked(bool checked)
{
	QString newPath;
	if (!checked) {
		m_style->setCustomBackgroundPath(QString());
	} else if (checked) {
		newPath = QFileDialog::getOpenFileName(this, tr("Choose background image"),
		                                        QDir::homePath(),
		                                        tr("Images (*.png *.xpm *.jpg *.bmp *.gif)"));
		if (!newPath.isEmpty())
			setBackgroundPath(newPath);
		else
			newPath = backgroundPath();
	}
	m_style->setCustomBackgroundPath(newPath);
	rebuildChatView();
	setModified(true);
}

void WebViewAppearance::on_colorButton_clicked(bool checked)
{
	QColor newColor;
	bool shouldRebuild = false;
	if (!checked) {
		m_style->setCustomBackgroundColor(QColor());
		shouldRebuild = true;
	} else if (checked) {
		QColor oldColor = backgroundColor();
		newColor = QColorDialog::getColor(oldColor, 0, QString(), QColorDialog::ShowAlphaChannel);
		if (newColor.isValid()) {
			setBackgroundColor(newColor);
			shouldRebuild = true;
		} else {
			ui->colorButton->setChecked(false);
		}
	}
	if (shouldRebuild) {
		m_style->setCustomBackgroundColor(newColor);
		rebuildChatView();
		setModified(true);
	}
}

}

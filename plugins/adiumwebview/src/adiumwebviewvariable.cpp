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
#include "adiumwebviewvariable.h"
#include <QStringBuilder>

namespace Adium
{

WebViewFont::WebViewFont(const WebViewCustomStyle &style, QWidget *parent) : QWidget(parent)
{
	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	setLayout(layout);
	fontLabel = new QLabel();
	QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	fontLabel->setSizePolicy(sizePolicy);
	changeButton = new QToolButton();
	changeButton->setText(tr("Change"));
	connect(changeButton, SIGNAL(clicked()), SLOT(changeCurrentFont()));
	layout->addWidget(fontLabel);
	layout->addWidget(changeButton);
	QString cvalue = style.value;
	QFont fvalue;
	QRegExp rxBold("(bold)");
	QRegExp rxItalic("(italic)");
	QRegExp rxSCaps("(small-caps)");
	QRegExp rxSize("(\\d+)(pt|px)");
	if (cvalue.contains(rxSize)) {
		if (rxSize.cap(2) == "pt")
			fvalue.setPointSize(rxSize.cap(1).toInt());
		else
			fvalue.setPixelSize(rxSize.cap(1).toInt());
		cvalue.remove(rxSize);
	}
	if (cvalue.contains(rxBold)) {
		fvalue.setBold(true);
		cvalue.remove(rxBold);
	}
	if (cvalue.contains(rxItalic)) {
		fvalue.setItalic(true);
		cvalue.remove(rxItalic);
	}
	if (cvalue.contains(rxSCaps)) {
		fvalue.setCapitalization(QFont::SmallCaps);
		cvalue.remove(rxBold);
	}
	cvalue = cvalue.simplified();
	fvalue.setFamily(cvalue);
	fontLabel->setFont(fvalue);
	fontLabel->setText(QString("%1 %2").arg(fvalue.pointSize() != -1
											? QString("%1pt").arg(fvalue.pointSize())
											: QString("%1px").arg(fvalue.pixelSize())).arg(fvalue.family()));
	m_style.selector = style.selector;
	m_style.parameter = style.parameter;
}

const WebViewCustomStyle &WebViewFont::chatStyle()
{
	QFont fvalue(fontLabel->font());
	QString fontSize(fvalue.pointSize() != -1
					 ? QString("%1pt ").arg(fvalue.pointSize())
					 : QString("%1px ").arg(fvalue.pixelSize()));
	m_style.value = QString(fvalue.bold() ? "bold " : "") % QString(fvalue.italic() ? "italic " : "")
			% QString(fvalue.capitalization() ? "small-caps " : "") % fontSize % fvalue.family();
	return m_style;
}

void WebViewFont::changeCurrentFont()
{
	bool ok;
	QFont newFont = QFontDialog::getFont(&ok, fontLabel->font(), this);
	if (ok && newFont != fontLabel->font())
	{
		fontLabel->setFont(newFont);
		fontLabel->setText(QString("%1 %2").arg(newFont.pointSize() != -1
												? QString("%1pt").arg(newFont.pointSize())
												: QString("%1px").arg(newFont.pixelSize())).arg(newFont.family()));
		emit changeValue();
	}
}

WebViewColor::WebViewColor(const WebViewCustomStyle &style, QWidget *parent) : QToolButton(parent)
{
	color = QColor(style.value);
	connect(this, SIGNAL(clicked()), SLOT(changeCurrentColor()));
	setStyleSheet("background: "+color.name());
	m_style.selector = style.selector;
	m_style.parameter = style.parameter;
}

const WebViewCustomStyle &WebViewColor::chatStyle()
{
	m_style.value = color.name();
	return m_style;
}

void WebViewColor::changeCurrentColor()
{
	QColor newColor = QColorDialog::getColor(color);
	if (newColor.isValid() && newColor != color)
	{
		color = newColor;
		setStyleSheet("background: "+color.name());
		emit changeValue();
	}
}

WebViewNumeric::WebViewNumeric(const WebViewCustomStyle &style,
						 double min, double max, double step, QWidget *parent) : QDoubleSpinBox(parent)
{
	connect(this, SIGNAL(valueChanged(double)), SLOT(onChangeValue()));
	int dec = QString::number(step).section(".", 1).length();
	setMinimum(min);
	setMaximum(max);
	setSingleStep(step);
	setDecimals(dec);
	setValue(style.value.toDouble());
	m_style.selector = style.selector;
	m_style.parameter = style.parameter;
}

const WebViewCustomStyle &WebViewNumeric::chatStyle()
{
	m_style.value = QString::number(value());
	return m_style;
}

void WebViewNumeric::onChangeValue()
{
	emit changeValue();
}

WebViewBoolean::WebViewBoolean(const WebViewCustomStyle &style,
						 const QString &trueValue, const QString &falseValue, QWidget *parent) : QCheckBox(parent)
{
	connect(this, SIGNAL(toggled(bool)), SLOT(onChangeValue()));
	m_trueValue = trueValue;
	m_falseValue = falseValue;
	if (style.value == m_trueValue)
		setChecked(true);
	else
		setChecked(false);
	m_style.selector = style.selector;
	m_style.parameter = style.parameter;
}

const WebViewCustomStyle &WebViewBoolean::chatStyle()
{
	m_style.value = isChecked() ? m_trueValue : m_falseValue;
	return m_style;
}

void WebViewBoolean::onChangeValue()
{
	emit changeValue();
}

}


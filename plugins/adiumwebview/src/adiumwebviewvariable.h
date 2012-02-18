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
#ifndef CHATVARIABLE_H
#define CHATVARIABLE_H

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QFontDialog>
#include <QColorDialog>
#include <QColor>
#include <QDoubleSpinBox>
#include <QCheckBox>

namespace Adium
{

struct WebViewCustomStyle
{
	QString selector;
	QString parameter;
	QString value;
};

class WebViewVariable
{
public:
	virtual const WebViewCustomStyle &chatStyle() = 0;
	virtual ~WebViewVariable() {}
};

}
Q_DECLARE_INTERFACE(Adium::WebViewVariable, "org.qutim.core.ChatVariable")

namespace Adium
{

class WebViewFont : public QWidget, public WebViewVariable
{
	Q_OBJECT
	Q_INTERFACES(Adium::WebViewVariable)
public:
	WebViewFont(const WebViewCustomStyle &style, QWidget *parent = 0);
	const WebViewCustomStyle &chatStyle();
signals:
	void changeValue();
private slots:
	void changeCurrentFont();
private:
	QLabel *fontLabel;
	QToolButton *changeButton;
	WebViewCustomStyle m_style;
};

class WebViewColor : public QToolButton, public WebViewVariable
{
	Q_OBJECT
	Q_INTERFACES(Adium::WebViewVariable)
public:
	WebViewColor(const WebViewCustomStyle &style, QWidget *parent = 0);
	const WebViewCustomStyle &chatStyle();
signals:
	void changeValue();
private slots:
	void changeCurrentColor();
private:
	QColor color;
	WebViewCustomStyle m_style;
};

class WebViewNumeric : public QDoubleSpinBox, public WebViewVariable
{
	Q_OBJECT
	Q_INTERFACES(Adium::WebViewVariable)
public:
	WebViewNumeric(const WebViewCustomStyle &style,
				double min, double max, double step, QWidget *parent = 0);
	const WebViewCustomStyle &chatStyle();
signals:
	void changeValue();
private slots:
	void onChangeValue();
private:
	WebViewCustomStyle m_style;
};

class WebViewBoolean : public QCheckBox, public WebViewVariable
{
	Q_OBJECT
	Q_INTERFACES(Adium::WebViewVariable)
public:
	WebViewBoolean(const WebViewCustomStyle &style,
				const QString &trueValue, const QString &falseValue,  QWidget *parent = 0);
	const WebViewCustomStyle &chatStyle();
signals:
	void changeValue();
private slots:
	void onChangeValue();
private:
	WebViewCustomStyle m_style;
	QString m_trueValue;
	QString m_falseValue;
};

}

#endif // CHATVARIABLE_H


/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

namespace Core
{
namespace AdiumChat
{

struct CustomChatStyle
{
	QString selector;
	QString parameter;
	QString value;
};

class ChatVariable
{
public:
	virtual const CustomChatStyle &chatStyle() = 0;
	virtual ~ChatVariable() {}
};

}
}
Q_DECLARE_INTERFACE(Core::AdiumChat::ChatVariable, "org.qutim.core.ChatVariable")

namespace Core
{
namespace AdiumChat
{

class ChatFont : public QWidget, ChatVariable
{
	Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatVariable)
public:
	ChatFont(const CustomChatStyle &style, QWidget *parent = 0);
	const CustomChatStyle &chatStyle();
signals:
	void changeValue();
private slots:
	void changeCurrentFont();
private:
	QLabel *fontLabel;
	QToolButton *changeButton;
	CustomChatStyle m_style;
};

class ChatColor : public QToolButton, ChatVariable
{
	Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatVariable)
public:
	ChatColor(const CustomChatStyle &style, QWidget *parent = 0);
	const CustomChatStyle &chatStyle();
signals:
	void changeValue();
private slots:
	void changeCurrentColor();
private:
	QColor color;
	CustomChatStyle m_style;
};

class ChatNumeric : public QDoubleSpinBox, ChatVariable
{
	Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatVariable)
public:
	ChatNumeric(const CustomChatStyle &style,
				double min, double max, double step, QWidget *parent = 0);
	const CustomChatStyle &chatStyle();
signals:
	void changeValue();
private slots:
	void onChangeValue();
private:
	CustomChatStyle m_style;
};

class ChatBoolean : public QCheckBox, ChatVariable
{
	Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatVariable)
public:
	ChatBoolean(const CustomChatStyle &style,
				const QString &trueValue, const QString &falseValue,  QWidget *parent = 0);
	const CustomChatStyle &chatStyle();
signals:
	void changeValue();
private slots:
	void onChangeValue();
private:
	CustomChatStyle m_style;
	QString m_trueValue;
	QString m_falseValue;
};

}
}

#endif // CHATVARIABLE_H


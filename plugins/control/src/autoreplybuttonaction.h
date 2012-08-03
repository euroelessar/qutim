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

#ifndef CONTROL_AUTOREPLYBUTTONACTION_H
#define CONTROL_AUTOREPLYBUTTONACTION_H

#include <QWidgetAction>
#include <qutim/actiongenerator.h>
#include <qutim/contact.h>
#include <qutim/message.h>

class QSpinBox;
class QToolButton;

namespace Control
{

class AutoReplyButtonActionGenerator : public qutim_sdk_0_3::ActionGenerator
{
public:
	AutoReplyButtonActionGenerator();
	
protected:
	virtual QObject *generateHelper() const;
	virtual void createImpl(QAction *action,QObject *obj) const;
	virtual void showImpl(QAction *action,QObject *obj);
	virtual void hideImpl(QAction *action,QObject *obj);
	
private:
};

class AutoReplyButtonAction : public QWidgetAction
{
	Q_OBJECT
public:
	explicit AutoReplyButtonAction(QObject *parent = 0);
	
	void setController(QObject *controller);
signals:
	
protected:
	virtual QWidget *createWidget(QWidget *parent);
    virtual void deleteWidget(QWidget *widget);
	
private:
	QWeakPointer<qutim_sdk_0_3::Contact> m_controller;
};

class AutoReplyButton : public QWidget
{
	Q_OBJECT
public:
	AutoReplyButton(QWidget *parent, qutim_sdk_0_3::Contact *contact);
	~AutoReplyButton();
	
protected slots:
	void onButtonClicked();
	
private:
	qutim_sdk_0_3::Contact *m_contact;
	QSpinBox *m_spinBox;
	QToolButton *m_button;
	qutim_sdk_0_3::MessageList m_messages;
};

}

#endif // CONTROL_AUTOREPLYBUTTONACTION_H

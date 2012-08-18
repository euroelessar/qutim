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

#ifndef CONTROL_QUICKANSWERBUTTONACTION_H
#define CONTROL_QUICKANSWERBUTTONACTION_H

#include <qutim/actiongenerator.h>
#include <qutim/contact.h>
#include <QWidgetAction>
#include <QListWidget>
#include <QStyledItemDelegate>

namespace Control {

class QuickAnswerMenu : public QListWidget
{
	Q_OBJECT
public:
	QuickAnswerMenu(qutim_sdk_0_3::Contact *contact);
	
	void mousePressEvent(QMouseEvent *);
	
protected slots:
	void onAnswersChanged(const QStringList &answers);
	void onItemClicked(QListWidgetItem *item);
	
private:
	qutim_sdk_0_3::Contact *m_contact;
};

class QuickAnswerButtonActionGenerator : public qutim_sdk_0_3::ActionGenerator
{
public:
	QuickAnswerButtonActionGenerator(QObject *object, const char *slot);
	
protected:
	virtual QObject *generateHelper() const;
//	virtual void createImpl(QAction *action,QObject *obj) const;
//	virtual void showImpl(QAction *action,QObject *obj);
//	virtual void hideImpl(QAction *action,QObject *obj);
	
private:
};

class ItemDeledate : public QStyledItemDelegate
{
public:
	ItemDeledate(QObject *parent = 0);
	void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
};

class QuickAnswerButtonAction : public QWidgetAction
{
	Q_OBJECT
public:
	explicit QuickAnswerButtonAction(QObject *parent = 0);
	
signals:
	
public slots:
	
protected:
	virtual QWidget *createWidget(QWidget *parent);
    virtual void deleteWidget(QWidget *widget);
};

} // namespace Control

#endif // CONTROL_QUICKANSWERBUTTONACTION_H

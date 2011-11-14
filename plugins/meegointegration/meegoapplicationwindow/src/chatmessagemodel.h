/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#ifndef CHATMESSAGEMODEL_H
#define CHATMESSAGEMODEL_H

#include <QAbstractListModel>
#include <qutim/message.h>

namespace MeegoIntegration
{
class ChatMessageModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ChatMessageModel(QObject *parent = 0);

	void append(qutim_sdk_0_3::Message &msg);
	bool eventFilter(QObject *, QEvent *);
	
	// QAbstractListModel
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
signals:

public slots:

private:
	QString createSenderName(const qutim_sdk_0_3::Message &msg) const;
	struct Item
	{
		QString plainText;
		QString richText;
	};
	QList<Item> m_items;

	QList<qutim_sdk_0_3::Message> m_messages;
};
}

#endif // CHATMESSAGEMODEL_H

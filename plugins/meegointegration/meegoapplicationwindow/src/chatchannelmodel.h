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

#ifndef CHATCHANNELMODEL_H
#define CHATCHANNELMODEL_H

#include <QAbstractListModel>
#include <qutim/chatsession.h>

namespace MeegoIntegration
{
class ChatChannelModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ChatChannelModel(QObject *parent = 0);
	
	// QAbstractListModel
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

signals:

public slots:

private slots:
	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
	void onSessionUnreadChanged();
	void onSessionDeath(QObject *object);
	
private:
	QList<qutim_sdk_0_3::ChatSession*> m_sessions;
};
}

#endif // CHATCHANNELMODEL_H

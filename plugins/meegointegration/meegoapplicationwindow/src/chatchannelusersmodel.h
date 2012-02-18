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

#ifndef CHATCHANNELMODEL_H
#define CHATCHANNELMODEL_H

#include <QAbstractListModel>
#include <qutim/chatsession.h>

namespace MeegoIntegration
{
class ChatChannelUsersModel : public QAbstractListModel
{
    Q_OBJECT
	Q_PROPERTY(QString statusPrefix READ statusPrefix WRITE setStatusPrefix NOTIFY statusPrefixChanged)
	Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    explicit ChatChannelUsersModel(QObject *parent = 0);
	
	QString statusPrefix();
	void setStatusPrefix(const QString &prefix);
	void addUnit(qutim_sdk_0_3::Buddy *unit);
	void removeUnit(qutim_sdk_0_3::Buddy *unit);
	
	// QAbstractListModel
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
signals:
	void statusPrefixChanged(const QString &prefix);
	void countChanged(int arg);
	
private slots:
	void onUnitDeath(QObject *object);
	void onStatusChanged(const qutim_sdk_0_3::Status &status);
	void onTitleChanged(const QString &title, const QString &oldTitle);
	
private:
	struct Node {
		Node(qutim_sdk_0_3::Buddy *u, const QString &t) : title(t), unit(u) {}
		Node(qutim_sdk_0_3::Buddy *u) : title(u->title()), unit(u) {}
		Node() : unit(NULL) {}
		QString title;
		qutim_sdk_0_3::Buddy *unit;
		
		bool operator <(const Node &o) const
		{
			const int cmp = title.compare(o.title, Qt::CaseInsensitive);
			return cmp < 0 || (cmp == 0 && unit < o.unit);
		}
	};

	QList<Node> m_units;
	QString m_statusPrefix;
};
}

#endif // CHATCHANNELMODEL_H


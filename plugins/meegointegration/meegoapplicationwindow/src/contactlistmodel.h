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

#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include <QAbstractListModel>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/contact.h>

namespace MeegoIntegration
{
class ContactListModel : public QAbstractListModel
{
    Q_OBJECT
	Q_PROPERTY(QString statusPrefix READ statusPrefix WRITE setStatusPrefix NOTIFY statusPrefixChanged)
	Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
	Q_PROPERTY(bool showOffline READ showOffline WRITE setShowOffline NOTIFY showOfflineChanged)
	Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
	
    ContactListModel();
	virtual ~ContactListModel();
	
	QString statusPrefix();
	void setStatusPrefix(const QString &prefix);
	QString filter() const;
	void setFilter(const QString &filter);
	bool showOffline() const;
	void setShowOffline(bool showOffline);
	
	Q_INVOKABLE QVariant get(int index);
	
	// QAbstractListModel
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public slots:
	
signals:
	void statusPrefixChanged(const QString &prefix);
	void filterChanged(const QString &filter);
	void showOfflineChanged(bool showOffline);
	void countChanged(int count);
	
private slots:
	void onAccountCreated(Ureen::Account *account);
	void onAccountDeath(QObject *object);
	void onContactCreated(Ureen::Contact *contact);
	void onContactTitleChanged(const QString &title, const QString &oldTitle);
	void onContactStatusChanged(const Ureen::Status &status);
	void onContactDeath(QObject *object);
	
private:
	void checkVisibility();
	void checkVisibility(Ureen::Contact *contact, bool forced = false);
	bool isVisible(Ureen::Contact *contact);
	int indexOfContact(Ureen::Contact *contact) const;
	struct Item {
		QString title;
		Ureen::Contact *contact;
		
		bool operator <(const Item &o) const;
	};
	QHash<QObject*, QString> m_titles;
	QList<Item> m_contacts;
	QString m_filter;
	bool m_showOffline;
	QString m_statusPrefix;
};
}

#endif // CONTACTLISTMODEL_H


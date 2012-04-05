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

#ifndef JABBER_JPGPKEYDIALOG_H
#define JABBER_JPGPKEYDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QtCrypto>

class QPushButton;

namespace Jabber {

namespace Ui {
class JPGPKeyDialog;
}

class JPGPProxyModel : public QSortFilterProxyModel
{
public:
    JPGPProxyModel(QObject *parent);

    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
};

class JPGPKeyDialog : public QDialog
{
	Q_OBJECT
public:
	enum Type {
		PublicKeys,
		SecretKeys
	};
	
	explicit JPGPKeyDialog(Type type, const QString &pgpKeyId, QWidget *parent = 0);
	~JPGPKeyDialog();

	QString addKeyEntry(const QCA::KeyStoreEntry &keyEntry);
	virtual void done(int result);

	QCA::KeyStoreEntry keyStoreEntry() const;

public slots:
	void onAddKeyButtonClicked();
	void onTimerShot();
	
private:
	Ui::JPGPKeyDialog *ui;
	Type m_type;
	QPushButton *m_addButton;
	JPGPProxyModel *m_proxyModel;
	QStandardItemModel *m_model;
	QStringList m_keysToAdd;
};


} // namespace Jabber
#endif // JABBER_JPGPKEYDIALOG_H

/****************************************************************************
**
** qutIM instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#include "addaccountdialogwrapper.h"
#include "quickaddaccountdialog.h"
#include <qdeclarative.h>
#include <qutim/protocol.h>
#include <qutim/contact.h>
#include <qutim/icon.h>
#include <qutim/chatsession.h>
#include <qutim/servicemanager.h>
#include <QStringBuilder>
#include <qutim/extensioninfo.h>
#include <qutim/icon.h>
#include <qutim/metaobjectbuilder.h>
#include <qutim/debug.h>
#include <qutim/servicemanager.h>


namespace MeegoIntegration
{
enum {
	AccountRole = Qt::UserRole,
	WizardRole,
	InfoRole
};

Q_GLOBAL_STATIC(QList<AddAccountDialogWrapper*>, m_managers)
QuickAddAccountDialog* AddAccountDialogWrapper::m_currentDialog;

AddAccountDialogWrapper::AddAccountDialogWrapper()
{
	m_managers()->append(this);
	m_wizards = new QList<AccountCreationWizard *>();
	QHash<int, QByteArray> roleNames;
	roleNames.insert(WizardRole, "wizard");
	roleNames.insert(Qt::DisplayRole, "display");
	roleNames.insert(InfoRole, "info");
	setRoleNames(roleNames);

}

void AddAccountDialogWrapper::loadAccounts()
{
	if (m_wizards)
	{
		beginRemoveRows(QModelIndex(),0,m_wizards->size());
		m_wizards->clear();
		endRemoveRows();
	}
	else
		m_wizards = new QList<AccountCreationWizard*>();

	QSet<QByteArray> protocols;
	foreach (Protocol *protocol, Protocol::all()) {
		protocols.insert(protocol->metaObject()->className());
	}

	foreach(const ObjectGenerator *gen, ObjectGenerator::module<AccountCreationWizard>()) {
		const char *proto = MetaObjectBuilder::info(gen->metaObject(), "DependsOn");
		if (!protocols.contains(proto))
			continue;
		AccountCreationWizard *wizard = gen->generate<AccountCreationWizard>();
		beginInsertRows(QModelIndex(), m_wizards->size(), m_wizards->size());
		m_wizards->append(wizard);
		endInsertRows();
	}

}


AddAccountDialogWrapper::~AddAccountDialogWrapper()
{
	m_managers()->removeOne(this);
}

void AddAccountDialogWrapper::init()
{
	qmlRegisterType<AddAccountDialogWrapper>("org.qutim", 0, 3, "AccountCreator");

}


void AddAccountDialogWrapper::showDialog(QuickAddAccountDialog * dialog)
{
	m_currentDialog = dialog;
	showDialog();
}

void AddAccountDialogWrapper::showDialog()
{
	qDebug()<<"Show Account Creator Dialog";
	for (int i = 0; i < m_managers()->count();i++)
	{
		m_managers()->at(i)->loadAccounts();
		qDebug()<<"Show Account Creator";
		emit m_managers()->at(i)->shown();
	}
}

int AddAccountDialogWrapper::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_wizards->size();
}

QVariant AddAccountDialogWrapper::data(const QModelIndex &index, int role) const
{
	if (index.row() < 0 || index.row() > m_wizards->size())
		return QVariant();
	AccountCreationWizard *wizard = m_wizards->at(index.row());
	ExtensionInfo info = wizard->info();
	QIcon icon = info.icon();
	if (!icon.actualSize(QSize(1,1)).isValid())
		icon = Icon("applications-internet");
	switch (role) {
	case Qt::DisplayRole:
		return info.name().toString();
	case Qt::DecorationRole:
		return QString();
	case WizardRole:
		return m_wizards->indexOf(wizard);
	case InfoRole:{
		return info.description().toString();}
	default:
		return QVariant();
	}

}

QObject* AddAccountDialogWrapper::getWidget(int index)
{
	return m_wizards->at(index)->createPages(m_wizard).at(0);
}

bool AddAccountDialogWrapper::validateWidget(QObject* widget)
{
	QWizardPage *wiz=qobject_cast<QWizardPage*>(widget);
	return (wiz->validatePage());
}

bool AddAccountDialogWrapper::checkOpen()
{
	foreach (Protocol *proto,Protocol::all()) {
		if (!proto->accounts().isEmpty())
			return false;
	}
	return true;
}


}


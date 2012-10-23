/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef SIMPLEACTIONS_H
#define SIMPLEACTIONS_H
#include <QObject>
#include <QScopedPointer>
#include "qutim/inforequest.h"

class QAction;
namespace Ureen
{
class ActionGenerator;
class Status;
class Account;
}

namespace Core {

class SimpleActions : public QObject
{
	Q_OBJECT
public:
	SimpleActions();
	virtual ~SimpleActions();
private slots:
	void onTagsEditAction(QObject*);
	void onCopyIdCreated(QAction *, QObject *);
	void onCopyIdTriggered(QObject *obj);
	void onContactRenameAction(QObject*);
	void onContactNameSelected(const QString &name);
	void onShowInfoAction(QObject *obj);
	void onShowInfoActionCreated(QAction *, QObject *);
	void onInformationSupportLevelChanged(Ureen::InfoRequestFactory::SupportLevel level);
	void onContactAddRemoveActionCreated(QAction *, QObject *);
	void onContactAddRemoveAction(QObject*);
	void onRemoveChoosed(int result);
	void onContactAddRemoveActionDestroyed();
	void onAccountCreated(Ureen::Account *account);
	void onAccountStatusChanged(const Ureen::Status &);
	void inListChanged(bool);
	void onDisableSoundActionCreated(QAction *, QObject *);
	void onDisableSoundAction(QAction *action);
	void onNotificationBackendStateChanged(const QByteArray &type, bool enabled);
	void onNotificationBackendCreated(const QByteArray &type);
	void onNotificationBackendDestroyed(const QByteArray &type);
	void onJoinLeave(QObject *);
	void onJoinLeaveActionCreated(QAction *, QObject *);
	void onJoinedChanged(bool);
private:
	void setDisableSoundActionVisibility(const QByteArray &backendType, bool visible);
private:
	QScopedPointer<Ureen::ActionGenerator> m_tagEditGen;
	QScopedPointer<Ureen::ActionGenerator> m_copyIdGen;
	QScopedPointer<Ureen::ActionGenerator> m_contactRenameGen;
	QScopedPointer<Ureen::ActionGenerator> m_contactAddRemoveGen;
	QScopedPointer<Ureen::ActionGenerator> m_showInfoGen;
	QScopedPointer<Ureen::ActionGenerator> m_disableSound;
	QScopedPointer<Ureen::ActionGenerator> m_joinGroupLeaveGen;
};

} // namespace Core

#endif // SIMPLEACTIONS_H


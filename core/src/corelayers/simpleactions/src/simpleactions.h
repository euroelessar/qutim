/****************************************************************************
 *  simpleactions.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *  Copyright (c) 2011 by Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef SIMPLEACTIONS_H
#define SIMPLEACTIONS_H
#include <QObject>
#include <QScopedPointer>
#include "qutim/inforequest.h"

class QAction;
namespace qutim_sdk_0_3
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
	void onShowInfoAction(QObject *obj);
	void onShowInfoActionCreated(QAction *, QObject *);
	void onInformationSupportLevelChanged(qutim_sdk_0_3::InfoRequestFactory::SupportLevel level);
	void onContactAddRemoveActionCreated(QAction *, QObject *);
	void onContactAddRemoveAction(QObject*);
	void onContactAddRemoveActionDestroyed();
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &);
	void inListChanged(bool);
	void onDisableSoundActionCreated(QAction *, QObject *);
	void onDisableSoundAction(QAction *action);
	void onNotificationBackendStateChanged(const QByteArray &type, bool enabled);
	void onNotificationBackendCreated(const QByteArray &type);
	void onNotificationBackendDestroyed(const QByteArray &type);
private:
	void setDisableSoundActionVisibility(const QByteArray &backendType, bool visible);
private:
	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_tagEditGen;
	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_copyIdGen;
	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_contactRenameGen;
	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_contactAddRemoveGen;
	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_showInfoGen;
	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_disableSound;
};

} // namespace Core

#endif // SIMPLEACTIONS_H

/****************************************************************************
 *  simpleactions.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

class QAction;
namespace qutim_sdk_0_3
{
class ActionGenerator;
class Status;
}

namespace Core {

class SimpleActions : public QObject
{
	Q_OBJECT
public:
	SimpleActions();
	virtual ~SimpleActions();
private slots:
	void onTagsEditCreated(QAction *, QObject *);
	void onTagsEditAction(QObject*);
	void onCopyIdCreated(QAction *, QObject *);
	void onCopyIdTriggered(QObject *obj);
	void onContactRenameCreated(QAction *, QObject *);
	void onContactRenameAction(QObject*);
	void onShowInfoAction(QObject *obj);
	void onShowInfoActionCreated(QAction *, QObject *);
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &);
private:;
	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_tagEditGen;
	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_copyIdGen;
	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_contactRenameGen;
	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_showInfoGen;
};

} // namespace Core

#endif // SIMPLEACTIONS_H

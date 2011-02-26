/****************************************************************************
 *  simpleactions.cpp
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

#include "simpleactions.h"
#include <qutim/contact.h>
#include "simpletagseditor/simpletagseditor.h"
#include <qutim/systemintegration.h>
#include <qutim/icon.h>
#include <qutim/debug.h>
#include <qutim/account.h>
#include <qutim/chatunit.h>
#include <qutim/protocol.h>
#include <QClipboard>
#include <QApplication>
#include <QDesktopWidget>

namespace Core {

using namespace qutim_sdk_0_3;

class CopyIdGenerator : public ActionGenerator
{
public:
	CopyIdGenerator(QObject *obj) :
		ActionGenerator(Icon("edit-copy"),QT_TRANSLATE_NOOP("ContactList", "Copy id to clipboard"),obj,SLOT(onCopyIdTriggered(QObject*)))
	{
		setType(ActionTypeContactList|ActionTypeAdditional);
	}
};

SimpleActions::SimpleActions()
{
	m_tagEditGen.reset(new ActionGenerator(Icon("feed-subscribe"),
										   QT_TRANSLATE_NOOP("ContactList", "Edit tags"),
										   this, SLOT(onTagsEditAction(QObject*))));
	m_tagEditGen->subscribe(this, SLOT(onTagsEditCreated(QAction*, QObject*)));
	MenuController::addAction<Contact>(m_tagEditGen.data());

	m_copyIdGen.reset(new CopyIdGenerator(this));
	m_copyIdGen->subscribe(this, SLOT(onCopyIdCreated(QAction*,QObject*)));
	m_copyIdGen->setPriority(-100);
	MenuController::addAction<ChatUnit>(m_copyIdGen.data());
}

SimpleActions::~SimpleActions()
{

}

void SimpleActions::onTagsEditAction(QObject *controller)
{
	Contact *contact = qobject_cast<Contact*>(controller);
	if (!contact)
		return;
	SimpleTagsEditor *editor = new SimpleTagsEditor (contact);
	centerizeWidget(editor);
	editor->setTags(contact->tags());
	editor->load();
	SystemIntegration::show(editor);
}

void SimpleActions::onTagsEditCreated(QAction *a, QObject *o)
{
	debug() << o << ActionGenerator::get(a);
}

void SimpleActions::onCopyIdCreated(QAction *action, QObject *obj)
{
	ChatUnit *unit = qobject_cast<ChatUnit*>(obj);
	Q_ASSERT(unit);
	QString id =  unit->account()->protocol()->data(Protocol::ProtocolIdName).toString();
	action->setText(QObject::tr("Copy %1 to clipboard").arg(id));
}

void SimpleActions::onCopyIdTriggered(QObject *obj)
{
	ChatUnit *unit = qobject_cast<ChatUnit*>(obj);
	Q_ASSERT(unit);
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(unit->id());
}


} // namespace Core


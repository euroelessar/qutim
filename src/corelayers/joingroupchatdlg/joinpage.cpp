/****************************************************************************
 *  joinpage.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "joinpage.h"
#include <qutim/event.h>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QAction>
#include <QApplication>
#include <qutim/dataforms.h>
#include <qutim/account.h>

using namespace qutim_sdk_0_3;

namespace Core {

JoinPage::JoinPage(QWidget *parent) :
	GroupChatPage(parent)
{
	QCheckBox *box = new QCheckBox(QT_TRANSLATE_NOOP("JoinGroupChat", "Save to bookmarks"),this);
	m_bookmarksEdit = new QLineEdit(this);
	connect(box,SIGNAL(stateChanged(int)),m_bookmarksEdit,SLOT(setEnabled(bool)));
	m_bookmarksEdit->setEnabled(false);
	m_layout->addWidget(box);
	m_layout->addWidget(m_bookmarksEdit);

	QAction *action = new QAction(this);
	action->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Join"));
	action->setSoftKeyRole(QAction::PositiveSoftKey);
	connect(action,SIGNAL(triggered()),SLOT(join()));
	addAction(action);
}

void JoinPage::updateDataForm()
{
	Event event("groupchat-fields");
	qApp->sendEvent(account(),&event);
	DataItem item = event.at<DataItem>(0);

	if (m_dataForm)
		m_dataForm->deleteLater();

	m_dataForm = AbstractDataForm::get(item);
	if (m_dataForm) {
		m_dataForm->setParent(this);
		m_dataForm->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
		m_layout->insertWidget(0,m_dataForm.data());
	}
}

void JoinPage::join()
{
	DataItem item = qobject_cast<AbstractDataForm*>(m_dataForm)->item();
	Event event ("groupchat-join",qVariantFromValue(item));
	if (m_bookmarksEdit->isEnabled()) {
		event.args[1] = true;
		event.args[2] = m_bookmarksEdit->text();
	}
	qApp->sendEvent(account(),&event);
	emit joined();
}

void JoinPage::showEvent(QShowEvent *ev)
{
	updateDataForm();
	QScrollArea::showEvent(ev);
}

} // namespace Core

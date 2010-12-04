/****************************************************************************
 *  bookmarkspage.cpp
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

#include "bookmarkspage.h"
#include "bookmarksmodel.h"
#include <QVBoxLayout>
#include <QComboBox>
#include <QAction>
#include <itemdelegate.h>
#include <qutim/event.h>
#include <qutim/account.h>
#include <qutim/dataforms.h>
#include <QLabel>
#include <QApplication>

namespace Core {

using namespace qutim_sdk_0_3;

BookmarksPage::BookmarksPage(QWidget *parent) :
	GroupChatPage(parent)
{
	m_bookmarksBox = new QComboBox(this);
	m_bookmarksBox->setIconSize(QSize(22,22));
	m_bookmarksBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
	m_layout->addWidget(new QLabel(QT_TRANSLATE_NOOP("JoinGroupChat", "Select bookmark"),
								   this));
	m_layout->addWidget(m_bookmarksBox);

	QAction *action = new QAction(this);
	action->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Save"));
	action->setSoftKeyRole(QAction::PositiveSoftKey);
	connect(action,SIGNAL(triggered()),SLOT(onSave()));
	addAction(action);

	m_removeAction = new QAction(this);
	m_removeAction->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Remove"));
	m_removeAction->setSoftKeyRole(QAction::PositiveSoftKey);
	connect(m_removeAction,SIGNAL(triggered()),SLOT(onRemove()));
	addAction(m_removeAction);

	m_bookmarksBox->setItemDelegate(new ItemDelegate(this));
	connect(m_bookmarksBox,SIGNAL(currentIndexChanged(int)),SLOT(onCurrentIndexChanged(int)));
}

void BookmarksPage::setModel(BookmarksModel *model)
{
	m_bookmarksBox->setModel(model);
}

void BookmarksPage::updateDataForm(const QString &name)
{
	Event event("groupchat-fields");
	event.args[1] = name;
	event.args[2] = true;
	qApp->sendEvent(account(), &event);
	DataItem item = event.at<DataItem>(0);
	if (m_dataForm)
		m_dataForm->deleteLater();

	m_dataForm = AbstractDataForm::get(item);
	if (m_dataForm) {
		m_dataForm->setParent(this);
		m_dataForm->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
		m_layout->insertWidget(2,m_dataForm.data());
	}
}

void BookmarksPage::onCurrentIndexChanged(int index)
{
	m_removeAction->setVisible(index ? !isRecent(index) : false);
	updateDataForm(m_bookmarksBox->itemText(index));
}

void BookmarksPage::onSave()
{
	DataItem item = qobject_cast<AbstractDataForm*>(m_dataForm)->item();
	Event event ("groupchat-bookmark-save", qVariantFromValue(item));
	event.args[1] = m_bookmarksBox->currentText(); //old name
	qApp->sendEvent(account(), &event);
	emit bookmarksChanged();
}

void BookmarksPage::onRemove()
{
	Event event ("groupchat-bookmark-remove", m_bookmarksBox->currentText());
	qApp->sendEvent(account(), &event);
	emit bookmarksChanged();
}

bool BookmarksPage::isRecent(int index)
{
	BookmarksModel *model = static_cast<BookmarksModel*>(m_bookmarksBox->model());
	BookmarkType type = model->data(index, BookmarkTypeRole).value<BookmarkType>();
	return type == BookmarkRecentItem;
}

void BookmarksPage::showEvent(QShowEvent *ev)
{
	updateDataForm();
	GroupChatPage::showEvent(ev);
}

} // namespace Core

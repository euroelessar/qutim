/****************************************************************************
**
** qutIM - instant messenger
**
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

#include "bookmarkspage.h"
#include "bookmarksmodel.h"
#include <QVBoxLayout>
#include <QComboBox>
#include <QAction>
#include <qutim/itemdelegate.h>
#include <qutim/event.h>
#include <qutim/account.h>
#include <qutim/groupchatmanager.h>
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
	m_removeAction->setSoftKeyRole(QAction::NegativeSoftKey);
	m_removeAction->setVisible(false);
	connect(m_removeAction,SIGNAL(triggered()),SLOT(onRemove()));
	addAction(m_removeAction);

	m_bookmarksBox->setItemDelegate(new ItemDelegate(this));
	connect(m_bookmarksBox,SIGNAL(currentIndexChanged(int)),SLOT(onCurrentIndexChanged(int)));
}

void BookmarksPage::setModel(BookmarksModel *model)
{
	m_bookmarksBox->setModel(model);
}

void BookmarksPage::updateDataForm(DataItem fields)
{
	if (m_dataForm)
		m_dataForm.data()->deleteLater();

	if (fields.isNull()) {
		GroupChatManager *manager = account()->groupChatManager();
		if (!manager)
			return;
		fields = manager->fields();
		if (fields.isNull())
			return;
	}

	m_dataForm = AbstractDataForm::get(fields);
	if (m_dataForm) {
		m_dataForm.data()->setParent(this);
		m_dataForm.data()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
		m_layout->insertWidget(2,m_dataForm.data());
	}
}

void BookmarksPage::onCurrentIndexChanged(int index)
{
	m_removeAction->setVisible(index ? !isRecent(index) : false);
	updateDataForm(fields(index));
}

void BookmarksPage::onSave()
{
	GroupChatManager *manager = account()->groupChatManager();
	if (!manager)
		return;
	DataItem fields = m_dataForm.data()->item();
	DataItem oldFields = this->fields(m_bookmarksBox->currentIndex());
	if (fields.isNull())
		return;
	manager->storeBookmark(fields, oldFields);
	emit bookmarksChanged();
}

void BookmarksPage::onRemove()
{
	GroupChatManager *manager = account()->groupChatManager();
	if (!manager)
		return;

	DataItem fields = this->fields(m_bookmarksBox->currentIndex());
	if (fields.isNull())
		return;
	manager->removeBookmark(fields);
	emit bookmarksChanged();
}

bool BookmarksPage::isRecent(int index)
{
	BookmarksModel *model = static_cast<BookmarksModel*>(m_bookmarksBox->model());
	BookmarkType type = model->data(index, BookmarkTypeRole).value<BookmarkType>();
	return type == BookmarkRecentItem;
}

DataItem BookmarksPage::fields(int index)
{
	if(!index)
		return account()->groupChatManager()->fields();
	BookmarksModel *model = static_cast<BookmarksModel*>(m_bookmarksBox->model());
	return model->data(index, Qt::UserRole).value<DataItem>();
}

void BookmarksPage::showEvent(QShowEvent *ev)
{
	updateDataForm(fields(m_bookmarksBox->currentIndex()));
	GroupChatPage::showEvent(ev);
}

} // namespace Core


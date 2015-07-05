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

#include "joinpage.h"
#include <qutim/event.h>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QAction>
#include <QApplication>
#include <qutim/dataforms.h>
#include <qutim/account.h>
#include <qutim/groupchatmanager.h>

using namespace qutim_sdk_0_3;

namespace Core {

JoinPage::JoinPage(QWidget *parent) :
	GroupChatPage(parent)
{
	QCheckBox *box = new QCheckBox(QT_TRANSLATE_NOOP("JoinGroupChat", "Save to bookmarks"),this);
	m_layout->addWidget(box);

	m_joinAction = new QAction(this);
	m_joinAction->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Join"));
	connect(m_joinAction,SIGNAL(triggered()),SLOT(join()));
	addAction(m_joinAction);
}

void JoinPage::updateDataForm()
{
	if (m_dataForm)
		m_dataForm.data()->deleteLater();
	m_joinAction->setEnabled(false);

	GroupChatManager *manager = account()->groupChatManager();
	if (!manager)
		return;

	DataItem item = manager->fields();
	m_dataForm = AbstractDataForm::get(item);
	if (m_dataForm) {
		m_dataForm.data()->setParent(this);
		m_dataForm.data()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
		m_layout->insertWidget(0,m_dataForm.data());
		m_joinAction->setEnabled(m_dataForm.data()->isComplete());
		connect(m_dataForm.data(), SIGNAL(completeChanged(bool)),
				m_joinAction, SLOT(setEnabled(bool)));
	}
}

void JoinPage::join()
{
	if (!m_dataForm)
		return;

	GroupChatManager *manager = account()->groupChatManager();
	if (!manager)
		return;

	DataItem item = m_dataForm.data()->item();
	manager->join(item);
	emit joined();
}

void JoinPage::showEvent(QShowEvent *ev)
{
	updateDataForm();
	QScrollArea::showEvent(ev);
}

} // namespace Core


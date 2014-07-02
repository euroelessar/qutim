/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
** Copyright © 2014 Nicolay Izoderov <nico-izo@ya.ru>
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

#include "uripage.h"
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

UriPage::UriPage(QWidget *parent) :
	GroupChatPage(parent)
{
	QCheckBox *box = new QCheckBox(QT_TRANSLATE_NOOP("JoinGroupChat", "Save to bookmarks"),this);
	m_layout->addWidget(box);

	m_joinAction = new QAction(this);
	m_joinAction->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Join"));
	connect(m_joinAction,SIGNAL(triggered()),SLOT(join()));
	addAction(m_joinAction);
}

void UriPage::updateDataForm()
{
	if (m_dataForm)
		m_dataForm.data()->deleteLater();
	m_joinAction->setEnabled(false);

	GroupChatManager *manager = account()->groupChatManager();
	if (!manager)
		return;

	DataItem item = manager->fields();
	QList<DataItem> items = item.subitems();
	QList<DataItem>::const_iterator i;
	for(i = items.constBegin(); i != items.constEnd(); ++i)
		if((*i).name() == "conference") break;
	items[i-items.constBegin()] = DataItem("conference", QT_TRANSLATE_NOOP("Jabber", "Conference"), m_uri);
	item.setSubitems(items);

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

void UriPage::join()
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

void UriPage::showEvent(QShowEvent *ev)
{
	updateDataForm();
	QScrollArea::showEvent(ev);
}

void UriPage::setUri(const QString &uri)
{
	m_uri = uri;
}

void UriPage::setUri(const QUrl &)
{

}

} // namespace Core


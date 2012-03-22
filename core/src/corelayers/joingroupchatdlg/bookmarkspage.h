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

#ifndef BOOKMARKSPAGE_H
#define BOOKMARKSPAGE_H

#include <QScrollArea>
#include <QWeakPointer>
#include "groupchatpage.h"
#include <qutim/dataforms.h>

class QComboBox;
namespace Core {

class BookmarksModel;
class BookmarksPage : public GroupChatPage
{
    Q_OBJECT
public:
	explicit BookmarksPage(QWidget *parent = 0);
	void setModel(BookmarksModel *model);
signals:
	void bookmarksChanged();
public slots:
	void updateDataForm(qutim_sdk_0_3::DataItem fields);
protected:
	void showEvent(QShowEvent *);
private slots:
	void onCurrentIndexChanged(int index);
	void onSave();
	void onRemove();
private:
	bool isRecent(int index);
	qutim_sdk_0_3::DataItem fields(int index);
	QComboBox *m_bookmarksBox;
	QAction *m_removeAction;
	QWeakPointer<qutim_sdk_0_3::AbstractDataForm> m_dataForm;
};

} // namespace Core

#endif // BOOKMARKSPAGE_H


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

#ifndef JOINPAGE_H
#define JOINPAGE_H

#include <QScrollArea>
#include <QWeakPointer>
#include "groupchatpage.h"
#include <qutim/dataforms.h>

class QVBoxLayout;
class QCheckBox;
class QLineEdit;
namespace Core {

class JoinPage : public GroupChatPage
{
    Q_OBJECT
public:
	explicit JoinPage(QWidget *parent = 0);
public slots:
	void join();
	void updateDataForm();
protected:
	void showEvent(QShowEvent *);
signals:
	void joined();
private:
	QWeakPointer<qutim_sdk_0_3::AbstractDataForm> m_dataForm;
	QAction *m_joinAction;
};

} // namespace Core

#endif // JOINPAGE_H


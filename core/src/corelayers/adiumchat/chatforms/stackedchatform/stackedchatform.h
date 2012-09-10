/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef STACKEDCHATFORM_H
#define STACKEDCHATFORM_H

#include <qutim/adiumchat/abstractchatform.h>

namespace qutim_sdk_0_3
{
	class SettingsItem;
}

namespace Core
{
namespace AdiumChat
{

class StackedChatForm : public AbstractChatForm
{
	Q_OBJECT
	Q_CLASSINFO("Uses","SettingsLayer")
public:
	explicit StackedChatForm();
	~StackedChatForm();
protected:
	virtual AbstractChatWidget *createWidget(const QString &key);
private:
	qutim_sdk_0_3::SettingsItem *m_settingsItem;
};

}
}

#endif //STACKEDCHATFORM_H


/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef STACKEDCHATBEHAVIOR_H
#define STACKEDCHATBEHAVIOR_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include <qutim/adiumchat/chatedit.h>
#include "../stackedchatwidget.h"

namespace Ui {
class StackedChatBehavior;
}

namespace Core
{
namespace AdiumChat
{
namespace Symbian
{


using namespace qutim_sdk_0_3;
class StackedChatBehavior : public SettingsWidget
{
	Q_OBJECT

public:
	explicit StackedChatBehavior();
	virtual ~StackedChatBehavior();
	virtual void cancelImpl();
	virtual void loadImpl();
	virtual void saveImpl();
protected:
	void changeEvent(QEvent *e);
private slots:
	void onValueChanged();
private:
	Ui::StackedChatBehavior *ui;
};

} // namespace Symbian
} // namespace AdiumChat
} // namespace Core
#endif // STACKEDCHATBEHAVIOR_H


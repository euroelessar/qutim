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
#ifndef CHATAPPEARANCE_H
#define CHATAPPEARANCE_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include <QHash>

namespace Ui
{
	class quickChatAppearance;
}
namespace Core
{

namespace AdiumChat
{

using namespace qutim_sdk_0_3;
class ChatVariable;
class ChatViewController;
class QuickChatController;
class QuickChatViewWidget;
class ChatAppearance : public SettingsWidget
{
	Q_OBJECT
public:
	ChatAppearance();
	virtual void cancelImpl();
	virtual void loadImpl();
	virtual void saveImpl();
	virtual ~ChatAppearance();
private slots:
	void onThemeChanged(int index);
	void onStateChanged(int);
private:
	void getThemes();
	void makePage();
	Ui::quickChatAppearance *ui;
	QStringList m_themes;
	QString m_currentStyleName;
	QuickChatController *m_controller;
	QuickChatViewWidget *m_widget;
	bool isLoad;
};

}
}

#endif // CHATAPPEARANCE_H


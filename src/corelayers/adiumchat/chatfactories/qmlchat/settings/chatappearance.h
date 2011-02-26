/****************************************************************************
 *  chatappearance.cpp.cpp
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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
#ifndef CHATAPPEARANCE_H
#define CHATAPPEARANCE_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include <QHash>

namespace Ui
{
	class chatAppearance;
}
namespace Core
{

namespace AdiumChat
{

using namespace qutim_sdk_0_3;
class ChatVariable;
class ChatViewController;
class QuickChatViewController;
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
private:
	void getThemes();
	void makePage();
	Ui::chatAppearance *ui;
	QStringList m_themes;
	QString m_currentStyleName;
	QuickChatViewController *m_controller;
	QuickChatViewWidget *m_widget;
	bool isLoad;
};

}
}

#endif // CHATAPPEARANCE_H

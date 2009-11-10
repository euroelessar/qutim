/****************************************************************************
 *  xsettingsdialog.cpp
 *
 *  Copyright (c) 2009 by Sidorov Aleksey <sauron@citadelspb.com>
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
#include <libqutim/settingswidget.h>
#include "chatlayerimpl.h"
namespace Ui
{
	class chatAppearance;
}
namespace AdiumChat
{
	class ChatAppearance : public SettingsWidget
	{
		Q_OBJECT
	public:
		ChatAppearance();
		virtual void cancelImpl();
		virtual void loadImpl();
		virtual void saveImpl();
		virtual ~ChatAppearance();
	private:
		Ui::chatAppearance *ui;
	};

}

#endif // CHATAPPEARANCE_H

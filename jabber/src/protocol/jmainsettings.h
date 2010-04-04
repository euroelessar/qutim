/*
Copyright (c) 2009 by Denis Daschenko <daschenko@gmail.com>
***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************
*/

#ifndef JABBERSETTINGS_H
#define JABBERSETTINGS_H

#include <qutim/settingswidget.h>
#include <qutim/configbase.h>

namespace Ui
{
	class JMainSettings;
}

namespace Jabber
{
	using namespace qutim_sdk_0_3;

	class JMainSettings: public SettingsWidget
	{
		Q_OBJECT
		public:
			JMainSettings();
			~JMainSettings();
			void loadImpl();
			void cancelImpl();
			void saveImpl();
		private:
			Ui::JMainSettings *ui;

	};
}

#endif // JABBERSETTINGS_H

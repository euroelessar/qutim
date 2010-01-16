/****************************************************************************
 *  localizationsettings.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef LOCALIZATIONSETTINGS_H
#define LOCALIZATIONSETTINGS_H

#include "libqutim/settingswidget.h"
#include <QtCore/QHash>

namespace Ui {
    class LocalizationSettings;
}

class QListWidgetItem;

namespace Core
{
	class LocalizationSettings : public qutim_sdk_0_3::SettingsWidget
	{
		Q_OBJECT
	public:
		LocalizationSettings();
		~LocalizationSettings();

	protected:
		virtual void loadImpl();
		virtual void saveImpl();
		virtual void cancelImpl();
		void changeEvent(QEvent *e);

	private:
		Ui::LocalizationSettings *m_ui;
		QHash<QString, QListWidgetItem *> m_items;
	};
}

#endif // LOCALIZATIONSETTINGS_H

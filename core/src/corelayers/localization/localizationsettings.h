/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
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

#ifndef LOCALIZATIONSETTINGS_H
#define LOCALIZATIONSETTINGS_H

#include <qutim/settingswidget.h>
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

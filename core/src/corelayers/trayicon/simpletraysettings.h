/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef SIMPLETRAYSETTINGS_H
#define SIMPLETRAYSETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>

namespace Ui {
class SimpletraySettings;
}

namespace Core {

extern const char * traySettingsFilename;

class SimpletraySettings : public qutim_sdk_0_3::SettingsWidget
{
	Q_OBJECT	
public:
	enum Option {
		CounterDontShow,
		CounterShowMessages,
		CounterShowSessions
	};
	explicit SimpletraySettings(QWidget *parent = 0);
	~SimpletraySettings();
protected:
	void loadImpl();
	void saveImpl();
	void cancelImpl();

private:
	::Ui::SimpletraySettings *ui;
};

}

#endif // SIMPLETRAYSETTINGS_H


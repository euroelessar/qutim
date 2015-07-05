/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef FILETRANSFERSETTINGS_H
#define FILETRANSFERSETTINGS_H

#include <qutim/settingswidget.h>
#include <qutim/startupmodule.h>

class QListWidget;

namespace Core {

using namespace qutim_sdk_0_3;

class FileTransferSettingsWidget : public SettingsWidget
{
	Q_OBJECT
public:
	FileTransferSettingsWidget();
protected:
	void clearState();
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
private slots:
	void onChanged();
private:
	QListWidget *m_factoriesWidget;
	bool m_changed;
};

class FileTransferSettings : public QObject, public qutim_sdk_0_3::StartupModule
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::StartupModule)
public:
	FileTransferSettings();
};

} // namespace Core

#endif // FILETRANSFERSETTINGS_H


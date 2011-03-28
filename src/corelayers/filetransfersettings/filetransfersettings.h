/****************************************************************************
 *  filetransfersettings.h
 *
 *  Copyright (c) 2011 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

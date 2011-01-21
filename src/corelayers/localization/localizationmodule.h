/****************************************************************************
 *  localizationmodule.h
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

#ifndef LOCALIZATIONMODULE_H
#define LOCALIZATIONMODULE_H

#include <qutim/startupmodule.h>
#include <QtCore/QTranslator>

namespace Core
{
	class LocalizationModule : public QObject, public qutim_sdk_0_3::StartupModule
	{
		Q_OBJECT
		Q_INTERFACES(qutim_sdk_0_3::StartupModule)
	public:
		LocalizationModule();
		
		static QStringList determineSystemLocale();
		static void loadLanguage(const QStringList &langs);
	public slots:
		void onSettingsSave();
	};
}

#endif // LOCALIZATIONMODULE_H

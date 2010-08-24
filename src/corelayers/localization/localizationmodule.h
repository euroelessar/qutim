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

#include <qutim/libqutim_global.h>
#include <QtCore/QTranslator>

namespace Core
{
	class LocalizationModule : public QObject
	{
		Q_OBJECT
	public:
		LocalizationModule();
	public slots:
		void onSettingsSave();
	private:
		void loadLanguage(const QString &lang);
		QList<QTranslator *> m_translators;
	};
}

#endif // LOCALIZATIONMODULE_H

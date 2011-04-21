/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef OLDSOUNDTHEMEPROVIDER_H
#define OLDSOUNDTHEMEPROVIDER_H

#include <qutim/notificationslayer.h>

using namespace qutim_sdk_0_3;

namespace Core
{
class OldSoundThemeProvider : public SoundThemeProvider
{
public:
    OldSoundThemeProvider(const QString &name, const QString &path, QString variant);
	virtual bool setSoundPath(Notification::Type sound, const QString &file);
	virtual QString soundPath(Notification::Type sound);
	virtual QString themeName();
	virtual bool saveTheme();
protected:
	QMap<Notification::Type, QString> m_map;
	QString m_filePath;
	QString m_themeName;
};
}

#endif // OLDSOUNDTHEMEPROVIDER_H

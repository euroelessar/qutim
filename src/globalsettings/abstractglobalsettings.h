/*
    AbstractGlobalSettings

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include <QString>

#ifndef ABSTRACTGLOBALSETTINGS_H_
#define ABSTRACTGLOBALSETTINGS_H_

class AbstractGlobalSettings
{
public:
	AbstractGlobalSettings();
	virtual ~AbstractGlobalSettings();
	static AbstractGlobalSettings &instance();
	void setProfileName(const QString &profile_name);
	void loadNetworkSettings();
private:
	QString m_profile_name;
};

#endif /*ABSTRACTGLOBALSETTINGS_H_*/

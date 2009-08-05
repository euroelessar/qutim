/*
    iconManager

    Copyright (c) 2008 by Alexey Pereslavtsev <steel1.0@mail.ru>
    						Nigmatullin Ruslan <euroelessar@gmail.com>

 This file is a part of qutIM project.
 You can find more information at http://qutim.org


 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/


#ifndef ICONMANAGER_H
#define ICONMANAGER_H
#include "include/qutim/iconmanagerinterface.h"
#include <QString>
#include <QIcon>
#include <QHash>
#include <QtXml>

using namespace qutim_sdk_0_2;

/*!
 * Icon manager class.
 * If you use icons, then you should use this class to get path to your icon.
 * IconManager knows about some kinds of icons.
 */
class IconManager : public qutim_sdk_0_2::IconManagerInterface
{
	IconManager();
	virtual ~IconManager() {}
public:
	typedef QPair<QString,QIcon> Icon;
	typedef QHash<QString,Icon>  QStringIconHash;
	struct Icons
	{
		inline Icons() : use_default(false) {}
		QHash<QString,QStringIconHash> icons;
		bool use_default;
		QStringIconHash standart;
		QString path;
		QString qrc_path;
	};
	//! The constructor sets all path to internal.

	//! Call instance of IconManager
	static IconManager &instance();

	//! This function returns full path to some system icon.
	//! \param Short name of icon (can be without extension)
	//! \example ui.deleteButton->setIcon (QIcon (iconManager->getSystemIconName("delete_user")));
	inline QString getIconFileName( const QString &icon_name ) { return getIconPath( icon_name, IconInfo::System ); }

	//! This function returns some system icon
	//! \param Short name of icon (can be without extension)
	//! \example ui.deleteButton->setIcon (iconManager->getSystemIcon("delete_user"));
	inline QIcon getIcon( const QString &icon_name ) { return getIcon( icon_name, IconInfo::System ); }

	inline QString getStatusIconFileName( const QString& icon_name, const QString &default_path = QString() ) { return getIconPath( icon_name, IconInfo::Status, default_path ); }
	inline QIcon getStatusIcon( const QString &icon_name, const QString &default_path = QString() ) { return getIcon( icon_name, IconInfo::Status, default_path ); }

	void loadProfile( const QString & profile_name );

	QIcon getIcon( const QString &name, IconInfo::Type category, const QString &subtype = QString() ) const;
	QString getIconPath( const QString &name, IconInfo::Type category, const QString &subtype = QString() ) const;
	bool setIcon( const IconInfo &icon, const QString &path );
	bool addPack( const QString &name, const IconInfoList &pack );
private:
	const Icon *getIconInfo( const QString &name, IconInfo::Type category, const QString &subtype );
	bool tryAddIcon( const QString &name, IconInfo::Type category, const QString &subtype, bool qrc = false );
	bool setStatusIconPackage( Icons &icons );
	void loadSettings();

	QString m_profile_name;

	QList<Icons> m_icons;
};

namespace qutim_sdk_0_2 {

class Icon : public QIcon
{
public:
	inline Icon( const QString &name, IconInfo::Type category, const QString &subtype = QString() )
			: QIcon(IconManager::instance().getIcon(name,category,subtype)) {}
	inline Icon( const QString &name, const QString &subtype = QString() )
			: QIcon(IconManager::instance().getIcon(name,IconInfo::System,subtype)) {}
	inline Icon( const QIcon &icon )
			: QIcon(icon) {}
};

}

#endif //ICONMANAGER_H

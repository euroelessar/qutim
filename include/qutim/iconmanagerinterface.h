/*!
 * Icon Manager Interface
 *
 * @author Nigmatullin Ruslan
 * Copyright (c) 2009 by Nigmatullin Ruslan «euroelessar@gmail.com»
 * encoding: UTF-8
 *
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 */

#ifndef ICONMANAGERINTERFACE_H
#define ICONMANAGERINTERFACE_H

#include <QIcon>
#include <qutim/plugininterface.h>

namespace qutim_sdk_0_2 {

/** 
* @brief Description of icon.
*/
struct IconInfo
{
	/** 
	* @brief Type of icon
	*/
	enum Type
	{
		System = 0,
		Status,
		Client,
		Plugin,
		Invalid,
		Protocol = Plugin
	};
	/** 
	* @brief Name of icon, i.e. "crazy"
	*/
	QString name;
	/** 
	* @brief Category of icon, i.e. System
	*/
	Type category;
	/** 
	* @brief Subtype of icon, i.e. "moods". Usually is empty.
	*/
	QString subtype;
	/** 
	* @brief Graphic representation of icon
	*/
	QIcon icon;
};
typedef QList<IconInfo> IconInfoList;

/** 
* @brief Interface of IconManager for access to icons of core
*/
class IconManagerInterface
{
public:
	virtual ~IconManagerInterface() {}

	/** 
	* @brief Get icon by it's name, category and subttype
	* 
	* @param name Name of icon
	* @param category Category of icon
	* @param subtype Subtype of icon
	* 
	* @return Icon from core or null icon if no icon found
	*/
	virtual QIcon getIcon( const QString &name, IconInfo::Type category, const QString &subtype = QString() ) const = 0;
	/** 
	* @brief Get icon path by icon's name, category and subtype
	* 
	* @param name Name of icon
	* @param category Category of icon
	* @param subtype Subtype of icon
	* 
	* @return Path to icon from core or empty string if no icon found
	*/
	virtual QString getIconPath( const QString &name, IconInfo::Type category, const QString &subtype = QString() ) const = 0;

	// TODO: SDK 0.3 Merge this two methods
	/** 
	* @brief Add icon to core
	* 
	* @param icon Description of icon
	* @param path Path to icon for tooltip, if empty core guess it itself
	* 
	* @return true if adding is success, otherwise false
	*/
	virtual bool setIcon( const IconInfo &icon, const QString &path ) { Q_UNUSED(icon); Q_UNUSED(path); return false; }

	// For binary compatibility with older soft
	/** 
	* @brief Overload of setIcon.
	* 
	* @param icon Description of icon.
	* 
	* @return true if adding is success, otherwise false
	*/
	inline bool setIcon( const IconInfo &icon )
	{
		return setIcon(icon, QString());
	}

	/** 
	* @brief Adding list of icons by it's descriptions
	* 
	* @param name Name of package
	* @param pack List of icons' descriptions
	* 
	* @return true if any icon has been added, otherwise false
	*/
	virtual bool addPack( const QString &name, const IconInfoList &pack ) { Q_UNUSED(name); Q_UNUSED(pack); return false; }


	/** 
	* @brief Overload of getIcon with category System
	* 
	* @param name Name of icon
	* @param subtype Subtype of icon
	* 
	* @return Icon from core or null icon if not found
	*/
	inline QIcon getIcon( const QString &name, const QString &subtype = QString() )
	{
		return getIcon( name, IconInfo::System, subtype );
	}

	/** 
	* @brief Overload of getIconPath with category System
	* 
	* @param name Name of icon
	* @param subtype Subtype of icon
	* 
	* @return Path to icon from core or empty string if not found
	*/
	inline QString getIconPath( const QString &name, const QString &subtype = QString() )
	{
		return getIconPath( name, IconInfo::System, subtype );
	}
};

#ifndef BUILD_QUTIM
/** 
* @brief A wrapper around QIcon which provides access to qutIM icons.
*/
class Icon : public QIcon
{
public:
	/** 
	* @brief Constructor of icon by name, category and subtype
	* 
	* @param name Name of icon
	* @param category Category of icon
	* @param subtype Subtype of icon
	*/
	inline Icon( const QString &name, IconInfo::Type category, const QString &subtype = QString() )
			: QIcon(SystemsCity::IconManager()->getIcon(name,category,subtype)) {}
	/** 
	* @brief Overload
	* 
	* @param name Name of icon
	* @param subtype Subtype of icon
	*/
	inline Icon( const QString &name, const QString &subtype = QString() )
			: QIcon(SystemsCity::IconManager()->getIcon(name,IconInfo::System,subtype)) {}
	/** 
	* @brief Copy constructor which takes an QIcon
	* 
	* @param icon Icon to copy
	*/
	inline Icon( const QIcon &icon )
			: QIcon(icon) {}
};

typedef IconManagerInterface IconManager;
#endif

}

#endif // ICONMANAGERINTERFACE_H

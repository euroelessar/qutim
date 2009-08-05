/*
    iconManager

    Copyright (c) 2008 by Alexey Pereslavtsev <steel1.0@mail.ru>
    						Nigmatullin Ruslan <euroelessar@gmail.com>
    						Dmitri Arekhta <DaemonES@gmail.com>

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

#include "iconmanager.h"
#include "pluginsystem.h"

#include <QImageReader>

static bool icon_manager_initialized = false;

IconManager::IconManager()
{
	for( int i = 0; i < IconInfo::Invalid; i++ )
		m_icons << Icons();
}
IconManager &IconManager::instance()
{
	static IconManager ims;
	return ims;
}

static inline void setPath( IconManager::Icons &icons, const QString &path, const QString &qrc_path )
{
	QDir dir = path;
	if( dir.exists() )
		icons.path = path;
	icons.qrc_path = qrc_path;
}

void IconManager::loadSettings()
{
	icon_manager_initialized = true;
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings" );
	settings.beginGroup( "gui" );
	setPath( m_icons[IconInfo::System], settings.value( "systemicon", QString() ).toString(), ":/icons/core" );
	setPath( m_icons[IconInfo::Status], settings.value( "statusicon", QString() ).toString(), ":/icons" );
	if( m_icons[IconInfo::Status].path.isEmpty() )
		m_icons[IconInfo::Status].path = m_icons[IconInfo::System].path;
	setPath( m_icons[IconInfo::Client], settings.value( "clienticon", QString() ).toString(), ":/icons/clients" );
	setPath( m_icons[IconInfo::Protocol], settings.value( "protocolicon", QString() ).toString(), ":/icons/protocol" );
	setStatusIconPackage( m_icons[IconInfo::Status] );
	settings.endGroup();
}

void IconManager::loadProfile(const QString & profile_name)
{
	m_profile_name = profile_name;
	loadSettings();
}

static QString tryGetFilePath( const QDir &dir, const QString &name, bool ini = true )
{
	QString base_path = dir.absoluteFilePath( name );
	QString path = base_path;
	if( !dir.exists( name ) )
	{
		if( ini && dir.exists( name + ".ini" ) )
			return dir.filePath( name + ".ini" );
		QFileInfo file;
		file.setFile( dir, name );
		QList<QByteArray> extensions = QImageReader::supportedImageFormats();
		int current = extensions.indexOf( file.suffix().toLatin1() );
		if( current > 0 )
			extensions.swap( 0, current );
		current = 0;
		do
		{
			path = base_path + QLatin1Char('.') + QString::fromLatin1(extensions.at(current++).constData());
			file.setFile( path );
		}
		while( current < extensions.size() && !file.exists() );
		if( !file.exists() )
			path = QString();
	}
	return path;
}

bool IconManager::setStatusIconPackage( Icons &icons )
{
	QDir dir = icons.path;
	if( !dir.exists() )
		return false;
	if( icons.path.endsWith( ".AdiumStatusIcons", Qt::CaseInsensitive ) )
	{
		QHash<QString,QString> from_adium;
		from_adium.insert( "Generic Available", "online" );
		from_adium.insert( "Free for Chat", "ffc" );
		from_adium.insert( "Generic Away", "away" );
		from_adium.insert( "Not Available", "na" );
		from_adium.insert( "Occupied", "occupied" );
		from_adium.insert( "DND", "dnd" );
		from_adium.insert( "Invisible", "invisible" );
		from_adium.insert( "Offline", "offline" );
		from_adium.insert( "Unknown", "connecting" );
		from_adium.insert( "Available At Home", "athome" );
		from_adium.insert( "Available At Work", "atwork" );
		from_adium.insert( "Lunch", "lunch" );
		from_adium.insert( "Available Evil", "evil" );
		from_adium.insert( "Available Depression", "depression" );
		from_adium.insert( "Without Authorization", "noauth" );
		QSettings style(dir.filePath( "Icons.plist"), PluginSystem::instance().plistFormat());
		QVariantMap tabs = style.value( "Tabs" ).toMap();
		if( !tabs.isEmpty() )
		{
			QVariantMap::iterator it = tabs.begin();
			for( ; it != tabs.end(); it++ )
			{
				QString name = from_adium.value( it.key(), QString() );
				if( name.isEmpty() )
					continue;
				QString path = tryGetFilePath( dir, it.value().toString(), false );
				if( !path.isEmpty() )
					icons.standart.insert( name, Icon( path, QIcon(path) ) );
			}
		}
		if( !icons.standart.contains( "online") || !icons.standart.contains( "offline" ) )
		{
			icons.standart.clear();
			return false;
		}
		icons.use_default = true;
		icons.path = icons.qrc_path = QString();
		if( !icons.standart.contains( "ffc" ) )
			icons.standart["ffc"] = icons.standart["online"];
		if( !icons.standart.contains( "na" ) )
			icons.standart["na"] = icons.standart["away"];
		if( !icons.standart.contains( "occupied" ) )
			icons.standart["occupied"] = icons.standart["away"];
		if( !icons.standart.contains( "dnd" ) )
			icons.standart["dnd"] = icons.standart["away"];
		if( !icons.standart.contains( "athome" ) )
			icons.standart["athome"] = icons.standart["online"];
		if( !icons.standart.contains( "atwork" ) )
			icons.standart["atwork"] = icons.standart["online"];
		if( !icons.standart.contains( "lunch" ) )
			icons.standart["lunch"] = icons.standart["online"];
		if( !icons.standart.contains( "evil" ) )
			icons.standart["evil"] = icons.standart["online"];
		if( !icons.standart.contains( "depression" ) )
			icons.standart["depression"] = icons.standart["online"];
		if( !icons.standart.contains( "noauth" ) )
			icons.standart["noauth"] = icons.standart["offline"];
		return true;
	}
	return false;
}

bool IconManager::tryAddIcon( const QString &name, IconInfo::Type category, const QString &subtype, bool qrc )
{
	if( category >= IconInfo::Invalid )
		return false;
	Icons &icons = m_icons[category];
	if( !qrc && icons.path.isEmpty() )
		qrc = true;
	QDir dir = qrc ? icons.qrc_path : icons.path;
	if( !subtype.isEmpty() )
	{
//		qDebug() << name << subtype << dir.absolutePath();
		bool result = dir.cd(subtype);
//		qDebug() << dir.absolutePath() << result;
		if( !result )
			return qrc ? false : tryAddIcon( name, category, subtype, true );
	}
	QString path = tryGetFilePath( dir, name );
	if( path.isEmpty() )
		return qrc ? false : tryAddIcon( name, category, subtype, true );
	else if( path.endsWith( ".ini" ) )
	{
		QSettings settings( path, QSettings::IniFormat );
		QString default_icon = settings.value( "icon/default", QString() ).toString();
		QStringList icons_list = settings.value( "icon/list", QStringList() ).toStringList();
		if( icons_list.isEmpty() )
		{
			path = tryGetFilePath( dir, name, false );
			if( path.isEmpty() )
				return qrc ? false : tryAddIcon( name, category, subtype, true );
		}
		else
		{
			if( default_icon.isEmpty() )
				default_icon = icons_list.first();
			QStringIconHash &hash = ( icons.use_default || subtype.isEmpty() ) ? icons.standart : icons.icons[subtype];
			QIcon icon;
			foreach( const QString &path, icons_list )
				icon.addFile( dir.absoluteFilePath( path ) );
			hash.insert( name, Icon( dir.absoluteFilePath( default_icon ), icon ) );
			return true;
		}
	}
	QIcon icon( path );
	QStringIconHash &hash = ( icons.use_default || subtype.isEmpty() ) ? icons.standart : icons.icons[subtype];
	hash.insert( name, Icon( path, icon ) );
	return true;
}

static const IconManager::Icon null_icon = IconManager::Icon( QString(), QIcon() );

const IconManager::Icon *IconManager::getIconInfo( const QString &name, IconInfo::Type category, const QString &subtype )
{
	if( category < 0 || category >= IconInfo::Invalid || !icon_manager_initialized )
		return &null_icon;
	Icons &icons = m_icons[category];
	QStringIconHash *hash = 0;
	if( icons.use_default || subtype.isEmpty() )
		hash = &icons.standart;
	else
	{
		QHash<QString,QStringIconHash>::iterator iter = icons.icons.find( subtype );
		if( iter == icons.icons.end() )
		{
			if( tryAddIcon( name, category, subtype ) )
				iter = icons.icons.find( subtype );
			else
				return getIconInfo( name, category, QString() );
		}
		hash = &iter.value();
	}
	QStringIconHash::iterator it = hash->find( name );
	if( it == hash->end() )
	{
		if( tryAddIcon( name, category, subtype ) )
			it = hash->find( name );
		else
		{
			if( icons.use_default || subtype.isEmpty() )
				return &(hash->operator []( name ) = null_icon);
			return getIconInfo( name, category, QString() );
		}
	}
	return &it.value();
}

QIcon IconManager::getIcon( const QString &name, IconInfo::Type category, const QString &subtype ) const
{
	const Icon *icon = const_cast<IconManager *>(this)->getIconInfo( name, category, subtype );
	if( !icon )
		return QIcon();
	return icon->second;
}

QString IconManager::getIconPath( const QString &name, IconInfo::Type category, const QString &subtype ) const
{
	const Icon *icon = const_cast<IconManager *>(this)->getIconInfo( name, category, subtype );
	if( !icon )
		return QString();
	return icon->first;
}

bool IconManager::setIcon( const IconInfo &icon, const QString &path )
{
	if( icon.category < 0 || icon.category >= IconInfo::Invalid )
		return false;
	Icons &icons = m_icons[icon.category];
	QStringIconHash *hash = 0;
	if( icons.use_default || icon.subtype.isEmpty() )
		hash = &icons.standart;
	else
		hash = &icons.icons[icon.subtype];
	Icon &new_icon = hash->operator []( icon.name );
	if( !new_icon.first.isEmpty() )
		return false;
	QString tmp_path = path;
	if(path.isEmpty())
	{
		QSize size;
#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
		QList<QSize> sizes = icon.icon.availableSizes();
		if(sizes.isEmpty())
			size = QSize(16, 16);
		else
		{
			size = sizes.first();
			for(int i=1; i < sizes.size(); i++)
			{
				if(sizes.at(i).height() < size.height())
					size = sizes.at(i);
			}
		}
#else
		size = QSize(16, 16);
#endif
		QPixmap pixmap = icon.icon.pixmap(size);
		QTemporaryFile *file = new QTemporaryFile(QDir::temp().filePath("qutim_temp"));
		QObject::connect(qApp, SIGNAL(aboutToQuit()), file, SLOT(deleteLater()));
		pixmap.save(file, "png");
		tmp_path = file->fileName();
	}
	new_icon.first = tmp_path;
	new_icon.second = icon.icon;
	return true;
}

bool IconManager::addPack( const QString &name, const IconInfoList &pack )
{
	QStringList icon_names;
	foreach(const IconInfo &info, pack)
		setIcon(info, QString());
	return true;
}

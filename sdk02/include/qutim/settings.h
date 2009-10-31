/*!
 * Settings Interface
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <qutim/plugininterface.h>
#include <QDir>

namespace qutim_sdk_0_2 {

class Settings
{
public:
	enum FieldType
	{
		General = 0,
		Password
	};
	inline Settings( const TreeModelItem &item );
	inline Settings( const QString &name );
	inline ~Settings();

	inline void clear();
	inline void sync();

	inline void beginGroup( const QString &prefix );
	inline void endGroup();
	inline QString group() const;

	inline int beginReadArray( const QString &prefix );
	inline void beginWriteArray( const QString &prefix, int size = -1 );
	inline void endArray();
	inline void setArrayIndex( int i );

	inline QStringList allKeys() const;
	inline QStringList childKeys() const;
	inline QStringList childGroups() const;
	inline bool isWritable() const;

	inline QVariant value( const QString &key, QVariant default_value = QVariant() );
	inline void setValue( FieldType type, const QString &key, const QVariant &value );
	inline void setValue( const QString &key, const QVariant &value ) { setValue( General, key, value ); }

	inline static QString getProfilePath() { return SystemsCity::PluginSystem()->getProfilePath(); }
	inline static QDir getProfileDir() { return SystemsCity::PluginSystem()->getProfileDir(); }

private:
	Q_DISABLE_COPY(Settings)
	SettingsInterface *p;
};

class SettingsInterface
{
friend class Settings;
protected:
	virtual ~SettingsInterface() {}
	virtual void clear() = 0;
	virtual void sync() = 0;

	virtual void beginGroup( const QString &prefix ) = 0;
	virtual void endGroup() = 0;
	virtual QString group() const = 0;

	virtual int beginReadArray( const QString &prefix ) = 0;
	virtual void beginWriteArray( const QString &prefix, int size = -1 ) = 0;
	virtual void endArray() = 0;
	virtual void setArrayIndex( int i ) = 0;

	virtual QStringList allKeys() const = 0;
	virtual QStringList childKeys() const = 0;
	virtual QStringList childGroups() const = 0;
	virtual bool isWritable() const = 0;

	virtual QVariant value( const QString &key, QVariant default_value = QVariant() ) = 0;
	virtual void setValue( Settings::FieldType type, const QString &key, const QVariant &value ) = 0;
};

inline Settings::Settings( const TreeModelItem &item )
	: p(SystemsCity::PluginSystem()->getSettings(item)) {}

inline Settings::Settings( const QString &name )
	: p(SystemsCity::PluginSystem()->getSettings(name)) {}

inline Settings::~Settings()
	{ if( p ) delete p; }

inline void Settings::clear()
	{ if( p ) p->clear(); }

inline void Settings::sync()
	{ if( p ) p->sync(); }

inline void Settings::beginGroup( const QString &prefix )
	{ if( p ) p->beginGroup( prefix ); }

inline void Settings::endGroup()
	{ if( p ) p->endGroup(); }

inline QString Settings::group() const
	{ return p ? p->group() : QString(); }

inline int Settings::beginReadArray( const QString &prefix )
	{ return p ? p->beginReadArray( prefix ) : 0; }

inline void Settings::beginWriteArray( const QString &prefix, int size )
	{ if( p ) p->beginWriteArray( prefix, size ); }

inline void Settings::endArray()
	{ if( p ) p->endArray(); }

inline void Settings::setArrayIndex( int i )
	{ if( p ) p->setArrayIndex( i ); }

inline QStringList Settings::allKeys() const
	{ return p ? p->allKeys() : QStringList(); }

inline QStringList Settings::childKeys() const
	{ return p ? p->childKeys() : QStringList(); }

inline QStringList Settings::childGroups() const
	{ return p ? p->childGroups() : QStringList(); }

inline bool Settings::isWritable() const
	{ return p ? p->isWritable() : false; }

inline QVariant Settings::value( const QString &key, QVariant default_value )
	{ return p ? p->value( key, default_value ) : QVariant(); }

inline void Settings::setValue( Settings::FieldType type, const QString &key, const QVariant &value )
	{ if( p ) p->setValue( type, key, value ); }

}

namespace qutim_sdk_0_3 {

typedef qutim_sdk_0_2::Settings Settings;

}

#endif // SETTINGS_H

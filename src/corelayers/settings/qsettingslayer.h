#ifndef QSETTINGSLAYER_H
#define QSETTINGSLAYER_H

#include "include/qutim/settings.h"
#include "include/qutim/layerinterface.h"
#include <QSettings>
#include <QDir>
#include <QStringList>

using namespace qutim_sdk_0_2;

class QSettingsInterface : public SettingsInterface
{
public:
	inline QSettingsInterface( const QString &organization, const QString &application )
		: m_settings(QSettings::defaultFormat(),QSettings::UserScope,organization,application) {}
	virtual void clear() { m_settings.clear(); }
	virtual void sync() { m_settings.sync(); }

	virtual void beginGroup( const QString &prefix ) { m_settings.beginGroup( prefix ); }
	virtual void endGroup() { m_settings.endGroup(); }
	virtual QString group() const { return m_settings.group(); }

	virtual int beginReadArray( const QString &prefix ) { return m_settings.beginReadArray( prefix ); }
	virtual void beginWriteArray( const QString &prefix, int size = -1 ) { m_settings.beginWriteArray( prefix, size ); }
	virtual void endArray() { m_settings.endArray(); }
	virtual void setArrayIndex( int i ) { m_settings.setArrayIndex( i ); }

	virtual QStringList allKeys() const { return m_settings.allKeys(); }
	virtual QStringList childKeys() const { return m_settings.childKeys(); }
	virtual QStringList childGroups() const { return m_settings.childGroups(); }
	virtual bool isWritable() const { return m_settings.isWritable(); }

	virtual QVariant value( const QString &key, QVariant def = QVariant() ) { return m_settings.value( key, def ); }
	virtual void setValue( Settings::FieldType type, const QString &key, const QVariant &value ) { Q_UNUSED(type); m_settings.setValue( key, value ); }
private:
	QSettings m_settings;
};

class QSettingsLayer : public SettingsLayerInterface
{
public:
    QSettingsLayer();
	virtual bool init(PluginSystemInterface *plugin_system)
	{
		m_name = "qutim";
		quint8 major, minor, secminor;
		quint16 svn;
		plugin_system->getQutimVersion(major, minor, secminor, svn);
		m_version = QString("%1.%2.%3 r%4").arg(major).arg(minor).arg(secminor).arg(svn);
		return true;
	}
	virtual void release() {}
	virtual void setProfileName(const QString &profile_name) { Q_UNUSED(profile_name); }
	virtual void setLayerInterface( LayerType, LayerInterface *) {}

	virtual void saveLayerSettings() {}
	virtual void removeLayerSettings() {}

	virtual void saveGuiSettingsPressed() {}
	virtual void removeGuiLayerSettings() {}

	virtual SettingsInterface *getSettings( const TreeModelItem &item );
	virtual SettingsInterface *getSettings( const QString &name );
	virtual bool logIn( const QString &profile, const QString &password );
	virtual QString getProfilePath();
	virtual QDir getProfileDir();
	inline void ensure_profile_path()
	{
		if( m_profile_path || SystemsCity::ProfileName().isEmpty() )
			return;
		QSettings settings( QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+SystemsCity::ProfileName(), "profilesettings" );
		QFileInfo config_dir = settings.fileName();
		QDir dir = qApp->applicationDirPath();
		if( config_dir.canonicalPath().contains( dir.canonicalPath() ) )
			m_profile_path = new QString( QDir::current().relativeFilePath( config_dir.absolutePath() ) );
		else
			m_profile_path = new QString( config_dir.absolutePath() );
	}

private:
	QString *m_profile_path;
};

#endif // QSETTINGSLAYER_H

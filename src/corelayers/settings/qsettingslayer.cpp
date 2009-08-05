#include "qsettingslayer.h"
#include <QDir>
#include <QFileInfo>

QSettingsLayer::QSettingsLayer()
{
	m_profile_path = 0;
}

SettingsInterface *QSettingsLayer::getSettings( const TreeModelItem &item )
{
	switch( item.m_item_type )
	{
	case 0:{
		QSettingsInterface *qset = new QSettingsInterface( "qutim/qutim."+SystemsCity::ProfileName()+"/"+item.m_protocol_name+"."+item.m_account_name,
									   "contactsettings" );
		qset->beginGroup( item.m_item_name );
		return qset;}
	case 1:{
		QSettingsInterface *qset = new QSettingsInterface( "qutim/qutim."+SystemsCity::ProfileName()+"/"+item.m_protocol_name+"."+item.m_account_name,
									   "groupsettings" );
		qset->beginGroup( item.m_item_name );
		return qset;}
	case 2:
		return new QSettingsInterface( "qutim/qutim."+SystemsCity::ProfileName()+"/"+item.m_protocol_name+"."+item.m_account_name,
									   "accountsettings" );
		break;
	case 32:{
		QSettingsInterface *qset = new QSettingsInterface( "qutim/qutim."+SystemsCity::ProfileName()+"/"+item.m_protocol_name+"."+item.m_account_name,
									   "conferencesettings" );
		qset->beginGroup( item.m_item_name );
		return qset;}
		break;
	default:
		Q_ASSERT_X( false, "QSettingsLayer::getSettings", "Unknown item type" ); // he-he, this will be segfault!
		return 0;
	}
}

SettingsInterface *QSettingsLayer::getSettings( const QString &name )
{
	static const QString profile( "qutim/qutim."+SystemsCity::ProfileName() );
	return new QSettingsInterface( profile, name );
}

bool QSettingsLayer::logIn( const QString &profile, const QString &password )
{
	return true;
}

QString QSettingsLayer::getProfilePath()
{
	ensure_profile_path();
	return m_profile_path ? *m_profile_path : QString();
}

QDir QSettingsLayer::getProfileDir()
{
	ensure_profile_path();
	return m_profile_path ? QDir( *m_profile_path ) : QDir();
}

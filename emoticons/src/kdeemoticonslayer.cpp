#include "kdeemoticonslayer.h"
#include <QUrl>

K_GLOBAL_STATIC(KEmoticons, s_self)


KdeEmoticonsLayer::KdeEmoticonsLayer()
{
	m_theme = self()->theme();
}

KEmoticons *KdeEmoticonsLayer::self()
{
	return s_self;
}

bool KdeEmoticonsLayer::init( PluginSystemInterface *plugin_system )
{
	Q_UNUSED(plugin_system);
	m_name = "kde";
	m_version = "0.1.0";
	return true;
}

void KdeEmoticonsLayer::setProfileName( const QString &profile_name )
{
	Q_UNUSED(profile_name);
}

void KdeEmoticonsLayer::saveGuiSettingsPressed()
{
	m_theme = self()->theme();
}

QHash<QString,QStringList> KdeEmoticonsLayer::getEmoticonsList()
{
	QHash<QString,QStringList> hash;
	
	QHashIterator<QString, QStringList> i(m_theme.emoticonsMap());
	int j=1;
	while (i.hasNext())
	{
		i.next();
		hash.insert(QString::number(j)+"|"+i.key(), i.value());
		j++;
	}
	return hash;
}   

void KdeEmoticonsLayer::checkMessageForEmoticons( QString &message )
{
	QList<KEmoticonsTheme::Token> tokens = m_theme.tokenize( message, KEmoticons::parseMode() | KEmoticonsTheme::SkipHTML );
	if ( tokens.isEmpty() && !message.isEmpty() )
		return;

	QString result;

	foreach( const KEmoticonsTheme::Token &token, tokens )
	{
		switch( token.type )
		{
		case KEmoticonsTheme::Text:
			result += token.text;
			break;
		case KEmoticonsTheme::Image:{
			QUrl url( token.picPath, QUrl::TolerantMode );
			if( url.scheme().isEmpty() )
			{
				url.setScheme( "file" );
				QString html = token.picHTMLCode;
				result += html.replace( token.picPath, url.toString() );
			}
			else
				result += token.picHTMLCode;
			break;}
		default:
			qWarning() << "Unknown token type. Something's broken.";
			break;
		}
	}

	message = result;
}

QString KdeEmoticonsLayer::getEmoticonsPath()
{
	return m_theme.themePath();
}

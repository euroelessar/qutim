#include "spellerlayerclass.h"

SpellerLayerClass::SpellerLayerClass()
{
}

bool SpellerLayerClass::init(PluginSystemInterface *plugin_system)
{
	m_name = "qutim";
	quint8 major, minor, secminor;
	quint16 svn;
	plugin_system->getQutimVersion(major, minor, secminor, svn);
	m_version = QString("%1.%2.%3 r%4").arg(major).arg(minor).arg(secminor).arg(svn);
	return true;
}

void SpellerLayerClass::release()
{
}

void SpellerLayerClass::setProfileName(const QString &profile_name)
{
	Q_UNUSED(profile_name);
}

void SpellerLayerClass::setLayerInterface( LayerType type, LayerInterface *layer_interface)
{
	Q_UNUSED(type);
	Q_UNUSED(layer_interface);
}

void SpellerLayerClass::saveLayerSettings()
{
}

QList<SettingsStructure> SpellerLayerClass::getLayerSettingsList()
{
	return m_settings;
}

void SpellerLayerClass::removeLayerSettings()
{
}


void SpellerLayerClass::saveGuiSettingsPressed()
{
}

QList<SettingsStructure> SpellerLayerClass::getGuiSettingsList()
{
	return m_gui_settings;
}

void SpellerLayerClass::removeGuiLayerSettings()
{
}


void SpellerLayerClass::startSpellCheck( QTextEdit *document )
{
#if 0
	if( m_highlighters.contains( document ) )
	{
		m_highlighters.value( document )->setActive( true );
	}
	else
	{
		SpellerHighlighter *highlighter = new SpellerHighlighter( document );
		connect( highlighter, SIGNAL(destroyed(QObject*)), this, SLOT(onDestruction(QObject*)) );
		m_highlighters.insert( document, highlighter );
	}
#else
	Q_UNUSED(document);
#endif
}

void SpellerLayerClass::stopSpellCheck( QTextEdit *document )
{
#if 0
	if( !m_highlighters.contains( document ) )
		return;
	m_highlighters.value( document )->setActive( false );
#else
	Q_UNUSED(document);
#endif
}

bool SpellerLayerClass::isCorrect( const QString &word ) const
{
#if 0
	return MacSpeller::instance().isCorrect( word );
#else
	return true;
#endif
}

bool SpellerLayerClass::isMisspelled( const QString &word ) const
{
#if 0
	return !MacSpeller::instance().isCorrect( word );
#else
	return false;
#endif
}

QStringList SpellerLayerClass::suggest( const QString &word ) const
{
#if 0
	return MacSpeller::instance().suggestions( word );
#else
	return QStringList() << word;
#endif
}

#if 0
void SpellerLayerClass::onDestruction( QObject *object )
{
	SpellerHighlighter *highlighter =  qobject_cast<SpellerHighlighter *>( object );
	if( highlighter )
		m_highlighters.remove( m_highlighters.key( highlighter ) );
}
#endif


#include "abstractthemeengine.h"

AbstractThemeEngine::AbstractThemeEngine()
{
	m_border_object = 0;
}

AbstractThemeEngine::~AbstractThemeEngine()
{
}

AbstractThemeEngine &AbstractThemeEngine::instance()
{
	static AbstractThemeEngine ate;
	return ate;
}

void AbstractThemeEngine::setCLBorder(QWidget *cl_widget)
{
	if ( m_border_object )
	{
		if( m_border_object->getWidget() == cl_widget )
			return;
		m_border_object->stopSkinning();
		delete m_border_object;
		m_border_object = 0;
	}
	m_border_object = new QSkinObject(cl_widget);
	m_border_object->loadSkinIni(m_border_theme_path + "/cl_border/");
	m_border_object->startSkinning();
}

void AbstractThemeEngine::stopSkiningCl()
{
	if ( m_border_object )
	{
		m_border_object->stopSkinning();
		delete m_border_object;
		m_border_object = 0;
	}
}

void AbstractThemeEngine::loadProfile(const QString &profile_name)
{
	m_profile_name = profile_name;
	reloadContent();
}

void AbstractThemeEngine::reloadContent()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");

#if defined (Q_OS_WIN32)
        m_border_theme_path = settings.value("gui/borders", ":/style/border/").toString();
#else
        m_border_theme_path = settings.value("gui/borders", "").toString();
#endif
}

#include "mobilesettingslayer.h"
#include "src/modulemanagerimpl.h"
#include "mobilesettingsdialog.h"
#include <libqutim/icon.h>

namespace Core
{

	namespace Mobile
	{

		static CoreModuleHelper<MobileSettingsLayer> mobile_settings_layer_static(
				QT_TRANSLATE_NOOP("Plugin", "Mobile Settings dialog"),
				QT_TRANSLATE_NOOP("Plugin", "qutIM settings dialog implementation for mobile devices")
				);

		void MobileSettingsLayer::show(const qutim_sdk_0_3::SettingsItemList& settings)
		{
			if (m_widget.isNull())
				m_widget =  new SettingsDialog(settings);
			m_widget->showMaximized();
		}
		void MobileSettingsLayer::close()
		{
			if (!m_widget.isNull())
				m_widget->deleteLater();
		}

		
	}

}

#include "xsettingslayerimpl.h"
#include "xsettingsdialog.h"
#include "src/modulemanagerimpl.h"
#include <libqutim/icon.h>

static Core::CoreModuleHelper<XSettingsLayerImpl> xsettings_layer_static(
		QT_TRANSLATE_NOOP("Plugin", "X Settings dialog"),
		QT_TRANSLATE_NOOP("Plugin", "Default qutIM settings dialog realization with OS X style top bar")
		);

XSettingsLayerImpl::XSettingsLayerImpl()
{
	//GeneralSettingsItem<XSettingsProperies> *item = new GeneralSettingsItem<XSettingsProperies>(Settings::General, Icon("preferences-other"), QT_TRANSLATE_NOOP("Settings","X Settings"));
	//Settings::registerItem(item);
}


XSettingsLayerImpl::~XSettingsLayerImpl()
{

}


void XSettingsLayerImpl::show (const SettingsItemList& settings )
{
	m_dialog =  new XSettingsDialog(settings);
	m_dialog->show();
}


void XSettingsLayerImpl::close()
{
	m_dialog->deleteLater();
}

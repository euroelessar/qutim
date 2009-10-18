#include "xsettingslayerimpl.h"
#include "xsettingsdialog.h"
#include "src/modulemanagerimpl.h"

static Core::CoreModuleHelper<XSettingsLayerImpl> xsettings_layer_static(
		QT_TRANSLATE_NOOP("Plugin", "X Settings dialog"),
		QT_TRANSLATE_NOOP("Plugin", "Default qutIM settings dialog realization with OS X style top bar")
		);

XSettingsLayerImpl::XSettingsLayerImpl()
{

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

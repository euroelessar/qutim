#include "xsettingslayerimpl.h"
#include "xsettingsdialog.h"
#include "src/modulemanagerimpl.h"
#include <libqutim/icon.h>

static Core::CoreModuleHelper<XSettingsLayerImpl> xsettings_layer_static(
		QT_TRANSLATE_NOOP("Plugin", "X Settings dialog"),
		QT_TRANSLATE_NOOP("Plugin", "Default qutIM settings dialog realization with OS X style top bar")
		);

XSettingsLayerImpl::XSettingsLayerImpl()
:	m_dialog(0)
{

}


XSettingsLayerImpl::~XSettingsLayerImpl()
{

}


void XSettingsLayerImpl::show (const SettingsItemList& settings )
{
	if (m_dialog == 0)
		m_dialog =  new XSettingsDialog(settings);
	m_dialog->show();
}


void XSettingsLayerImpl::close()
{
	m_dialog->deleteLater();
}

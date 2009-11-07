#include "settingslayerimpl.h"
#include "settingsdialog.h"
#include "modulemanagerimpl.h"
#include <qutim/icon.h>

static Core::CoreModuleHelper<SettingsLayerImpl> settings_layer_static( //при помощи данной функции мы сообщаем менеджеру модулей о том, что перед ним лежит модуль
		QT_TRANSLATE_NOOP("Plugin", "Simple Settings dialog"), //описание, макрос QT_TRANSLATE_NOOP позволяет делать его переводимым на разные языки
		QT_TRANSLATE_NOOP("Plugin", "SDK03 example")
		);

void SettingsLayerImpl::show (const SettingsItemList& settings )
{
	if (m_dialog.isNull())
		m_dialog =  new SettingsDialog(settings); //создаем диалог, которому передаем наш список настроек
	m_dialog->show();
}


void SettingsLayerImpl::close()
{
	m_dialog->deleteLater();
}


SettingsLayerImpl::SettingsLayerImpl()
{
  
}


SettingsLayerImpl::~SettingsLayerImpl()
{
  
}
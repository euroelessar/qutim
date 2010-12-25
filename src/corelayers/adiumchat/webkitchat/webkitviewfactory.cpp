#include "webkitviewfactory.h"
#include "webkitchatviewwidget.h"
#include "chatstyleoutput.h"
#include "settings/chatappearance.h"
#include <qutim/settingslayer.h>
#include <qutim/servicemanager.h>
#include <qutim/icon.h>


namespace Core
{
namespace AdiumChat
{

WebkitViewFactory::WebkitViewFactory()
{
	m_appearanceSettings = new GeneralSettingsItem<ChatAppearance>(Settings::Appearance, Icon("view-choose"),
																   QT_TRANSLATE_NOOP("Settings","Chat"));

	//FIXME move to chatstyleoutput
	QObject *obj = ServiceManager::getByName("ChatForm");
	if (!obj) {
		return;
	}
	Settings::registerItem(m_appearanceSettings);
}

WebkitViewFactory::~WebkitViewFactory()
{
	Settings::removeItem(m_appearanceSettings);
	delete m_appearanceSettings;
}

QWidget *WebkitViewFactory::createViewWidget()
{
	return new WebkitChatViewWidget();
}
QObject *WebkitViewFactory::createViewController()
{
	ChatStyleOutput *output = new ChatStyleOutput();
	m_appearanceSettings->connect(SIGNAL(saved()),output,SLOT(loadSettings()));
	return output;
}

}
}

#include "tabbedchatform.h"
#include "tabbedchatwidget.h"
#include <qutim/settingslayer.h>
#include "settings/chatbehavior.h"
#include <qutim/icon.h>
#include <qutim/shortcut.h>

namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;

TabbedChatForm::TabbedChatForm() :
	AbstractChatForm(),
	m_settingsItem(0)
{
	m_settingsItem = new GeneralSettingsItem<ChatBehavior>(Settings::General, Icon("view-choose"),
															  QT_TRANSLATE_NOOP("Settings","Chat"));
	m_settingsItem->connect(SIGNAL(saved()),this,SLOT(onSettingsChanged()));
	Settings::registerItem(m_settingsItem);
	Shortcut::registerSequence("chatListSession",
							   QT_TRANSLATE_NOOP("ChatLayer", "Open session list"),
							   "ChatWidget",
							   QKeySequence("Alt+S")
							   );
}

TabbedChatForm::~TabbedChatForm()
{
	Settings::removeItem(m_settingsItem);
	delete m_settingsItem;
}

AbstractChatWidget *TabbedChatForm::createWidget(const QString &key)
{
	return new TabbedChatWidget(key);
}

}
}

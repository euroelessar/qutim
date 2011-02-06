#include "stackedchatform.h"
#include "stackedchatwidget.h"
#include <qutim/settingslayer.h>
#include "settings/stackedchatbehavior.h"
#include <qutim/icon.h>
#include <qutim/shortcut.h>

namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;

StackedChatForm::StackedChatForm() :
	AbstractChatForm(),
	m_settingsItem(0)
{
	m_settingsItem = new GeneralSettingsItem<StackedChatBehavior>(Settings::General, Icon("view-choose"),
															  QT_TRANSLATE_NOOP("Settings","Chat"));
	m_settingsItem->connect(SIGNAL(saved()),this,SLOT(onSettingsChanged()));
	Settings::registerItem(m_settingsItem);
	Shortcut::registerSequence("chatListSession",
							   QT_TRANSLATE_NOOP("ChatLayer", "Open session list"),
							   "ChatWidget",
							   QKeySequence("Alt+S")
							   );
}

StackedChatForm::~StackedChatForm()
{
	Settings::removeItem(m_settingsItem);
	delete m_settingsItem;
}

AbstractChatWidget *StackedChatForm::createWidget(const QString &key)
{
	return new StackedChatWidget(key);
}

}
}

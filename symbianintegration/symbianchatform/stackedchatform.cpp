#include "stackedchatform.h"
#include "stackedchatwidget.h"
#include <qutim/settingslayer.h>
#include "settings/stackedchatbehavior.h"
#include <qutim/icon.h>
#include <qutim/shortcut.h>
#include <qutim/systemintegration.h>

namespace Core
{
namespace AdiumChat
{
namespace Symbian
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

	SystemIntegration::show(createWidget(QString())); //nice hack
}

StackedChatForm::~StackedChatForm()
{
	Settings::removeItem(m_settingsItem);
	delete m_settingsItem;
}

AbstractChatWidget *StackedChatForm::createWidget(const QString &key)
{
	Q_UNUSED(key)
	if(!m_chatWidget)
		m_chatWidget = new StackedChatWidget();
	return m_chatWidget;
}

} // namespace Symbian
} // namespace AdiumChat
} // namespace Core

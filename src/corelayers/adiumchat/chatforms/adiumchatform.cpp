#include "adiumchatform.h"
#include "modulemanagerimpl.h"
#include "adiumchatwidget.h"

namespace Core
{
	namespace AdiumChat
	{
		using namespace qutim_sdk_0_3;

		static CoreModuleHelper<AdiumChatForm> classic_chatform_static(
				QT_TRANSLATE_NOOP("Plugin", "Adium"),
				QT_TRANSLATE_NOOP("Plugin", "Adium-like chatform implementation for adiumchat")
				);
		
		AdiumChatForm::AdiumChatForm()
		{
		}
		
		AdiumChatForm::~AdiumChatForm()
		{
		}

		AbstractChatWidget *AdiumChatForm::createWidget(const QString &key, bool removeSessionOnClose)
		{
			return new AdiumChatWidget(key, removeSessionOnClose);
		}
	}
}

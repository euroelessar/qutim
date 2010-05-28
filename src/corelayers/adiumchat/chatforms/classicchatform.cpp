#include "classicchatform.h"
#include "modulemanagerimpl.h"
#include "classicchatwidget.h"

namespace Core
{
	namespace AdiumChat
	{
		using namespace qutim_sdk_0_3;

		static CoreModuleHelper<ClassicChatForm> classic_chatform_static(
				QT_TRANSLATE_NOOP("Plugin", "Classic"),
				QT_TRANSLATE_NOOP("Plugin", "Default qutIM chatform implementation for adiumchat")
				);
		
		ClassicChatForm::ClassicChatForm()
		{
		}

		ClassicChatForm::~ClassicChatForm()
		{
		}
		
		AbstractChatWidget *ClassicChatForm::createWidget(const QString &key, bool removeSessionOnClose)
		{
			return new ClassicChatWidget(key, removeSessionOnClose);
		}
	}
}

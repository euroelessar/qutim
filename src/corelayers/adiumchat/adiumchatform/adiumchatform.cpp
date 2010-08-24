#include "adiumchatform.h"
#include "adiumchatwidget.h"

namespace Core
{
	namespace AdiumChat
	{
		using namespace qutim_sdk_0_3;
		
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

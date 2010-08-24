#include "classicchatform.h"
#include "classicchatwidget.h"

namespace Core
{
	namespace AdiumChat
	{
		using namespace qutim_sdk_0_3;
		
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

#ifndef ADIUMCHATFORM_H
#define ADIUMCHATFORM_H

#include "../chatlayer/chatforms/abstractchatform.h"

namespace Core
{
	namespace AdiumChat
	{
		class AdiumChatForm : public AbstractChatForm
		{
			Q_OBJECT
		public:
			explicit AdiumChatForm();
			~AdiumChatForm();
		protected:
			virtual AbstractChatWidget *createWidget(const QString &key, bool removeSessionOnClose);
		};

	}
}

#endif //ADIUMCHATFORM_H

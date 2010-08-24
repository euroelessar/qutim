#ifndef CLASSICCHATFORM_H
#define CLASSICCHATFORM_H

#include "../chatlayer/chatforms/abstractchatform.h"

namespace Core
{
	namespace AdiumChat
	{
		class ClassicChatForm : public AbstractChatForm
		{
			Q_OBJECT
		public:
			explicit ClassicChatForm();
			~ClassicChatForm();
		protected:
			virtual AbstractChatWidget *createWidget(const QString &key, bool removeSessionOnClose);
		};
	}
}

#endif // CLASSICCHATFORM_H

#ifndef CHATLAYERIMPL_H
#define CHATLAYERIMPL_H
#include <libqutim/messagesession.h>

using namespace qutim_sdk_0_3;

namespace AdiumChat
{

	class ChatLayerImpl : public ChatLayer
	{
		Q_OBJECT
	public:
		virtual ChatSession* getSession ( Account* acc, const QString& id, bool create = true );
		virtual QList< ChatSession* > sessions();
		ChatLayerImpl();
	};

}
#endif // CHATLAYERIMPL_H

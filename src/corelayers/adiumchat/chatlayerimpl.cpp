#include "chatlayerimpl.h"
#include <libqutim/account.h>

namespace AdiumChat

{

	ChatLayerImpl::ChatLayerImpl()
	{

	}


	ChatSession* ChatLayerImpl::getSession ( Account* acc, const QString& id, bool create )
	{

	}


	QList<ChatSession* > ChatLayerImpl::sessions()
	{
		QList<ChatSession* >  list;
		foreach (ChatSessionHash hash, m_chat_sessions)
		{
			foreach (ChatSession *item, hash )
			{
				list.append(item);
			}
		}
		return list;
	}

}
#include "chatviewfactory.h"
#include <qutim/servicemanager.h>

namespace Core
{

namespace AdiumChat
{

using namespace qutim_sdk_0_3;
ChatViewFactory *ChatViewFactory::instance()
{
	ChatViewFactory *f = ServiceManager::getByName<ChatViewFactory*>("ChatViewFactory");
	Q_ASSERT(f);
	return f;
}

}

}

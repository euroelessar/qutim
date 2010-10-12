#ifndef WEBKITVIEWFACTORY_H
#define WEBKITVIEWFACTORY_H
#include "../chatlayer/chatviewfactory.h"

namespace qutim_sdk_0_3
{
class SettingsItem;
}

namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;

class WebkitViewFactory : public ChatViewFactory
{
public:
	WebkitViewFactory();
	virtual ~WebkitViewFactory();
	virtual ChatViewWidget *createViewWidget();
	virtual ChatViewController *createViewController();
private:
	SettingsItem *m_appearanceSettings;
};

}
}


#endif // WEBKITVIEWFACTORY_H

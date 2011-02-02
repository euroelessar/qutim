#ifndef WEBKITVIEWFACTORY_H
#define WEBKITVIEWFACTORY_H
#include <chatlayer/chatviewfactory.h>

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
	Q_OBJECT
	Q_CLASSINFO("Uses", "SettingsLayer")
public:
	WebkitViewFactory();
	virtual ~WebkitViewFactory();
	virtual QWidget *createViewWidget();
	virtual QObject *createViewController();
private:
	SettingsItem *m_appearanceSettings;
};

}
}


#endif // WEBKITVIEWFACTORY_H

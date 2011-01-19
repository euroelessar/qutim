#ifndef CHATVIEWFACTORY_H
#define CHATVIEWFACTORY_H

#include <QWidget>
#include "chatlayer_global.h"

namespace qutim_sdk_0_3
{
class ChatUnit;
class Message;
}

namespace Core
{
namespace AdiumChat
{

class ChatSessionImpl;
class ChatViewWidget;
class ChatViewController;
class ADIUMCHAT_EXPORT ChatViewFactory : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "ChatViewFactory")
	Q_CLASSINFO("Uses", "ChatForm")
public:
	virtual ~ChatViewFactory() {}
	virtual QWidget *createViewWidget() = 0;
	virtual QObject *createViewController() = 0;
	static ChatViewFactory *instance();
};

class ChatViewWidget
{
public:
	virtual ~ChatViewWidget() {}
	virtual void setViewController(QObject *controller) = 0;
};

class ChatViewController
{
public:
	virtual ~ChatViewController() {}
	virtual void setChatSession(ChatSessionImpl *session) = 0;
	virtual ChatSessionImpl *getSession() const = 0;
	virtual void appendMessage(const qutim_sdk_0_3::Message &msg) = 0;
	virtual void clear() {}
};

}
}

Q_DECLARE_INTERFACE(Core::AdiumChat::ChatViewWidget, "org.qutim.core.ChatViewWidget")
Q_DECLARE_INTERFACE(Core::AdiumChat::ChatViewController, "org.qutim.core.ChatViewController")

#endif // CHATVIEWFACTORY_H

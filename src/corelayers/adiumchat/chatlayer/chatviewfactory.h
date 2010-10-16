#ifndef CHATVIEWFACTORY_H
#define CHATVIEWFACTORY_H

#include <QWidget>

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
class ChatViewFactory : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "ChatViewFactory")
	Q_CLASSINFO("Uses", "ChatForm")
public:
	virtual ~ChatViewFactory() {}
	virtual ChatViewWidget *createViewWidget() = 0;
	virtual ChatViewController *createViewController() = 0;
};

class ChatViewWidget /*: public QWidget*/
{
public:
	virtual ~ChatViewWidget() {}
	virtual void setViewController(ChatViewController *controller) = 0;
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
#endif // CHATVIEWFACTORY_H

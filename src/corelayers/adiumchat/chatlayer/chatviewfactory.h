#ifndef CHATVIEWFACTORY_H
#define CHATVIEWFACTORY_H

class QWidget;
class QObject;

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
class ChatViewFactory
{
public:
	virtual ~ChatViewFactory() {}

	virtual QWidget *createViewWidget() = 0;
	virtual QObject *createViewController() = 0;
};

class ChatViewController;
class ChatViewWidget
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
};

}
}
#endif // CHATVIEWFACTORY_H

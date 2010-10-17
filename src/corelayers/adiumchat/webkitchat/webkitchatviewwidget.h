#ifndef WEBKITCHATVIEWWIDGET_H
#define WEBKITCHATVIEWWIDGET_H
#include <chatlayer/chatviewfactory.h>
#include <QWebView>

namespace Core
{
namespace AdiumChat
{

class WebkitChatViewWidget : public QWebView, public ChatViewWidget
{
public:
    WebkitChatViewWidget();
	virtual void setViewController(ChatViewController *controller);
};

}
}
#endif // WEBKITCHATVIEWWIDGET_H

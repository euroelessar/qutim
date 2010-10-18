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
	Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatViewWidget)
public:
    WebkitChatViewWidget();
	virtual void setViewController(QObject *controller);
};

}
}
#endif // WEBKITCHATVIEWWIDGET_H

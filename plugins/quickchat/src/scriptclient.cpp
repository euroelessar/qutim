#include "scriptclient.h"
#include "chatcontroller.h"
#include "websockettransport.h"

namespace QuickChat {

ScriptClient::ScriptClient() :
    m_server(QStringLiteral("qutIM QuickChat"), QWebSocketServer::NonSecureMode),
    m_clientWrapper(&m_server)
{
    if (!m_server.listen(QHostAddress::LocalHost)) {
        qFatal("Failed to create WebSocket server: %s", qPrintable(m_server.errorString()));
        return;
    }

    QObject::connect(&m_clientWrapper, &WebSocketClientWrapper::clientConnected,
                     &m_channel, &QWebChannel::connectTo);

    m_channel.registerObject(QStringLiteral("client"), this);
}

QSharedPointer<ScriptClient> ScriptClient::instance()
{
    static QWeakPointer<ScriptClient> self;
    if (auto shared = self.toStrongRef())
        return shared;

    auto shared = QSharedPointer<ScriptClient>::create();
    self = shared.toWeakRef();
    return shared;
}

void ScriptClient::addController(const QString &id, ChatController *controller)
{
    m_controllers.insert(id, controller);
}

void ScriptClient::removeController(const QString &id)
{
    m_controllers.remove(id);
}

QUrl ScriptClient::serverUrl() const
{
    return m_server.serverUrl();
}

void ScriptClient::debugLog(const QString &id, const QString &message)
{
    if (ChatController *channel = find(id))
        channel->debugLog(message);
}

void ScriptClient::appendNick(const QString &id, const QString &nick)
{
    if (ChatController *channel = find(id))
        channel->appendNick(nick);
}

void ScriptClient::contextMenu(const QString &id, const QString &nick)
{
    if (ChatController *channel = find(id))
        channel->contextMenu(nick);
}

void ScriptClient::appendText(const QString &id, const QString &text)
{
    if (ChatController *channel = find(id))
        channel->appendText(text);
}

void ScriptClient::setTopic(const QString &id, const QString &topic)
{
    if (ChatController *channel = find(id))
        channel->setTopic(topic);
}

ChatController *ScriptClient::find(const QString &id) const
{
    return m_controllers.value(id);
}

} // namespace QuickChat

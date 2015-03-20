#ifndef QUICKCHAT_SCRIPTCLIENT_H
#define QUICKCHAT_SCRIPTCLIENT_H

#include "websocketclientwrapper.h"
#include <QWebSocketServer>
#include <QWebChannel>

namespace QuickChat {

class ChatController;

class ScriptClient : public QObject
{
    Q_OBJECT
public:
    static QSharedPointer<ScriptClient> instance();

    void addController(const QString &id, ChatController *controller);
    void removeController(const QString &id);
    QUrl serverUrl() const;

public slots:
    void debugLog(const QString &id, const QString &message);
    void appendNick(const QString &id, const QString &nick);
    void contextMenu(const QString &id, const QString &nick);
    void appendText(const QString &id, const QString &text);
    void setTopic(const QString &id, const QString &topic);

protected:
    explicit ScriptClient();
    ChatController *find(const QString &id) const;

    friend class QSharedPointer<ScriptClient>;

private:
    QWebSocketServer m_server;
    WebSocketClientWrapper m_clientWrapper;
    QWebChannel m_channel;
    QHash<QString, ChatController*> m_controllers;
};

} // namespace QuickChat

#endif // QUICKCHAT_SCRIPTCLIENT_H

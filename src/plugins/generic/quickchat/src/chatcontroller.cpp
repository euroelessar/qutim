#include "chatcontroller.h"
#include <qutim/utils.h>
#include <qutim/message.h>
#include <qutim/thememanager.h>
#include <qutim/debug.h>
#include <qutim/account.h>
#include <qutim/conference.h>
#include <qutim/history.h>
#include <qutim/emoticons.h>
#include <qutim/notification.h>
#include <qutim/servicemanager.h>
#include <qutim/chatsession.h>
#include <QUuid>

namespace QuickChat
{
using namespace qutim_sdk_0_3;

ChatController::ChatController()
	: m_session(nullptr)
{
	m_id = QUuid::createUuid().toString();
	m_client = ScriptClient::instance();
	m_client->addController(m_id, this);
}

ChatController::~ChatController()
{
	m_client->removeController(m_id);
}

QString ChatController::id() const
{
	return m_id;
}

QString ChatController::fontFamily() const
{
	return m_fontFamily;
}

int ChatController::fontSize() const
{
	return m_fontSize;
}

ChatChannel *ChatController::session() const
{
	return m_session;
}

void ChatController::setSession(ChatChannel *session)
{
	if (m_session == session)
		return;

	if (m_session) {
		disconnect(m_session, nullptr, this, nullptr);
		m_session->removeEventFilter(this);
	}

	m_session = session;

	if (session) {
		session->installEventFilter(this);
		connect(session, &ChatChannel::messageAppended, this, &ChatController::onMessageAppended);
	}

	emit sessionChanged(session);

	loadSettings(false);
	clearChat();
	loadHistory();
}

bool ChatController::isContentSimiliar(const Message &a, const Message &b)
{
	int flags = 0;
	if (!m_style.hasAction())
		flags |= Message::IgnoreActions;
	return a.isSimiliar(b, flags);
}

void ChatController::onMessageAppended(const qutim_sdk_0_3::Message &msg)
{
	Message copy = msg;
	QString html = UrlParser::parseUrls(copy.html(), UrlParser::Html);
	copy.setProperty("messageId", msg.id());
	if (msg.property("topic", false)) {
		copy.setHtml(html);
		m_topic = copy;
//        if (!m_isLoading)
//            updateTopic();
		return;
	}
	if (msg.property("firstFocus", false))
		clearFocusClass();
	// We don't want emoticons in topic
	html = Emoticons::theme().parseEmoticons(html);
	copy.setHtml(html);
	bool similiar = isContentSimiliar(m_last, msg);
	QString script = m_style.scriptForAppendingContent(copy, similiar, false, false);
	m_last = msg;
	evaluateJavaScript(script);
}

void ChatController::clearChat()
{
	Q_ASSERT(m_session);
	m_last = Message();
	m_isLoading = true;

	QString serverUrl = m_client->serverUrl().toString();
	QString html = m_style.baseTemplateForChat(m_session, m_id, serverUrl);

	emit htmlRequested(html, m_style.baseUrl());
	evaluateJavaScript(m_style.scriptForSettingCustomStyle());
	evaluateJavaScript(scriptForFontUpdate());
}

WebKitMessageViewStyle *ChatController::style()
{
	return &m_style;
}

bool ChatController::eventFilter(QObject *obj, QEvent *ev)
{
	if (obj == m_session && ev->type() == MessageReceiptEvent::eventType()) {
		MessageReceiptEvent *msgEvent = static_cast<MessageReceiptEvent *>(ev);
		evaluateJavaScript(QStringLiteral("client.markMessage(%1, %2);")
						   .arg(QString::number(msgEvent->id()),
								msgEvent->success() ? QStringLiteral("true") : QStringLiteral("false"))
						   );
		return true;
	}
	return QObject::eventFilter(obj, ev);
}

void ChatController::evaluateJavaScript(const QString &script)
{
	emit javaScriptRequested(script);
}

void ChatController::debugLog(const QString &message)
{
	qDebug() << "WebKit: " << message;
}

void ChatController::appendNick(const QString &nick)
{
	qDebug() << "appendNick: " << nick;
	emit appendNickRequested(nick);
}

void ChatController::contextMenu(const QString &nick)
{
//    emit menuRequested(nick);
	foreach (ChatUnit *unit, m_session->unit()->lowerUnits()) {
		if (Buddy *buddy = qobject_cast<Buddy*>(unit)) {
			if (buddy->name() == nick) {
				emit menuRequested(buddy);
			}
		}
	}
}

void ChatController::appendText(const QString &text)
{
	qDebug() << "appendText: " << text;
	emit appendTextRequested(text);
}

void ChatController::setTopic(const QString &topic)
{
	if (Conference *conference = qobject_cast<Conference*>(m_session->unit()))
		conference->setTopic(topic);
}

void ChatController::clearFocusClass()
{
	evaluateJavaScript(QStringLiteral("client.clearFocusClass()"));
}

void ChatController::loadSettings(bool onFly)
{
	Config config(QLatin1String("appearance/adiumChat"));
	config.beginGroup(QLatin1String("style"));
	if (!onFly) {
		m_styleName = config.value(QLatin1String("name"), QLatin1String("default"));
		m_style.setStylePath(ThemeManager::path(QLatin1String("webkitstyle"), m_styleName));
		m_style.setShowUserIcons(config.value(QLatin1String("showUserIcons"), true));
		m_style.setShowHeader(config.value(QLatin1String("showHeader"), true));
	}
	config.beginGroup(m_styleName);
	QString variant = config.value(QLatin1String("variant"), m_style.defaultVariant());
	m_style.setActiveVariant(variant);
	m_style.setCustomBackgroundType(config.value(QLatin1String("backgroundType"),
												 WebKitMessageViewStyle::BackgroundNormal));
	if (config.value(QLatin1String("customBackground"), false)) {
		m_style.setCustomBackgroundPath(config.value(QLatin1String("backgroundPath"), QString()));
		m_style.setCustomBackgroundColor(config.value(QLatin1String("backgroundColor"), QColor()));
	}
	QString fontFamily = config.value(QLatin1String("fontFamily"), m_style.defaultFontFamily());
	int fontSize = config.value(QLatin1String("fontSize"), m_style.defaultFontSize());
	setDefaultFont(fontFamily, fontSize);

	QString css;
	QString customFile = m_style.pathForResource(QLatin1String("Custom.json"));
	if (!customFile.isEmpty()) {
		QVariantList values = config.value(QLatin1String("customStyle"), QVariantList());
		Config variables(customFile);
		const int count = variables.arraySize();
		for (int index = 0; index < count; index++) {
			variables.setArrayIndex(index);
			css.append(QString::fromLatin1("%1 { %2: %3; } ")
					   .arg(variables.value(QLatin1String("selector"), QString()))
					   .arg(variables.value(QLatin1String("parameter"), QString()))
					   .arg(values.value(index, variables.value(QLatin1String("value"))).toString()));
		}
	}
	m_style.setCustomStyle(css);
}

void ChatController::onSettingsSaved()
{
	loadSettings(true);
	evaluateJavaScript(m_style.scriptForChangingVariant());
	evaluateJavaScript(m_style.scriptForSettingCustomStyle());
	evaluateJavaScript(scriptForFontUpdate());
}

void ChatController::loadHistory()
{
	Config config = Config(QLatin1String("appearance")).group(QLatin1String("chat/history"));
	int max_num = config.value(QLatin1String("maxDisplayMessages"), 5);
	MessageList messages = History::instance()->readSync(m_session->unit(), max_num);
	foreach (Message mess, messages) {
		mess.setProperty("silent", true);
		mess.setProperty("store", false);
		mess.setProperty("history", true);
		if (!mess.chatUnit()) //TODO FIXME
			mess.setChatUnit(m_session->unit());
		m_session->append(mess);
	}
}

QString ChatController::scriptForFontUpdate()
{
	return QStringLiteral("client.setDefaultFont(\"%1\", %2);")
			.arg(m_fontFamily, QString::number(m_fontSize));
}

void ChatController::setDefaultFont(const QString &family, int size)
{
	QFontInfo info(QFont(family, size));
	size = info.pixelSize();
	if (m_fontFamily != family) {
		m_fontFamily = family;
		emit fontFamilyChanged(family);
	}
	if (m_fontSize != size) {
		m_fontSize = size;
		emit fontSizeChanged(size);
	}
}

} // namespace QuickChat

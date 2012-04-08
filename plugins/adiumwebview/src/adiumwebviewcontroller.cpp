/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "adiumwebviewcontroller.h"
#include "../lib/webkitnetworkaccessmanager.h"
#include <qutim/utils.h>
#include <qutim/message.h>
#include <qutim/adiumchat/chatsessionimpl.h>
#include <qutim/thememanager.h>
#include <qutim/debug.h>
#include <qutim/account.h>
#include <qutim/conference.h>
#include <qutim/history.h>
#include <qutim/emoticons.h>
#include <qutim/notification.h>
#include <qutim/servicemanager.h>
#include <qutim/adiumchat/abstractchatform.h>
#include <qutim/adiumchat/chatlayerimpl.h>
#include <QPlainTextEdit>
#include <QWebFrame>
#include <QWebInspector>
#include <QWebElement>
#include <QDesktopServices>

namespace Adium {

using namespace qutim_sdk_0_3;
using namespace Core::AdiumChat;

WebViewLoaderLoop::WebViewLoaderLoop()
{
}

WebViewLoaderLoop::~WebViewLoaderLoop()
{
}

void WebViewLoaderLoop::addPage(QWebPage *page, const QString &html)
{
	for (int i = 0; i < m_pages.size(); ++i) {
		if (m_pages.at(i).data() == page) {
			m_htmls[i] = html;
			return;
		}
	}
	connect(page, SIGNAL(loadFinished(bool)), SLOT(onPageLoaded()));
	connect(page, SIGNAL(destroyed()), SLOT(onPageDestroyed()));
	m_pages.append(page);
	m_htmls.append(html);
	if (m_pages.size() == 1)
		page->mainFrame()->setHtml(html);
}

void WebViewLoaderLoop::onPageLoaded()
{
	disconnect(m_pages.first().data(), 0, this, 0);
	m_pages.removeFirst();
	m_htmls.removeFirst();
	if (!m_pages.isEmpty()) {
		QWebPage *page = m_pages.first().data();
		QString html = m_htmls.first();
		page->mainFrame()->setHtml(html);
	}
}

void WebViewLoaderLoop::onPageDestroyed()
{
	for (int i = 0; i < m_pages.size(); ++i) {
		if (m_pages.at(i).isNull()) {
			m_pages.removeAt(i);
			m_htmls.removeAt(i);
			--i;
		}
	}
}

Q_GLOBAL_STATIC(WebViewLoaderLoop, loaderLoop)

WebViewController::WebViewController(bool isPreview) :
	m_isLoading(false), m_isPreview(isPreview)
{
	m_topic.setProperty("topic", true);
	setNetworkAccessManager(new WebKitNetworkAccessManager(this));
	
	setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
	QWebInspector *inspector = new QWebInspector;
	inspector->setPage(this);
	connect(this, SIGNAL(destroyed()), inspector, SLOT(deleteLater()));
	connect(this, SIGNAL(loadFinished(bool)), SLOT(onLoadFinished()));
	connect(this, SIGNAL(linkClicked(QUrl)), SLOT(onLinkClicked(QUrl)));
	onObjectCleared();
	connect(mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), SLOT(onObjectCleared()));
}

WebViewController::~WebViewController()
{
}

void WebViewController::setChatSession(ChatSessionImpl* session)
{
	if (m_session.data() == session)
		return;
	
	m_session = session;
	m_session.data()->installEventFilter(this);
	if (qobject_cast<Conference*>(m_session.data()->unit())) {
		connect(m_session.data()->unit(), SIGNAL(topicChanged(QString,QString)),
		        this, SLOT(onTopicChanged(QString)));
	}
	if (!m_isPreview) {
		loadSettings(false);
		clearChat();
		loadHistory();
	}
}

ChatSessionImpl *WebViewController::getSession() const
{
	return static_cast<ChatSessionImpl *>(m_session.data());
}

bool WebViewController::isContentSimiliar(const Message &a, const Message &b)
{
	bool aIsAction = a.html().startsWith(QLatin1String("/me "), Qt::CaseInsensitive);
	bool bIsAction = b.html().startsWith(QLatin1String("/me "), Qt::CaseInsensitive);
	if (a.chatUnit() == b.chatUnit()
	        && (!m_style.hasAction() || (!aIsAction && !bIsAction))
	        && a.isIncoming() == b.isIncoming()
	        && a.property("senderName", QString()) == b.property("senderName", QString())
	        && a.property("service", false) == b.property("service", false)
	        && a.property("history", false) == b.property("history", false)
	        && a.property("mention", false) == b.property("mention", false)) {
		return qAbs(a.time().secsTo(b.time())) < 300;
	}
	return false;
}

void WebViewController::appendMessage(const qutim_sdk_0_3::Message &msg)
{
	Message copy = msg;
	QString html = UrlParser::parseUrls(copy.html());
	copy.setProperty("messageId", msg.id());
	if (msg.property("topic", false)) {
		copy.setHtml(html);
		m_topic = copy;
		if (!m_isLoading)
			updateTopic();
		return;
	}
	// We don't want emoticons in topic
	html = Emoticons::theme().parseEmoticons(html);
	copy.setHtml(html);
	bool similiar = isContentSimiliar(m_last, msg);
	QString script = m_style.scriptForAppendingContent(copy, similiar, false, false);
	m_last = msg;
	evaluateJavaScript(script);
}

void WebViewController::clearChat()
{
	Q_ASSERT(!m_session.isNull());
	m_last = Message();
	m_isLoading = true;
	loaderLoop()->addPage(this, m_style.baseTemplateForChat(m_session.data()));
	evaluateJavaScript(m_style.scriptForSettingCustomStyle());
}

QString WebViewController::quote()
{
	QString quote = selectedText();
	if (quote.isEmpty())
		quote = m_last.text();
	return quote;
}

WebKitMessageViewStyle *WebViewController::style()
{
	return &m_style;
}

void WebViewController::setDefaultFont(const QString &family, int size)
{
	QWebSettings *settings = QWebPage::settings();
	QFontInfo info(QFont(family, size));
	QWebSettings::FontFamily families[] = {
	    QWebSettings::StandardFont,
        QWebSettings::FixedFont,
        QWebSettings::SerifFont,
        QWebSettings::SansSerifFont
	};
	const int familiesSize = sizeof(families) / sizeof(families[0]);
	if (family.isEmpty()) {
		for (int i = 0; i < familiesSize; ++i)
			settings->resetFontFamily(families[i]);
	} else {
		for (int i = 0; i < familiesSize; ++i)
			settings->setFontFamily(families[i], family);
	}
	if (size < 0)
		settings->resetFontSize(QWebSettings::DefaultFontSize);
	else
		settings->setFontSize(QWebSettings::DefaultFontSize, info.pixelSize());
}

QString WebViewController::defaultFontFamily() const
{
	return QWebPage::settings()->fontFamily(QWebSettings::StandardFont);
}

int WebViewController::defaultFontSize() const
{
	QWebSettings *settings = QWebPage::settings();
	QString family = settings->fontFamily(QWebSettings::StandardFont);
	int size = settings->fontSize(QWebSettings::DefaultFontSize);
	QFont font(family);
	font.setPixelSize(size);
	QFontInfo info(font);
	return info.pointSize();
}

bool WebViewController::eventFilter(QObject *obj, QEvent *ev)
{
	if (obj == m_session.data() && ev->type() == MessageReceiptEvent::eventType()) {
		MessageReceiptEvent *msgEvent = static_cast<MessageReceiptEvent *>(ev);
		QWebFrame *frame = mainFrame();
		QWebElement elem = frame->findFirstElement(QLatin1String("#message")
		                                           + QString::number(msgEvent->id()));
		if (!elem.isNull()) {
			if (msgEvent->success()) {
				elem.removeClass(QLatin1String("notDelivered"));
				elem.addClass(QLatin1String("delivered"));
			} else {
				elem.addClass(QLatin1String("failedToDeliver"));
			}
		}
		return true;
	}
	return QWebPage::eventFilter(obj, ev);
}

QVariant WebViewController::evaluateJavaScript(const QString &script)
{
	// We can't always provide the result, sorry ;)
	QVariant result;
	if (!m_session || m_isLoading)
		m_pendingScripts << script;
	else
		result = mainFrame()->evaluateJavaScript(script);
	return result;
}

bool WebViewController::zoomImage(QWebElement elem)
{
	QString fullClass = QLatin1String("fullSizeImage");
	QString scaledClass = QLatin1String("scaledToFitImage");
	QStringList classes = elem.classes();
	if (classes.contains(fullClass)) {
		elem.removeClass(fullClass);
		elem.addClass(scaledClass);
	} else if (classes.contains(scaledClass)) {
		elem.removeClass(scaledClass);
		elem.addClass(fullClass);
	} else {
		return false;
	}
	
	evaluateJavaScript(QLatin1String("alignChat(true);"));
	return true;
}

void WebViewController::debugLog(const QString &message)
{
	debug() << "WebKit: " << message;
}

void WebViewController::appendNick(const QVariant &nick)
{
	QObject *form = ServiceManager::getByName("ChatForm");
	QObject *obj = 0;
	if (QMetaObject::invokeMethod(form, "textEdit", Q_RETURN_ARG(QObject*, obj),
								  Q_ARG(qutim_sdk_0_3::ChatSession*, m_session.data())) && obj) {
		QTextCursor cursor;
		if (QTextEdit *edit = qobject_cast<QTextEdit*>(obj))
			cursor = edit->textCursor();
		else if (QPlainTextEdit *edit = qobject_cast<QPlainTextEdit*>(obj))
			cursor = edit->textCursor();
		else
			return;
		if(cursor.atStart())
			cursor.insertText(nick.toString() + ": ");
		else
			cursor.insertText(nick.toString() + " ");
		static_cast<QWidget*>(obj)->setFocus();
	}
}

void WebViewController::contextMenu(const QVariant &nickVar)
{
	QString nick = nickVar.toString();
	foreach (ChatUnit *unit, m_session.data()->unit()->lowerUnits()) {
		if (Buddy *buddy = qobject_cast<Buddy*>(unit)) {
			if (buddy->name() == nick)
				buddy->showMenu(QCursor::pos());
		}
	}
}

void WebViewController::appendText(const QVariant &text)
{
	QObject *form = ServiceManager::getByName("ChatForm");
	QObject *obj = 0;
	if (QMetaObject::invokeMethod(form, "textEdit", Q_RETURN_ARG(QObject*, obj),
								  Q_ARG(qutim_sdk_0_3::ChatSession*, m_session.data())) && obj) {
		QTextCursor cursor;
		if (QTextEdit *edit = qobject_cast<QTextEdit*>(obj))
			cursor = edit->textCursor();
		else if (QPlainTextEdit *edit = qobject_cast<QPlainTextEdit*>(obj))
			cursor = edit->textCursor();
		else
			return;
		cursor.insertText(text.toString());
		cursor.insertText(" ");
		static_cast<QWidget*>(obj)->setFocus();
	}
}

void WebViewController::loadSettings(bool onFly)
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
		QVariantList values = config.value(QLatin1String("customStyle")).toList();
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

void WebViewController::onSettingsSaved()
{
	loadSettings(true);
	evaluateJavaScript(m_style.scriptForChangingVariant());
	evaluateJavaScript(m_style.scriptForSettingCustomStyle());
}

void WebViewController::loadHistory()
{
	Config config = Config(QLatin1String("appearance")).group(QLatin1String("chat/history"));
	int max_num = config.value(QLatin1String("maxDisplayMessages"), 5);
	MessageList messages = History::instance()->read(m_session.data()->unit(), max_num);
	foreach (Message mess, messages) {
		mess.setProperty("silent", true);
		mess.setProperty("store", false);
		mess.setProperty("history", true);
		if (!mess.chatUnit()) //TODO FIXME
			mess.setChatUnit(m_session.data()->unit());
		appendMessage(mess);
	}
}

void WebViewController::onLoadFinished()
{
	foreach (const QString &script, m_pendingScripts) {
		mainFrame()->evaluateJavaScript(script);
	}
	m_isLoading = false;
	m_pendingScripts.clear();
	if (qobject_cast<Conference*>(m_session.data()->unit())) {
		updateTopic();
	}
}

void WebViewController::onTopicChanged(const QString &topic)
{
	if (m_topic.text() == topic)
		return;
	m_topic.setText(QString());
	if (!m_isLoading)
		updateTopic();
}

void WebViewController::updateTopic()
{
	QWebElement element = mainFrame()->findFirstElement(QLatin1String("#topic"));
	if (element.isNull())
		return;
	if (!m_session)
		return;
	if (m_topic.text().isEmpty()) {
		Conference *conference = qobject_cast<Conference*>(m_session.data()->unit());
		if (!conference) {
			warning() << "Called WebViewController::updateTopic for non-conference";
			m_topic.setText(QString());
			m_topic.setHtml(QString());
		} else {
			m_topic.setText(conference->topic());
			m_topic.setHtml(QString());
			m_topic.setHtml(UrlParser::parseUrls(m_topic.html()));
		}
		m_topic.setTime(QDateTime::currentDateTime());
	}
	element.setInnerXml(m_style.templateForContent(m_topic, false));
}

void WebViewController::setTopic()
{
	QWebElement element = mainFrame()->findFirstElement(QLatin1String("#topicEdit"));
	Conference *conference = qobject_cast<Conference*>(m_session.data()->unit());
	if (element.isNull() || !conference)
		return;
	conference->setTopic(element.toPlainText());
	updateTopic();
}

void WebViewController::onContentsChanged()
{
}

void WebViewController::onObjectCleared()
{
	mainFrame()->addToJavaScriptWindowObject(QLatin1String("client"), this, QScriptEngine::QtOwnership);
}

void WebViewController::onLinkClicked(const QUrl &url)
{
	QDesktopServices::openUrl(url);
}

} // namespace Adium


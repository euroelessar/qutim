#define QUTIM_PLUGIN_NAME "org.qutim.adiumwebview"

#include "messageviewcontroller.h"
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
#include <QPlainTextEdit>

namespace AdiumWebView {

using namespace qutim_sdk_0_3;

MessageViewController::MessageViewController(QObject *parent) :
    QObject(parent), m_isLoading(false), m_isPreview(false)
{
    m_topic.setProperty("topic", true);
    
//    connect(this, SIGNAL(loadFinished(bool)), SLOT(onLoadFinished()));
}

MessageViewController::~MessageViewController()
{
}

void MessageViewController::setSession(ChatSession *session)
{
    if (m_session != session) {
        m_session = session;
        if (qobject_cast<Conference*>(m_session.data()->unit())) {
            connect(m_session.data()->unit(), SIGNAL(topicChanged(QString,QString)),
                    this, SLOT(onTopicChanged(QString)));
        }
        if (!m_isPreview) {
            loadSettings(false);
            clearChat();
        }
        emit sessionChanged();
    }
}

ChatSession *MessageViewController::session() const
{
    return m_session.data();
}

bool MessageViewController::isContentSimiliar(const Message &a, const Message &b)
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

void MessageViewController::append(const qutim_sdk_0_3::Message &msg)
{
    Message copy = msg;
    QString html = UrlParser::parseUrls(copy.html(), UrlParser::Html);
    copy.setProperty("messageId", msg.id());
    if (msg.property("topic", false)) {
        copy.setHtml(html);
        m_topic = copy;
        if (!m_isLoading)
            updateTopic();
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

void MessageViewController::clearChat()
{
    Q_ASSERT(!m_session.isNull());
    m_last = Message();
    m_isLoading = true;

    setHtml(m_style.baseTemplateForChat(m_session.data()));
    evaluateJavaScript(m_style.scriptForSettingCustomStyle());
}

WebKitMessageViewStyle *MessageViewController::style()
{
    return &m_style;
}

void MessageViewController::setDefaultFont(const QString &family, int size)
{
    QFontInfo info(QFont(family, size));
    emit fontRequested(family, info.pixelSize());

//    QWebSettings *settings = QWebPage::settings();
//    QWebSettings::FontFamily families[] = {
//        QWebSettings::StandardFont,
//        QWebSettings::FixedFont,
//        QWebSettings::SerifFont,
//        QWebSettings::SansSerifFont
//    };
//    const int familiesSize = sizeof(families) / sizeof(families[0]);
//    if (family.isEmpty()) {
//        for (int i = 0; i < familiesSize; ++i)
//            settings->resetFontFamily(families[i]);
//    } else {
//        for (int i = 0; i < familiesSize; ++i)
//            settings->setFontFamily(families[i], family);
//    }
//    if (size < 0)
//        settings->resetFontSize(QWebSettings::DefaultFontSize);
//    else
//        settings->setFontSize(QWebSettings::DefaultFontSize, info.pixelSize());
}

QString MessageViewController::defaultFontFamily() const
{
    return QString();
//    return QWebPage::settings()->fontFamily(QWebSettings::StandardFont);
}

int MessageViewController::defaultFontSize() const
{
    return 1;
//    QWebSettings *settings = QWebPage::settings();
//    QString family = settings->fontFamily(QWebSettings::StandardFont);
//    int size = settings->fontSize(QWebSettings::DefaultFontSize);
//    QFont font(family);
//    font.setPixelSize(size);
//    QFontInfo info(font);
//    return info.pointSize();
}

void MessageViewController::setHtml(const QString &html)
{
    emit htmlRequested(html);
}

//bool MessageViewController::eventFilter(QObject *obj, QEvent *ev)
//{
//    if (obj == m_session.data() && ev->type() == MessageReceiptEvent::eventType()) {
//        MessageReceiptEvent *msgEvent = static_cast<MessageReceiptEvent *>(ev);
//        QWebFrame *frame = mainFrame();
//        QWebElement elem = frame->findFirstElement(QLatin1String("#message")
//                                                   + QString::number(msgEvent->id()));
//        if (!elem.isNull()) {
//            if (msgEvent->success()) {
//                elem.removeClass(QLatin1String("notDelivered"));
//                elem.addClass(QLatin1String("delivered"));
//            } else {
//                elem.addClass(QLatin1String("failedToDeliver"));
//            }
//        }
//        return true;
//    }
//    return QWebPage::eventFilter(obj, ev);
//}

void MessageViewController::evaluateJavaScript(const QString &script)
{
    emit javaScriptRequest(script);
}

//bool MessageViewController::zoomImage(QWebElement elem)
//{
//    QString fullClass = QLatin1String("fullSizeImage");
//    QString scaledClass = QLatin1String("scaledToFitImage");
//    QStringList classes = elem.classes();
//    if (classes.contains(fullClass)) {
//        elem.removeClass(fullClass);
//        elem.addClass(scaledClass);
//    } else if (classes.contains(scaledClass)) {
//        elem.removeClass(scaledClass);
//        elem.addClass(fullClass);
//    } else {
//        return false;
//    }

//    evaluateJavaScript(QLatin1String("alignChat(true);"));
//    return true;
//}

void MessageViewController::debugLog(const QString &message)
{
    qDebug() << "WebKit: " << message;
}

void MessageViewController::appendNick(const QVariant &nick)
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

void MessageViewController::contextMenu(const QVariant &nickVar)
{
    QString nick = nickVar.toString();
    foreach (ChatUnit *unit, m_session.data()->unit()->lowerUnits()) {
        if (Buddy *buddy = qobject_cast<Buddy*>(unit)) {
            if (buddy->name() == nick)
                buddy->showMenu(QCursor::pos());
        }
    }
}

void MessageViewController::appendText(const QVariant &text)
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

void MessageViewController::setLoading(bool loading)
{
    if (loading)
        onLoadFinished();
}

void MessageViewController::loadSettings(bool onFly)
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

void MessageViewController::onSettingsSaved()
{
    loadSettings(true);
    evaluateJavaScript(m_style.scriptForChangingVariant());
    evaluateJavaScript(m_style.scriptForSettingCustomStyle());
}

void MessageViewController::onLoadFinished()
{
    m_isLoading = false;
    if (qobject_cast<Conference*>(m_session.data()->unit())) {
        updateTopic();
    }
}

void MessageViewController::onTopicChanged(const QString &topic)
{
    if (m_topic.text() == topic)
        return;
    m_topic.setText(QString());
    if (!m_isLoading)
        updateTopic();
}

void MessageViewController::updateTopic()
{
    if (!m_session)
        return;
    if (m_topic.text().isEmpty()) {
        Conference *conference = qobject_cast<Conference*>(m_session.data()->unit());
        if (!conference) {
            qWarning() << "Called MessageViewController::updateTopic for non-conference";
            m_topic.setText(QString());
            m_topic.setHtml(QString());
        } else {
            m_topic.setText(conference->topic());
            m_topic.setHtml(QString());
            m_topic.setHtml(UrlParser::parseUrls(m_topic.html()));
        }
        m_topic.setTime(QDateTime::currentDateTime());
    }

    emit topicRequested(m_style.templateForContent(m_topic, false));
}

//void MessageViewController::setTopic()
//{
//    QWebElement element = mainFrame()->findFirstElement(QLatin1String("#topicEdit"));
//    Conference *conference = qobject_cast<Conference*>(m_session.data()->unit());
//    if (element.isNull() || !conference)
//        return;
//    conference->setTopic(element.toPlainText());
//    updateTopic();
//}

void MessageViewController::clearFocusClass()
{
    emit clearFocusRequested();
//    QWebElementCollection elements = mainFrame()->findAllElements(QLatin1String(".focus"));
//    QString focusClass = QLatin1String("focus");
//    QString firstFocusClass = QLatin1String("firstFocus");
//    foreach (QWebElement element, elements) {
//        element.removeClass(focusClass);
//        element.removeClass(firstFocusClass);
//    }
}

} // namespace AdiumWebView

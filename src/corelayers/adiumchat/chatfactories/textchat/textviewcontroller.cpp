/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "textviewcontroller.h"
#include <QTextCursor>
#include <QUrl>
#include <QDateTime>
#include <QScrollBar>
#include <QPainter>
#include <QDesktopServices>
#include <QPlainTextEdit>
#include <qutim/servicemanager.h>
#include <qutim/chatunit.h>
#include <qutim/message.h>
#include <qutim/messagesession.h>
#include <qutim/conference.h>
#include <qutim/account.h>
#include <qutim/history.h>
#include <qutim/debug.h>
#include <qutim/emoticons.h>
#include <qutim/thememanager.h>
#include <chatlayer/chatsessionimpl.h>

using namespace qutim_sdk_0_3;

namespace Core
{
namespace AdiumChat
{
enum { EmoticonObjectType = 0x666 };

TextViewController::TextViewController()
{
	m_cache.setMaxCost(40);
	m_isLastIncoming = false;
	m_scrollBarPosition = 0;
	Config cfg = Config(QLatin1String("appearance")).group(QLatin1String("chat"));
	m_groupUntil = cfg.value<ushort>(QLatin1String("groupUntil"), 900);
	cfg.beginGroup(QLatin1String("textview"));
	m_animateEmoticons = cfg.value(QLatin1String("animateEmoticons"), true);
	m_incomingColor.setNamedColor(cfg.value(QLatin1String("incomingColor"), QLatin1String("#ff6600")));
	m_outgoingColor.setNamedColor(cfg.value(QLatin1String("outgoingColor"), QLatin1String("#0078ff")));
	m_serviceColor .setNamedColor(cfg.value(QLatin1String("serviceColor"),  QLatin1String("gray")));
	m_baseColor    .setNamedColor(cfg.value(QLatin1String("baseColor"),     QLatin1String("black")));
	m_urlColor     .setNamedColor(cfg.value(QLatin1String("urlColor"),      QLatin1String("#0033aa")));

	cfg.beginGroup(QLatin1String("font"));
#ifdef Q_WS_MAEMO_5
	m_font.setFamily(cfg.value(QLatin1String("family"), QLatin1String("Nokia Sans")));
	m_font.setPointSize(cfg.value(QLatin1String("size"), 15));
#elif defined(Q_WS_S60)
	m_font.setFamily(cfg.value(QLatin1String("family"), m_font.family()));
	m_font.setPointSize(cfg.value(QLatin1String("size"), m_font.pointSize()));
#else
	m_font.setFamily(cfg.value(QLatin1String("family"), QLatin1String("verdana")));
	m_font.setPointSize(cfg.value(QLatin1String("size"), 10));
#endif
	cfg.endGroup();
	cfg.endGroup();
	cfg.beginGroup(QLatin1String("history"));
	m_storeServiceMessages = cfg.value(QLatin1String("storeServiceMessages"), true);
	cfg.endGroup();
	documentLayout()->registerHandler(EmoticonObjectType, this);
	init();
}

TextViewController::~TextViewController()
{
}

void TextViewController::setChatSession(ChatSessionImpl *session)
{
	m_session = session;
	m_session->installEventFilter(this);
	loadHistory();
}

ChatSessionImpl *TextViewController::getSession() const
{
	return m_session;
}

void TextViewController::appendMessage(const qutim_sdk_0_3::Message &msg)
{
	if (msg.text().isEmpty())
		return;
	QTextCursor cursor(this);
	cursor.beginEditBlock();
	bool shouldScroll = isNearBottom();
	QTextCharFormat defaultFormat;
	defaultFormat.setFont(m_font);
	defaultFormat.setForeground(m_baseColor);
	cursor.setCharFormat(defaultFormat);
	cursor.movePosition(QTextCursor::End);
	QString currentSender = makeName(msg);
	bool isMe = msg.text().startsWith(QLatin1String("/me "));
	bool isService = msg.property("service", false);
	if(isMe || msg.property("action", false)) {
		QString text = isMe ? msg.text().mid(4) : msg.text();
		cursor.insertText(QLatin1String("\n"));
		QTextCharFormat format = defaultFormat;
		format.setFontWeight(QFont::Bold);
		format.setForeground(msg.isIncoming() ? m_incomingColor : m_outgoingColor);
		if (msg.isIncoming()) {
			format.setAnchor(true);
			format.setAnchorHref(QLatin1String("nick:///") + QUrl::toPercentEncoding(currentSender));
		}
		cursor.insertText(currentSender, format);
		if (msg.isIncoming()) {
			format.setAnchor(false);
			format.setAnchorHref(QString());
		}
		cursor.insertText(QLatin1String(" "), format);
		format.setFontWeight(QFont::Normal);
		appendText(cursor, text, format, true);
		m_lastSender.clear();
	} else if (isService) {
		cursor.insertText(QLatin1String("\n"));
		QTextCharFormat format = defaultFormat;
		format.setForeground(m_serviceColor);
		appendText(cursor, msg.text(), format, false);
		m_lastSender.clear();
	} else {
		if (m_isLastIncoming != msg.isIncoming() || currentSender != m_lastSender || shouldBreak(msg.time())) {
			cursor.insertBlock();
			QTextCharFormat format = defaultFormat;
			format.setFontWeight(QFont::Bold);
			format.setForeground(msg.isIncoming() ? m_incomingColor : m_outgoingColor);
			if (msg.isIncoming()) {
				format.setAnchor(true);
				format.setAnchorHref(QLatin1String("nick:///") + QUrl::toPercentEncoding(currentSender));
			}
			cursor.insertText(currentSender, format);
			if (msg.isIncoming()) {
				format.setAnchor(false);
				format.setAnchorHref(QString());
			}
			cursor.insertText(QLatin1String(" "), format);
			format.setFontWeight(QFont::Normal);
			QString timeFormat = QLatin1String((msg.time().date() == m_lastTime.date())
			                                   ? "(hh:mm)" : "(dd.MM.yyyy hh:mm)");
			cursor.insertText(msg.time().toString(timeFormat), format);
		}
		m_lastSender = currentSender;
		m_lastTime = msg.time();
		m_isLastIncoming = msg.isIncoming();
		cursor.insertText(QLatin1String("\n"));
		bool showReceived = msg.isIncoming();
		if (msg.property("history", false))
			showReceived = true;
		if (!showReceived)
			m_cache.insert(msg.id(), new int(cursor.position()));
		cursor.insertImage(QLatin1String(showReceived ? "bullet-received" : "bullet-send"));

		cursor.insertText(QLatin1String(" "), defaultFormat);
		appendText(cursor, msg.text(), defaultFormat, true);
	}
	if (msg.property("store", true) && (!isService || (isService && m_storeServiceMessages)))
		History::instance()->store(msg);
	if (shouldScroll)
		QTimer::singleShot(0, this, SLOT(ensureScrolling()));
	cursor.endEditBlock();
}

QList<TextViewController::Token> TextViewController::makeUrls(const QString &html)
{
	QList<Token> result;
	static QRegExp linkRegExp("([a-zA-Z0-9\\-\\_\\.]+@([a-zA-Z0-9\\-\\_]+\\.)+[a-zA-Z]+)|"
							  "(([a-zA-Z]+://|www\\.)([\\w:/\\?#\\[\\]@!\\$&\\(\\)\\*\\+,;=\\._~-]|&amp;|%[0-9a-fA-F]{2})+)",
							  Qt::CaseInsensitive);
	Q_ASSERT(linkRegExp.isValid());
	int pos = 0;
	int lastPos = 0;
	while(((pos = linkRegExp.indexIn(html, pos)) != -1))
	{
		Token tok = { html.midRef(lastPos, pos - lastPos), QString() };
		if (!tok.text.isEmpty()) {
			if (!result.isEmpty() && result.last().url.isEmpty()) {
				QStringRef tmp = result.last().text;
				result.last().text = QStringRef(tmp.string(), tmp.position(), tmp.size() + tok.text.size());
			} else {
				result << tok;
			}
		}
		QString link = linkRegExp.cap(0);
		tok.text = html.midRef(pos, link.size());
		pos += link.size();
		if (link.startsWith(QLatin1String("www."), Qt::CaseInsensitive))
			link.prepend(QLatin1String("http://"));
		else if(!link.contains(QLatin1String("//")))
			link.prepend(QLatin1String("mailto:"));
		tok.url = link;
		result << tok;
		lastPos = pos;
	}
	if (!result.isEmpty() && result.last().url.isEmpty()) {
		result.last().text = html.midRef(result.last().text.position());
	} else {
		Token tok = { html.midRef(lastPos), QString() };
		result << tok;
	}
	return result;
}

void TextViewController::appendText(QTextCursor &cursor, const QString &text,
                                    const QTextCharFormat &format, bool emo)
{
	QTextCharFormat urlFormat = format;
	urlFormat.setForeground(m_urlColor);
	urlFormat.setFontUnderline(true);
	urlFormat.setAnchor(true);
	foreach (const Token &textToken, makeUrls(text)) {
		if (!textToken.url.isEmpty()) {
			urlFormat.setAnchorHref(textToken.url);
			cursor.insertText(textToken.text.toString(), urlFormat);
			continue;
		} else if (!emo) {
			cursor.insertText(textToken.text.toString(), format);
			continue;
		}
		const QList<EmoticonsTheme::Token> tokens = Emoticons::theme().tokenize(textToken.text.toString());
		QString objectReplacement(QChar::ObjectReplacementCharacter);
		QTextCharFormat emoticonFormat;
		emoticonFormat.setObjectType(EmoticonObjectType);
		int emoticonIndex;
		for (int i = 0; i < tokens.size(); i++) {
			const EmoticonsTheme::Token &token = tokens.at(i);
			switch(token.type) {
			case EmoticonsTheme::Image:
				if (m_animateEmoticons) {
					emoticonIndex = addEmoticon(token.imgPath);
					emoticonFormat.setProperty(QTextFormat::UserProperty, emoticonIndex);
					m_emoticons.at(emoticonIndex).movie->indexes << cursor.position();
					cursor.insertText(objectReplacement, emoticonFormat);
				} else {
					if (!m_images.contains(token.imgPath)) {
						addResource(ImageResource, QUrl(token.imgPath), QPixmap(token.imgPath));
						m_images.insert(token.imgPath);
					}
					QTextImageFormat imageFormat;
					imageFormat.setName(token.imgPath);
					imageFormat.setToolTip(token.text);
					cursor.insertImage(imageFormat);
				}
				break;
			case EmoticonsTheme::Text:
				cursor.insertText(token.text, format);
			default:
				break;
			}
		}
	}
}

bool TextViewController::isNearBottom()
{
	if (!m_textEdit)
		return true;
	QScrollBar *scrollBar = m_textEdit->verticalScrollBar();
	qreal percentage = scrollBar->maximum() - scrollBar->value();
	percentage /= m_textEdit->viewport()->height();
	return percentage < 0.2;
}

void TextViewController::drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc,
                                int posInDocument, const QTextFormat &format)
{
	Q_UNUSED(posInDocument);
	Q_UNUSED(doc);
	int index = format.intProperty(QTextFormat::UserProperty);
	const EmoticonTrack &track = m_emoticons.at(index);
	QPixmap pixmap = track.movie->currentPixmap();
	painter->drawPixmap(rect, pixmap, track.movie->frameRect());
}

QSizeF TextViewController::intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
	Q_UNUSED(posInDocument);
	Q_UNUSED(doc);
	int index = format.intProperty(QTextFormat::UserProperty);
	const EmoticonTrack &track = m_emoticons.at(index);
	return track.movie->frameRect().size();
}

int TextViewController::addEmoticon(const QString &filename)
{
	int index = m_hash.value(filename, m_emoticons.size());
	if (index == m_emoticons.size()) {
		m_emoticons.append(EmoticonTrack());
		EmoticonTrack &track = m_emoticons.last();
		track.movie = new EmoticonMovie(filename, index, this);
		connect(track.movie, SIGNAL(frameChanged(int)), this, SLOT(animate()));
		track.movie->setCacheMode(QMovie::CacheAll);
//		QTimer::singleShot(0, track.movie, SLOT(start()));
		m_hash.insert(filename, index);
		track.movie->start();
	}
	return index;
}

void TextViewController::ensureScrolling()
{
	if (m_textEdit) {
		QScrollBar *scrollBar = m_textEdit->verticalScrollBar();
//		qreal percentage = scrollBar->maximum() - scrollBar->value();
//		percentage /= m_textEdit->viewport()->height();
//		debug() << percentage;
//		if (percentage < 0.1)
			scrollBar->setValue(scrollBar->maximum());
	}
}

void TextViewController::onAnchorClicked(const QUrl &url)
{
	Q_ASSERT(m_textEdit);
	if (url.scheme() == QLatin1String("nick")) {
		QObject *form = ServiceManager::getByName("ChatForm");
		QObject *obj = 0;
		if (QMetaObject::invokeMethod(form, "textEdit", Q_RETURN_ARG(QObject*, obj),
									  Q_ARG(qutim_sdk_0_3::ChatSession*, m_session)) && obj) {
			QTextCursor cursor;
			if (QTextEdit *edit = qobject_cast<QTextEdit*>(obj))
				cursor = edit->textCursor();
			else if (QPlainTextEdit *edit = qobject_cast<QPlainTextEdit*>(obj))
				cursor = edit->textCursor();
			else
				return;
			bool atStart = cursor.atStart();
			cursor.insertText(url.path().mid(1));
			cursor.insertText(QLatin1String(atStart ? ": " : " "));
			static_cast<QWidget*>(obj)->setFocus();
		}
	} else {
		QDesktopServices::openUrl(url);
	}
}

void TextViewController::animate()
{
	EmoticonMovie *movie = static_cast<EmoticonMovie*>(sender());
	if (!m_textEdit) {
		movie->stop();
		return;
	}
	QAbstractTextDocumentLayout *layout = documentLayout();
	QRect visibleRect(0, m_textEdit->verticalScrollBar()->value(),
	                  m_textEdit->viewport()->width(),
	                  m_textEdit->viewport()->height());
	int begin = layout->hitTest(visibleRect.topLeft(), Qt::FuzzyHit);
	int end = layout->hitTest(visibleRect.bottomRight(), Qt::FuzzyHit);
	int *indexesEnd = movie->indexes.data() + movie->indexes.size();
	int *beginIndex = qLowerBound(movie->indexes.data(), indexesEnd, begin);
	int *endIndex = qUpperBound(beginIndex, indexesEnd, end);
	if (beginIndex == endIndex)
		return;
	QRegion region;
	QTextCursor cursor(this);
	QSize emoticonSize = movie->frameRect().size();
	for (int *i = beginIndex; i != endIndex; i++) {
		cursor.setPosition(*i);
		QRect cursorRect = m_textEdit->cursorRect(cursor);
		region += QRectF(cursorRect.topLeft(), emoticonSize).toAlignedRect();
	}
	region &= m_textEdit->viewport()->visibleRegion();
	if (!region.isEmpty())
		m_textEdit->viewport()->update(region);
}

void TextViewController::init()
{
	QPixmap pixmap;
	QString stylePath = ThemeManager::path(QLatin1String("webkitstyle"), QLatin1String("Tory"))
	        + QLatin1String("/Contents/Resources/");
	pixmap.load(stylePath + QLatin1String("Images/bullet-received.png"));
	addResource(QTextDocument::ImageResource, QUrl(QLatin1String("bullet-received")), pixmap);
	pixmap.load(stylePath + QLatin1String("Images/bullet-send.png"));
	addResource(QTextDocument::ImageResource, QUrl(QLatin1String("bullet-send")), pixmap);
	for (int i = 0; i < m_emoticons.size(); i++)
		m_emoticons.at(i).movie->deleteLater();
	m_images.clear();
	m_emoticons.clear();
}

void TextViewController::loadHistory()
{
	debug() << Q_FUNC_INFO;
	Config config = Config(QLatin1String("appearance")).group(QLatin1String("chat/history"));
	int max_num = config.value(QLatin1String("maxDisplayMessages"), 5);
	MessageList messages = History::instance()->read(m_session->getUnit(), max_num);
	foreach (Message mess, messages) {
		mess.setProperty("silent", true);
		mess.setProperty("store", false);
		mess.setProperty("history", true);
		if (!mess.chatUnit()) //TODO FIXME
			mess.setChatUnit(m_session->getUnit());
		appendMessage(mess);
	}
	m_lastSender.clear();
}

QString TextViewController::makeName(const Message &mes)
{
	QString senderName = mes.property("senderName", QString());
	if (senderName.isEmpty()) {
		if (!mes.isIncoming()) {
			const Conference *conf = qobject_cast<const Conference*>(mes.chatUnit());
			if (conf && conf->me())
				senderName = conf->me()->title();
			else
				senderName = mes.chatUnit()->account()->name();
		} else {
			senderName = mes.chatUnit()->title();
		}
	}
	return senderName;
}

bool TextViewController::shouldBreak(const QDateTime &time)
{
	if (time.date() != m_lastTime.date())
		return true;
	if (m_lastTime.secsTo(time) > m_groupUntil)
		return true;
	return false;
}

void TextViewController::clearChat()
{
	debug() << Q_FUNC_INFO;
	QTextDocument::clear();
	init();
}

void TextViewController::setTextEdit(QTextBrowser *edit)
{
	if (m_textEdit)
		disconnect(m_textEdit, 0, this, 0);
	m_textEdit = edit;
	if (m_textEdit)
		connect(m_textEdit, SIGNAL(anchorClicked(QUrl)), this, SLOT(onAnchorClicked(QUrl)));
	for (int i = 0; i < m_emoticons.size(); i++)
		m_emoticons.at(i).movie->setPaused(!edit);
}

bool TextViewController::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() == MessageReceiptEvent::eventType()) {
		MessageReceiptEvent *msgEvent = static_cast<MessageReceiptEvent *>(ev);
		int *pos = m_cache.take(msgEvent->id());
		debug() << msgEvent->id() << (pos ? *pos : -1);
		if (pos) {
			QTextCursor cursor(this);
			cursor.beginEditBlock();
			cursor.setPosition(*pos);
			cursor.deleteChar();
			cursor.insertImage(QLatin1String("bullet-received"));
			cursor.endEditBlock();
//			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
//			QTextImageFormat format;
//			format.setName(QLatin1String("bullet-received"));
//			cursor.setCharFormat(format);
			delete pos;
		}
		return true;
	}
	return QTextDocument::eventFilter(obj, ev);
}
}
}

/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef TEXTVIEWCONTROLLER_H
#define TEXTVIEWCONTROLLER_H

#include <chatlayer/chatviewfactory.h>
#include <QTextDocument>
#include <QTextBrowser>
#include <QCache>
#include <QWeakPointer>
#include <QDateTime>
#include <QTextObjectInterface>
#include <QMovie>

namespace Core
{
namespace AdiumChat
{
class EmoticonMovie : public QMovie
{
public:
	EmoticonMovie(const QString &fileName, int index, QObject *parent) :
	    QMovie(fileName, QByteArray(), parent)
	{
		m_index = index;
	}
	
	QVector<int> indexes;
	
	int index() const { return m_index; }
private:
	int m_index;
};

struct EmoticonTrack
{
	EmoticonMovie *movie;
};

class TextViewController : public QTextDocument, public ChatViewController, public QTextObjectInterface
{
	Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatViewController QTextObjectInterface)
public:
    TextViewController();
	virtual ~TextViewController();
	virtual void setChatSession(ChatSessionImpl *session);
	virtual ChatSessionImpl *getSession() const;
	virtual void appendMessage(const qutim_sdk_0_3::Message &msg);
	void appendText(QTextCursor &cursor, const QString &text, const QTextCharFormat &format, bool emo);
	virtual void clearChat();
	virtual QString quote();
	void setTextEdit(QTextBrowser *edit);
	int scrollBarPosition() const { return m_scrollBarPosition; }
	void setScrollBarPosition(int pos) { m_scrollBarPosition = pos; }
	bool isNearBottom();
	
	
	// From QTextObjectInterface
    virtual void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc,
	                        int posInDocument, const QTextFormat &format);
    virtual QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format);
	
	bool eventFilter(QObject *, QEvent *);
public slots:
	void ensureScrolling();
protected slots:
	void onAnchorClicked(const QUrl &url);
	void animate();
private:
	QPixmap createBullet(const QColor &color);
	void init();
	void loadHistory();
	int addEmoticon(const QString &filename);
	QString makeName(const qutim_sdk_0_3::Message &mes);
	bool shouldBreak(const QDateTime &time);
	
	QWeakPointer<QTextBrowser> m_textEdit;
	ChatSessionImpl *m_session;
	QCache<qint64, int> m_cache;
	QDateTime m_lastTime;
	QString m_lastSender;
	QString m_lastIncomingMessage;
	bool m_isLastIncoming;
	bool m_animateEmoticons;
	bool m_atAnimation;
	short m_groupUntil;
	int m_scrollBarPosition;
	int m_bulletSize;
	QFont m_font;
	QColor m_backgroundColor;
	QColor m_incomingColor;
	QColor m_outgoingColor;
	QColor m_serviceColor;
	QColor m_baseColor;
	QColor m_urlColor;
	QColor m_bulletErrorColor;
	QColor m_bulletSentColor;
	QColor m_bulletReceivedColor;
	QSet<QString> m_images;
	QHash<QString, int> m_hash;
	QList<EmoticonTrack> m_emoticons;
};
}
}

#endif // TEXTVIEWCONTROLLER_H


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

#ifndef TEXTVIEWCONTROLLER_H
#define TEXTVIEWCONTROLLER_H

#include <chatlayer/chatviewfactory.h>
#include <QTextDocument>
#include <QTextEdit>
#include <QCache>
#include <QPointer>
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
	
	QList<int> indexes;
	
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
	void appendText(QTextCursor &cursor, const QString &text, const QTextCharFormat &format);
	virtual void clear();
	void setTextEdit(QTextEdit *edit);
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
	void animate();
private:
	void init();
	void loadHistory();
	int addEmoticon(const QString &filename);
	QString makeName(const qutim_sdk_0_3::Message &mes);
	bool shouldBreak(const QDateTime &time);
	
	QPointer<QTextEdit> m_textEdit;
	ChatSessionImpl *m_session;
	QCache<qint64, int> m_cache;
	QDateTime m_lastTime;
	QString m_lastSender;
	bool m_isLastIncoming;
	bool m_storeServiceMessages;
	bool m_animateEmoticons;
	bool m_atAnimation;
	short m_groupUntil;
	int m_scrollBarPosition;
	QColor m_incomingColor;
	QColor m_outgoingColor;
	QColor m_serviceColor;
	QSet<QString> m_images;
	QHash<QString, int> m_hash;
	QList<EmoticonTrack> m_emoticons;
};
}
}

#endif // TEXTVIEWCONTROLLER_H

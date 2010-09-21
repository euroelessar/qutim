/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef EMOEDITPLUGIN_H
#define EMOEDITPLUGIN_H

#include <qutim/plugin.h>
#include <qutim/messagesession.h>
#include <qutim/emoticons.h>
#include <QTextObjectInterface>

using namespace qutim_sdk_0_3;

class EmoEditPlugin : public Plugin, public QTextObjectInterface
{
	Q_INTERFACES(QTextObjectInterface)
	Q_OBJECT
public:
	EmoEditPlugin();
	virtual void init();
	virtual bool load();
	virtual bool unload();
    virtual QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format);
    virtual void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc,
							int posInDocument, const QTextFormat &format);
protected slots:
	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
	void onSessionDestroyed(QObject *session);
	void onDocumentContentsChanged(QTextDocument *doc = 0);
private:
	QScopedPointer<EmoticonsTheme> m_theme;
	bool m_inParsingState;
};

#endif // EMOEDITPLUGIN_H

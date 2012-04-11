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

#ifndef EMOEDITPLUGIN_H
#define EMOEDITPLUGIN_H

#include <qutim/plugin.h>
#include <qutim/chatsession.h>
#include <qutim/emoticons.h>
#include <QTextObjectInterface>

using namespace qutim_sdk_0_3;

class EmoEditPlugin : public Plugin, public QTextObjectInterface
{
	Q_INTERFACES(QTextObjectInterface)
	Q_OBJECT
	Q_CLASSINFO("Uses", "ChatLayer")
	Q_CLASSINFO("DebugName", "EmoEdit")
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


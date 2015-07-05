/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef CHATEMOTICONSWIDGET_H
#define CHATEMOTICONSWIDGET_H

#include <QScrollArea>
#include <QAction>
#include <QPointer>

namespace Core
{
namespace AdiumChat
{

class ChatEmoticonsWidget : public QScrollArea
{
	Q_OBJECT
public:
	ChatEmoticonsWidget(QWidget *parent = 0);
	void play();
	void stop();
public slots:
	void loadTheme();
	void clearEmoticonsPreview();
protected:
	void showEvent(QShowEvent *);
	void hideEvent(QHideEvent *);
	bool eventFilter(QObject *, QEvent *);
signals:
	void insertSmile(const QString &code);
private:
	QWidgetList m_active_emoticons;
};

class EmoAction : public QAction
{
	Q_OBJECT
public:
	EmoAction(QObject *parent = 0);
signals:
	void insertSmile(const QString &code);
private:
	QPointer<ChatEmoticonsWidget> m_emoticons_widget;
private slots:
	void onInsertSmile(const QString &code);
	void triggerEmoticons();
#ifdef Q_WS_MAEMO_5
	void orientationChanged();
#endif
};

}
}
#endif // CHATEMOTICONSWIDGET_H


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
#include "chatemoticonswidget.h"
#include <QLabel>
#include <QHash>
#include <QStyle>
#include <QMovie>
#include <flowlayout.h>
#include <qutim/emoticons.h>
#include <QEvent>
#include <qutim/debug.h>
#include <QWidgetAction>
#include <QMenu>
#include <QApplication>
#include <QDesktopWidget>
#include <qutim/systemintegration.h>
#include <qutim/servicemanager.h>

#include <QPlainTextEdit>

namespace Core
{
namespace AdiumChat
{
using namespace qutim_sdk_0_3;

ChatEmoticonsWidget::ChatEmoticonsWidget(QWidget *parent) :
	QScrollArea(parent)
{
#ifndef Q_WS_MAEMO_5
# ifdef QUTIM_MOBILE_UI
	QRect screenGeometry = QApplication::desktop()->screenGeometry();
	resize(screenGeometry.width() * 0.8, screenGeometry.height() * 0.8);
# else
	resize(400,400);
# endif
	setMinimumSize(size());
	setFrameStyle(QFrame::NoFrame);
#else
	move(80,0);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#endif
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QWidget *w = new QWidget(this);
	setWidget(w);
	new FlowLayout(w);

	QAction *action = new QAction(tr("Close"),this);
	action->setSoftKeyRole(QAction::NegativeSoftKey);
	connect(action, SIGNAL(triggered()), SLOT(close()));
	addAction(action);

	if(QObject *scroller = ServiceManager::getByName("Scroller"))
		QMetaObject::invokeMethod(scroller, "enableScrolling", Q_ARG(QObject*, viewport()));

}

void ChatEmoticonsWidget::loadTheme()
{
	EmoticonsTheme theme = Emoticons::theme();
	const QStringList emoticons = theme.emoticonsIndexes();
	const QHash<QString, QStringList> hash = theme.emoticonsMap();
	clearEmoticonsPreview();
	for (int i = 0; i < emoticons.size(); ++i) {
		QLabel *label = new QLabel();
		label->setFocusPolicy(Qt::StrongFocus);
		QMovie *emoticon = new QMovie (emoticons.at(i), QByteArray(), label);
		label->setMovie(emoticon);
		label->setToolTip(hash.value(emoticons.at(i)).first());
		widget()->layout()->addWidget(label);
		m_active_emoticons.append(label);

		label->installEventFilter(this);
	}
}

void ChatEmoticonsWidget::clearEmoticonsPreview()
{
	for (int i = 0;i != m_active_emoticons.count();i ++) {
		m_active_emoticons.at(i)->deleteLater();
	}
	m_active_emoticons.clear();
}

void ChatEmoticonsWidget::play()
{
	foreach (QWidget *widget,m_active_emoticons) {
		QLabel *label = static_cast<QLabel *>(widget);
		label->movie()->start(); //FIXME on s60 retarding and eats battery
	}
}

void ChatEmoticonsWidget::stop()
{
	foreach (QWidget *widget,m_active_emoticons) {
		QLabel *label = static_cast<QLabel *>(widget);
		label->movie()->stop();
	}
}

void ChatEmoticonsWidget::showEvent(QShowEvent *)
{
	play();
	FlowLayout *layout = static_cast<FlowLayout *>(widget()->layout());
	widget()->resize(width(),layout->heightForWidth(width()));
}

void ChatEmoticonsWidget::hideEvent(QHideEvent *)
{
	stop();
}

bool ChatEmoticonsWidget::eventFilter(QObject *obj, QEvent *event)
{
	if (QLabel *label = static_cast<QLabel *>(obj))	{
		if ((event->type() == QEvent::MouseButtonPress)&&(label->toolTip()!=""))
			emit insertSmile(label->toolTip());
	}
	return QObject::eventFilter(obj, event);
}

EmoAction::EmoAction(QObject *parent) :
	QAction(parent)
{
#ifndef Q_WS_MAEMO_5
	QMenu *menu = new QMenu(parentWidget());
	setMenu(menu);
	QWidgetAction *emoticons_widget_act = new QWidgetAction(this);
	m_emoticons_widget = new ChatEmoticonsWidget(menu);
	m_emoticons_widget.data()->loadTheme();
	emoticons_widget_act->setDefaultWidget(m_emoticons_widget.data());
	menu->addAction(emoticons_widget_act);
	connect(m_emoticons_widget.data(), SIGNAL(insertSmile(QString)),
			this,SLOT(onInsertSmile(QString)));
#else
	connect(this,SIGNAL(triggered()),this,SLOT(triggerEmoticons()));
#endif
}

void EmoAction::onInsertSmile(const QString &code)
{
	setProperty("emoticon",code);
	emit triggered();
	setProperty("emoticon","");
}
#ifdef Q_WS_MAEMO_5
void EmoAction::orientationChanged()
{
	if (m_emoticons_widget.data()->isVisible())
	{
		QRect screenGeometry = QApplication::desktop()->screenGeometry();
		if (screenGeometry.width() > screenGeometry.height())
		{
			//This crap need to completely remake!
			m_emoticons_widget.data()->hide();
			m_emoticons_widget.data()->resize(m_emoticons_widget.data()->parentWidget()->width()-160,m_emoticons_widget.data()->parentWidget()->height()-130);
			m_emoticons_widget.data()->show();
		}else{
			m_emoticons_widget.data()->hide();
			m_emoticons_widget.data()->resize(m_emoticons_widget.data()->parentWidget()->width()-160,m_emoticons_widget.data()->parentWidget()->height()/2);
			m_emoticons_widget.data()->show();
		}
	}
}
#endif

void EmoAction::triggerEmoticons()
{
#ifdef Q_WS_MAEMO_5
	if (!m_emoticons_widget) {
		m_emoticons_widget = new ChatEmoticonsWidget();
		m_emoticons_widget.data()->loadTheme();
		connect(m_emoticons_widget.data(), SIGNAL(insertSmile(QString)),
				this,SLOT(onInsertSmile(QString)));
				m_emoticons_widget.data()->setParent(qApp->activeWindow());
	}
	if (m_emoticons_widget.data()->isVisible()) {
		m_emoticons_widget.data()->hide();
	} else {
		QRect screenGeometry = QApplication::desktop()->screenGeometry();
		if (screenGeometry.width() > screenGeometry.height())
		{
			//This crap need to completely remake!
			m_emoticons_widget.data()->resize(m_emoticons_widget.data()->parentWidget()->width()-160,m_emoticons_widget.data()->parentWidget()->height()-130);
		}else{
			m_emoticons_widget.data()->resize(m_emoticons_widget.data()->parentWidget()->width()-160,m_emoticons_widget.data()->parentWidget()->height()/2);
		}
		m_emoticons_widget.data()->show();
	}
#else
	if (!m_emoticons_widget) {
		m_emoticons_widget = new ChatEmoticonsWidget();
		m_emoticons_widget.data()->loadTheme();
		connect(m_emoticons_widget.data(), SIGNAL(insertSmile(QString)),
				this,SLOT(onInsertSmile(QString)));
		SystemIntegration::show(m_emoticons_widget.data());
		m_emoticons_widget.data()->play();

	} else
		m_emoticons_widget.data()->deleteLater();
#endif

}

}
}


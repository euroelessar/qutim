/****************************************************************************
*  textpopup.cpp
*
*  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
*
***************************************************************************
*                                                                         *
*   This library is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************
*****************************************************************************/

#include "textpopup.h"
#include <QVBoxLayout>
#include "manager.h"
#include <QMouseEvent>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QDesktopWidget>
#include <qutim/qtwin.h>
#include <qutim/emoticons.h>
#include <qutim/chatunit.h>
#include <qutim/messagesession.h>
#include <qutim/message.h>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <qutim/debug.h>

namespace Core
{
namespace KineticPopups
{
TextPopupWidget::TextPopupWidget()
{
	m_browser = new QTextBrowser(this);
	QVBoxLayout *l = new QVBoxLayout(this);
	setLayout(l);
	l->setMargin(0);
	l->addWidget(m_browser);
	m_browser->viewport()->installEventFilter(this);

	QString theme_name = Manager::self()->themeName;
	ThemeHelper::PopupSettings popupSettings = ThemeHelper::loadThemeSetting(theme_name);
	init(popupSettings);
}

void TextPopupWidget::init(const ThemeHelper::PopupSettings &popupSettings)
{
	m_timer.setSingleShot(true);
	connect(&m_timer, SIGNAL(timeout()), this, SIGNAL(activated()));
	//init browser
	setTheme(popupSettings);

	if (popupSettings.popupFlags & ThemeHelper::Preview) {
		setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	}
	else {
		setWindowFlags(popup_settings.widgetFlags);
		setAttribute(Qt::WA_DeleteOnClose);
		if (popupSettings.popupFlags & ThemeHelper::Transparent) {
			setAttribute(Qt::WA_NoSystemBackground);
			setAttribute(Qt::WA_TranslucentBackground);
			m_browser->viewport()->setAttribute(Qt::WA_TranslucentBackground);
			m_browser->viewport()->setAutoFillBackground(false);
		}
		if (popupSettings.popupFlags & ThemeHelper::AeroThemeIntegration) {
			//init aero integration for win
			if (QtWin::isCompositionEnabled()) {
				QtWin::extendFrameIntoClientArea(this);
				setContentsMargins(0, 0, 0, 0);
			}
		}

	}
	m_browser->setFrameShape (QFrame::NoFrame);
	m_browser->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
	m_browser->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
	m_browser->setContextMenuPolicy(Qt::NoContextMenu);
	m_browser->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff);
}

void TextPopupWidget::setTheme(const QString &theme)
{
	setTheme(ThemeHelper::loadThemeSetting(theme));
}

void TextPopupWidget::setData(const QString& title, const QString& body,
							  QObject *sender,const QVariant &data)
{
	Manager *manager = Manager::self();

	m_sender = sender;
	m_data = data;

	QString popup_title = title;

	Contact *c = qobject_cast<Contact *>(sender);
	QString image_path = c ? c->avatar() : QString();
	if(image_path.isEmpty())
		image_path = QLatin1String(":/icons/qutim_64");
	QString popup_data = popup_settings.content;
	QString text;
//	if (true)
//		text = Emoticons::theme().parseEmoticons(text);
//	else
//		text = body;

	text = body;
	if (text.length() > manager->maxTextLength) {
		text.truncate(manager->maxTextLength);
		text.append("...");
	}
	popup_data.replace ( "{title}", popup_title );
	popup_data.replace ( "{body}", text);
	popup_data.replace ( "{imagepath}",image_path);
	m_browser->document()->setTextWidth(popup_settings.defaultSize.width());
	m_browser->document()->setHtml(popup_data);
	emit sizeChanged(sizeHint());

	if (manager->timeout > 0) {
		if (m_timer.isActive())
			m_timer.stop();
		m_timer.start(manager->timeout);
	}
}


void TextPopupWidget::setTheme (const ThemeHelper::PopupSettings &popupSettings )
{
	popup_settings = popupSettings;
	m_browser->setStyleSheet (popup_settings.styleSheet);
}

bool TextPopupWidget::eventFilter(QObject *obj, QEvent *ev)
{
	if(ev->type() == QEvent::MouseButtonPress) {
		QMouseEvent *event = static_cast<QMouseEvent*>(ev);
		if (event->button() == Qt::LeftButton) {
			onAction1Triggered();
		}
		else if (event->button() == Qt::RightButton)
			onAction2Triggered();
		else
			return false;
		emit activated();
		return true;
	}
	return AbstractPopupWidget::eventFilter(obj,ev);
}

TextPopupWidget::~TextPopupWidget()
{
}

void KineticPopups::TextPopupWidget::onAction1Triggered()
{
	if (ChatUnit *unit = qobject_cast<ChatUnit *>(m_sender)) {
		ChatUnit *metaContact = unit->metaContact();
		if (metaContact)
			unit = metaContact;
		ChatLayer::get(unit,true)->activate();
	}
	else if (QWidget *widget = qobject_cast<QWidget *>(m_sender)) {
		widget->raise();
	}
}

void KineticPopups::TextPopupWidget::onAction2Triggered()
{
	ChatUnit *unit = qobject_cast<ChatUnit *>(m_sender);
	ChatSession *sess;
	if (unit && (sess = ChatLayer::get(unit,false))) {

		if (m_data.canConvert<Message>())
			sess->markRead(m_data.value<Message>().id());
	}
}

void TextPopupWidget::timerEvent(QTimerEvent* ev)
{
	emit activated();
	AbstractPopupWidget::timerEvent(ev);
}

QSize TextPopupWidget::sizeHint() const
{
	int width = popup_settings.defaultSize.width();
	int height = static_cast<int>(m_browser->document()->size().height());
	return QSize(width,height);
}

}
}





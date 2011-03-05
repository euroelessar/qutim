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

namespace Core
{
namespace AdiumChat
{
using namespace qutim_sdk_0_3;

ChatEmoticonsWidget::ChatEmoticonsWidget(QWidget *parent) :
	QScrollArea(parent)
{
#ifndef Q_WS_MAEMO_5
	resize(400,400);
#else
	move(80,0);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#endif

#ifndef QUTIM_MOBILE_UI
	setMinimumSize(size());
	setFrameStyle(QFrame::NoFrame);
#endif
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QWidget *w = new QWidget(this);
	setWidget(w);
	new FlowLayout(w);

	QAction *action = new QAction(tr("Close"),this);
	action->setSoftKeyRole(QAction::NegativeSoftKey);
	connect(action, SIGNAL(triggered()), SLOT(close()));
	addAction(action);
}

void ChatEmoticonsWidget::loadTheme()
{
	QHash<QString, QStringList> theme_map = Emoticons::theme().emoticonsMap();
	QHash<QString, QStringList>::const_iterator it;
	clearEmoticonsPreview();
	for (it = theme_map.constBegin();it != theme_map.constEnd();it ++) {
		QLabel *label = new QLabel();
		label->setFocusPolicy(Qt::StrongFocus);
		QMovie *emoticon = new QMovie (it.key(), QByteArray(), label);
		label->setMovie(emoticon);
		label->setToolTip(it.value().first());
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

void ChatEmoticonsWidget::showEvent(QShowEvent *)
{

	foreach (QWidget *widget,m_active_emoticons) {
		QLabel *label = static_cast<QLabel *>(widget);
		label->movie()->start(); //FIXME on s60 retarding and eats battery
	}
#ifndef QUTIM_MOBILE_UI
	FlowLayout *layout = static_cast<FlowLayout *>(widget()->layout());
	widget()->resize(width(),layout->heightForWidth(width()));
#endif
}

void ChatEmoticonsWidget::hideEvent(QHideEvent *)
{
	foreach (QWidget *widget,m_active_emoticons) {
		QLabel *label = static_cast<QLabel *>(widget);
		label->movie()->stop();
	}
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
#ifndef QUTIM_MOBILE_UI
	QMenu *menu = new QMenu(parentWidget());
	setMenu(menu);
	QWidgetAction *emoticons_widget_act = new QWidgetAction(this);
	m_emoticons_widget = new ChatEmoticonsWidget(menu);
	m_emoticons_widget->loadTheme();
	emoticons_widget_act->setDefaultWidget(m_emoticons_widget);
	menu->addAction(emoticons_widget_act);
	connect(m_emoticons_widget, SIGNAL(insertSmile(QString)),
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
void EmoAction::triggerEmoticons()
{
	if (!m_emoticons_widget) {
		m_emoticons_widget = new ChatEmoticonsWidget();
#ifdef Q_WS_MAEMO_5
		emoticons_widget->setParent(qApp->activeWindow());
#endif
		m_emoticons_widget->loadTheme();
		connect(m_emoticons_widget, SIGNAL(insertSmile(QString)),
				this,SLOT(onInsertSmile(QString)));
	}

	if (m_emoticons_widget->isVisible()) {
		m_emoticons_widget->hide();
	}
	else {
#ifdef Q_WS_MAEMO_5
		QRect screenGeometry = QApplication::desktop()->screenGeometry();
		if (screenGeometry.width() > screenGeometry.height()) {
			//smith, please use relative coordinates
			emoticons_widget->resize(emoticons_widget->parentWidget()->width()-160,emoticons_widget->parentWidget()->height()-130);
		}
		else {
			emoticons_widget->resize(emoticons_widget->parentWidget()->width()-160,emoticons_widget->parentWidget()->height()/2-80);
		}
		emoticons_widget->show();
#else
		SystemIntegration::show(m_emoticons_widget);
#endif
	}
}

}
}

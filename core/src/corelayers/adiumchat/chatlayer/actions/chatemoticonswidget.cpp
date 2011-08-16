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
#ifdef Q_WS_MAEMO_5
void EmoAction::orientationChanged()
{
	if (m_emoticons_widget->isVisible())
	{
		QRect screenGeometry = QApplication::desktop()->screenGeometry();
		if (screenGeometry.width() > screenGeometry.height())
		{
			//This crap need to completely remake!
			m_emoticons_widget->hide();
			m_emoticons_widget->resize(m_emoticons_widget->parentWidget()->width()-160,m_emoticons_widget->parentWidget()->height()-130);
			m_emoticons_widget->show();
		}else{
			m_emoticons_widget->hide();
			m_emoticons_widget->resize(m_emoticons_widget->parentWidget()->width()-160,m_emoticons_widget->parentWidget()->height()/2);
			m_emoticons_widget->show();
		}
	}
}
#endif

void EmoAction::triggerEmoticons()
{
#ifdef Q_WS_MAEMO_5
	if (!m_emoticons_widget) {
		m_emoticons_widget = new ChatEmoticonsWidget();
		m_emoticons_widget->loadTheme();
		connect(m_emoticons_widget, SIGNAL(insertSmile(QString)),
				this,SLOT(onInsertSmile(QString)));
                m_emoticons_widget->setParent(qApp->activeWindow());
	}
	if (m_emoticons_widget->isVisible()) {
		m_emoticons_widget->hide();
	} else {
		QRect screenGeometry = QApplication::desktop()->screenGeometry();
		if (screenGeometry.width() > screenGeometry.height())
		{
			//This crap need to completely remake!
			m_emoticons_widget->resize(m_emoticons_widget->parentWidget()->width()-160,m_emoticons_widget->parentWidget()->height()-130);
		}else{
			m_emoticons_widget->resize(m_emoticons_widget->parentWidget()->width()-160,m_emoticons_widget->parentWidget()->height()/2);
		}
		m_emoticons_widget->show();
	}
#else
	if (!m_emoticons_widget) {
		m_emoticons_widget = new ChatEmoticonsWidget();
		m_emoticons_widget->loadTheme();
		connect(m_emoticons_widget, SIGNAL(insertSmile(QString)),
				this,SLOT(onInsertSmile(QString)));
		SystemIntegration::show(m_emoticons_widget);
		m_emoticons_widget->play();

	} else
		m_emoticons_widget->deleteLater();
#endif

}

}
}


#include "chatemoticonswidget.h"
#include <QLabel>
#include <QHash>
#include <QStyle>
#include <QMovie>
#include "3rdparty/flowlayout/flowlayout.h"
#include <qutim/emoticons.h>
#include <QEvent>
#include <qutim/debug.h>
#include <QWidgetAction>
#include <QMenu>

namespace Core
{
namespace AdiumChat
{
using namespace qutim_sdk_0_3;

ChatEmoticonsWidget::ChatEmoticonsWidget(QWidget *parent) :
	QScrollArea(parent)
{
#ifndef Q_WS_MAEMO_5
	resize(400,200);
#else
	resize(700,300);
#endif
	setMinimumSize(size());
	setFrameStyle(QFrame::NoFrame);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setWidget(new QWidget(this));

	new FlowLayout(widget());
}

void ChatEmoticonsWidget::loadTheme()
{
	QHash<QString, QStringList> theme_map = Emoticons::theme().emoticonsMap();
	QHash<QString, QStringList>::const_iterator it;
	clearEmoticonsPreview();
	for (it = theme_map.constBegin();it != theme_map.constEnd();it ++) {
		QLabel *label = new QLabel();
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
		label->movie()->start();
	}
	FlowLayout *layout = static_cast<FlowLayout *>(widget()->layout());
	widget()->resize(width(),layout->heightForWidth(width()));
	debug() << layout->heightForWidth(width());
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
		if (event->type() == QEvent::MouseButtonPress)
			emit insertSmile(label->toolTip());
	}
	return QObject::eventFilter(obj, event);
}

EmoAction::EmoAction(QObject *parent) :
	QAction(parent)
{
	QMenu *menu = new QMenu();
	setMenu(menu);
	QWidgetAction *emoticons_widget_act = new QWidgetAction(this);
	ChatEmoticonsWidget *emoticons_widget = new ChatEmoticonsWidget(menu);
	emoticons_widget->loadTheme();
	emoticons_widget_act->setDefaultWidget(emoticons_widget);
	menu->addAction(emoticons_widget_act);
	connect(emoticons_widget, SIGNAL(insertSmile(QString)),
			this,SLOT(onInsertSmile(QString)));
}

void EmoAction::onInsertSmile(const QString &code)
{
	setProperty("emoticon",code);
	emit triggered();
}

}
}

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

#include "chatlayerplugin.h"
#include "chatlayerimpl.h"
#include "chatsessionimpl.h"
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <qutim/debug.h>
#include <QWidgetAction>
#include "actions/chatemoticonswidget.h"
#include <QPlainTextEdit>
#include "chatforms/abstractchatwidget.h"
#include <qutim/shortcut.h>
#include <qutim/emoticons.h>
#include "chatforms/abstractchatform.h"
#include "quoterwidget.h"

namespace Core
{
namespace AdiumChat
{

template<typename T>
T findParent(QObject *w)
{
	while(w) {
		if(T parent = qobject_cast<T>(w))
			return parent;
		w = w->parent();
	}
	return 0;
}

class EmoActionGenerator : public ActionGenerator
{
public:
	EmoActionGenerator(QObject *obj) :
		ActionGenerator(Icon("face-smile"),QT_TRANSLATE_NOOP("ChatLayer", "Insert Emoticon"),obj,SLOT(onInsertEmoticon(QAction*,QObject*)))
	{
		setType(ActionTypeChatButton);
	}
	
	void showImpl(QAction *action, QObject *)
	{
		debug() << Q_FUNC_INFO;
		action->setVisible(!Emoticons::theme().isNull());
	}

protected:
	virtual QObject *generateHelper() const
	{
		EmoAction *emo = new EmoAction();
		prepareAction(emo);
		return emo;
	}
};

using namespace qutim_sdk_0_3;

void ChatLayerPlugin::init()
{
	LocalizedString name = QT_TRANSLATE_NOOP("Plugin", "Webkit chat layer");
	LocalizedString description = QT_TRANSLATE_NOOP("Plugin", "Default qutIM chat implementation, based on Adium chat styles");
	setInfo(name, description, QUTIM_VERSION);
	setCapabilities(Loadable);
	addExtension<Core::AdiumChat::ChatLayerImpl>(name, description);
}

bool ChatLayerPlugin::load()
{
	AbstractChatForm *form = ServiceManager::getByName<AbstractChatForm*>("ChatForm");
	if (!form) {
		return false;
	}

	Shortcut::registerSequence("showEmoticons",
							   QT_TRANSLATE_NOOP("ChatLayer", "Show Emoticons"),
							   "ChatWidget",
							   QKeySequence("alt+e")
							   );
	Shortcut::registerSequence("quote",
							   QT_TRANSLATE_NOOP("ChatLayer", "Quote"),
							   "ChatWidget",
							   QKeySequence("alt+q")
							   );
	Shortcut::registerSequence("clearChat",
							   QT_TRANSLATE_NOOP("ChatLayer", "Clear Chat"),
							   "ChatWidget",
							   QKeySequence("alt+c")
							   );

	ActionGenerator *gen = new EmoActionGenerator(this);
	gen->setShortcut(QLatin1String("showEmoticons"));
	form->addAction(gen);

	gen = new ActionGenerator(Icon("insert-text-quote"),
							  QT_TRANSLATE_NOOP("ChatLayer","Quote"),
							  this,
							  SLOT(onQuote(QObject*)));
	gen->setShortcut(QLatin1String("quote"));
	form->addAction(gen);

	gen = new ActionGenerator(Icon("edit-clear-list"),
							  QT_TRANSLATE_NOOP("ChatLayer","Clear chat"),
							  this,
							  SLOT(onClearChat(QObject*)));
	gen->setToolTip(QT_TRANSLATE_NOOP("ChatLayer","Clear chat field"));
	gen->setShortcut(QLatin1String("clearChat"));
	form->addAction(gen);

	return true;
}

bool ChatLayerPlugin::unload()
{
	return true;
}

void ChatLayerPlugin::onClearChat(QObject *controller)
{
	if(AbstractChatWidget *chat = findParent<AbstractChatWidget*>(controller))
		chat->currentSession()->clearChat();
}

void ChatLayerPlugin::onInsertEmoticon(QAction *act,QObject *controller)
{
	QString str = act->property("emoticon").toString();
	if(str.isEmpty())
		return;
	if(AbstractChatWidget *chat = findParent<AbstractChatWidget*>(controller))
		chat->getInputField()->insertPlainText(str);
}

void ChatLayerPlugin::onQuote(QObject *controller)
{
	AbstractChatWidget *chat = findParent<AbstractChatWidget*>(controller);
	if (!chat)
		return;
	ChatSessionImpl *session = chat->currentSession();
	const QString quote = session->quote();
	if (quote.isEmpty()) {
		const MessageList messages = session->lastMessages();
		debug() << messages.size();
		if (!messages.isEmpty()) {
			QuoterWidget *widget = new QuoterWidget(messages, controller);
			connect(widget, SIGNAL(quoteChoosed(QString,QObject*)), SLOT(onQuote(QString,QObject*)));
		}
		return;
	}
	onQuote(quote, controller);
}

void ChatLayerPlugin::onQuote(const QString &quote, QObject *controller)
{
	AbstractChatWidget *chat = findParent<AbstractChatWidget*>(controller);
	if (!chat)
		return;
	const QString newLine = QLatin1String("\n> ");
	QString text;
	if (chat->getInputField()->textCursor().atStart())
		text = QLatin1String("> ");
	else
		text = newLine;
	text.reserve(text.size() + quote.size() * 1.2);
	for (int i = 0; i < quote.size(); ++i) {
		if (quote[i] == QLatin1Char('\n') || quote[i].unicode() == QChar::ParagraphSeparator)
			text += newLine;
		else
			text += quote[i];
	}
	text += QLatin1Char('\n');
	chat->getInputField()->insertPlainText(text);
}

}
}

QUTIM_EXPORT_PLUGIN(Core::AdiumChat::ChatLayerPlugin)


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

#include "chatlayerplugin.h"
#include "chatlayerimpl.h"
#include "timemodifier/timemodifier.h"
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <qutim/debug.h>
#include <QWidgetAction>
#include "actions/chatemoticonswidget.h"
#include <QPlainTextEdit>
#include "chatforms/abstractchatwidget.h"
#include <qutim/shortcut.h>
#include "chatforms/abstractchatform.h"

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
	addExtension<Core::AdiumChat::TimeModifier,Core::AdiumChat::MessageModifier>(
				QT_TRANSLATE_NOOP("Plugin", "Time modifier"),
				QT_TRANSLATE_NOOP("Plugin", "Modifier for %time% and %timeOpened% param, based on Apple's documentation"));

	//temporary
	addAuthor(QT_TRANSLATE_NOOP("Author", "Perova Gala"),
			  QT_TRANSLATE_NOOP("Task", "Artist"),
			  QLatin1String("naitmara@mail.ru"),
			  QLatin1String("http://dawsnest.ru"));
	addAuthor(QT_TRANSLATE_NOOP("Author", "Rederick Asher"),
			  QT_TRANSLATE_NOOP("Task", "Artist"),
			  QLatin1String("chaoticblack@gmail.com"),
			  QLatin1String("litsovet.ru"));
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
	if(AbstractChatWidget *chat = findParent<AbstractChatWidget*>(controller)) {
		ChatSessionImpl *session = chat->currentSession();
		QString quote = session->quote();
		if (quote.isEmpty())
			return;
		quote = QLatin1String(chat->getInputField()->textCursor().atStart() ? "> " : "\n> ") +
				quote.replace('\n', "\n> ") +
				QLatin1Char('\n');
		chat->getInputField()->insertPlainText(quote);
	}
}

}
}

QUTIM_EXPORT_PLUGIN(Core::AdiumChat::ChatLayerPlugin)

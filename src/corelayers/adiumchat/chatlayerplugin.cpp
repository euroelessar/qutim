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
#include "chatlayer/chatlayerimpl.h"
#include "tabbedchatform/tabbedchatform.h"
#include "timemodifier/timemodifier.h"
#include "settings/chatbehavior.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <qutim/debug.h>
#include "webkitchat/webkitviewfactory.h"

namespace Core
{
namespace AdiumChat
{

class EmoActionGenerator : public ActionGenerator
{
public:
	EmoActionGenerator(QObject *obj) :
		ActionGenerator(Icon("face-smile"),QT_TRANSLATE_NOOP("ChatLayer", "Insert Emoticon"),obj,SLOT(onInsertEmoticon(QObject*)))
	{
		setType(ActionTypeChatButton);
	}
protected:
	virtual QObject *generateHelper() const
	{
		QAction *action = prepareAction(new QAction(0));
		return action;
	}
};

static SettingsItem *behaviourSettings = 0;

using namespace qutim_sdk_0_3;

void ChatLayerPlugin::init()
{
	LocalizedString name = QT_TRANSLATE_NOOP("Plugin", "Webkit chat layer");
	LocalizedString description = QT_TRANSLATE_NOOP("Plugin", "Default qutIM chat implementation, based on Adium chat styles");
	setInfo(name, description, QUTIM_VERSION);
	addExtension<Core::AdiumChat::ChatLayerImpl>(name, description);
	addExtension<Core::AdiumChat::TimeModifier,Core::AdiumChat::MessageModifier>(
				QT_TRANSLATE_NOOP("Plugin", "Time modifier"),
				QT_TRANSLATE_NOOP("Plugin", "Modifier for %time% and %timeOpened% param, based on Apple's documentation"));
	addExtension<Core::AdiumChat::TabbedChatForm>(
				QT_TRANSLATE_NOOP("Plugin", "Classic"),
				QT_TRANSLATE_NOOP("Plugin", "Default qutIM chatform implementation for adiumchat"));
	addExtension<Core::AdiumChat::WebkitViewFactory>(
				QT_TRANSLATE_NOOP("Plugin", "Webkit"),
				QT_TRANSLATE_NOOP("Plugin", "Default qutIM chat implementation, based on Adium chat styles"));
}

bool ChatLayerPlugin::load()
{
	AbstractChatForm *form = ServiceManager::getByName<AbstractChatForm*>("ChatForm");
	if (!form) {
		return false;
	}

	ActionGenerator *gen = new ActionGenerator(Icon("edit-clear-list"),
											   QT_TRANSLATE_NOOP("ChatLayer","Clear chat"),
											   this,
											   SLOT(onClearChat(QObject*)));
	gen->setToolTip(QT_TRANSLATE_NOOP("ChatLayer","Clear chat field"));
	form->addAction(gen);

	//DEPRECATED
	behaviourSettings = new GeneralSettingsItem<ChatBehavior>(Settings::General, Icon("view-choose"),
															  QT_TRANSLATE_NOOP("Settings","Chat"));
	behaviourSettings->connect(SIGNAL(saved()), form, SLOT(onBehaviorSettingsChanged()));
	Settings::registerItem(behaviourSettings);
	return true;
}

bool ChatLayerPlugin::unload()
{
	Settings::removeItem(behaviourSettings);
	delete behaviourSettings;
	return true;
}

void ChatLayerPlugin::onClearChat(QObject *controller)
{
	QWidget *widget = qobject_cast<QWidget*>(controller);
	while(widget) {
		if(AbstractChatWidget *chat = qobject_cast<AbstractChatWidget*>(widget)) {
			chat->currentSession()->clearChat();
		}
		widget = widget->parentWidget();
	}
}

}
}

QUTIM_EXPORT_PLUGIN(Core::AdiumChat::ChatLayerPlugin)

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

Array.prototype.contains = function (element) {
  for (var i = 0; i < this.length; i++) {
    if (this[i] == element)
      return true;
  }
  return false;
}

var plugin = {
  name: QT_TRANSLATE_NOOP("Plugin", "Ignorer"),
  description: QT_TRANSLATE_NOOP("Plugin", "Ignores messages from certain persons"),
  authors: [
    {
      name: QT_TRANSLATE_NOOP("Author", "Ruslan Nigmatullin"),
      task: QT_TRANSLATE_NOOP("Task", "Developer"),
      email: "euroelessar@ya.ru"
    }
  ],
  personsId: [],
  load: function() {
    client.settings.register({
      type: client.settings.Plugin,
      text: QT_TRANSLATE_NOOP("Plugin", "Ignorer"),
      entries: [
	{
	  type: "PlainTextEdit",
	  name: "persons",
	  text: QT_TRANSLATE_NOOP("Ignorer", "Evil ids")
	}
      ],
      onSaved: plugin.loadSettings
    });
    this.loadSettings();
    client.debug("You want to ignore this persons:", this.personsId);
    client.chatLayer.sessionCreated.connect(this.onSessionCreated);
    return true;
  },
  unload: function() {
    client.chatLayer.sessionCreated.disconnect(this.onSessionCreated);
    return true;
  },
  loadSettings: function() {
    var cfg = client.settings.value;
    plugin.personsId = (cfg.persons + "").split('\n');
  },
  onSessionCreated: function (session) {
    session.messageReceived.connect(plugin.onMessageReceived);
  },
  onMessageReceived: function (message) {
    if (plugin.personsId.contains(message.chatUnit.id)
      || plugin.personsId.contains(message.senderId)) {
      message.hide = true;
    }
  }
}
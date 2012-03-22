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

#include "quetzalplugin.h"
#include "quetzaleventloop.h"
#include "quetzalprotocol.h"
#include "quetzalaccount.h"
#include "quetzalconverstion.h"
#include "quetzalchat.h"
#include "quetzalrequest.h"
#include "quetzaljoinchatdialog.h"
#include "quetzalblist.h"
#include "quetzalnotify.h"
#include <qutim/event.h>
#include <qutim/systeminfo.h>
#include <purple.h>
#include <qutim/chatsession.h>
#include <qutim/debug.h>
#include <QCoreApplication>
#include <QLibrary>
#include <qutim/icon.h>
#include <qutim/notification.h>
#include <QAbstractEventDispatcher>
#include <QDateTime>
#include <QThread>
#include <QtConcurrentRun>

struct QuetzalConversationHandler
{
	typedef QSharedPointer<QuetzalConversationHandler> Ptr;
	
	~QuetzalConversationHandler()
	{
		foreach (PurpleConversation *conversation, conversations) {
			conversation->ui_data = 0;
			purple_conversation_destroy(conversation);
		}
	}
	QWeakPointer<ChatSession> isAlive;
	QList<PurpleConversation*> conversations;
};

Q_DECLARE_METATYPE(QuetzalConversationHandler::Ptr)

void quetzal_menu_dump(PurpleMenuAction *action, int offset)
{
	QByteArray off;
	for (int i = 0; i < offset; i++)
		off += "-";
	debug() << off << action->label;
	for (GList *it = action->children; it; it = it->next)
		quetzal_menu_dump((PurpleMenuAction *)it->data, offset + 1);
}

void quetzal_create_conversation(PurpleConversation *conv)
{
	if (conv->ui_data)
		return;
	QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(conv->account->ui_data);
	debug() << acc;
	ChatUnit *unit = acc->getUnit(conv->name);
	debug() << Q_FUNC_INFO << conv->name;
//	debug() << Q_FUNC_INFO << unit;
	if (conv->type == PURPLE_CONV_TYPE_IM) {
//		PurpleConvIm *data = purple_conversation_get_im_data(conv);
		const char *id = conv->name;
		PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(acc->purple()->gc->prpl);
		if (info->normalize)
			id = info->normalize(acc->purple(), conv->name);
		unit = acc->getUnit(id, true);
		ChatSession *session = ChatLayer::get(unit, true);
		QuetzalConversationHandler::Ptr handler =
				session->property("quetzal_handler").value<QuetzalConversationHandler::Ptr>();
		if (!handler) {
			handler = QuetzalConversationHandler::Ptr::create();
			handler->isAlive = session;
			session->setProperty("quetzal_handler", qVariantFromValue(handler));
		}
		conv->ui_data = handler.data();
//		qDebug() << conv->name << info->normalize(acc->purple(), conv->name);
		handler->conversations << conv;
//		unit = new QuetzalConversation(conv);
	} else if (!unit) {
		unit = new QuetzalChat(conv);
		acc->addChatUnit(unit);
	}
	
	/*else if (QuetzalContact *contact = qobject_cast<QuetzalContact *>(unit)) {
//			purple_blist_node_get_extended_menu
			debug() << Q_FUNC_INFO;
			GList *menu = PURPLE_PLUGIN_PROTOCOL_INFO(conv->account->gc->prpl)
						  ->blist_node_menu((PurpleBlistNode *)contact->buddy());
			for (GList *it = menu; it; it = it->next) {
				PurpleMenuAction *action = (PurpleMenuAction *)it->data;
				quetzal_menu_dump(action, 0);
			}
	}*/
}

void quetzal_destroy_conversation(PurpleConversation *conv)
{
	QuetzalConversationHandler *handler = reinterpret_cast<QuetzalConversationHandler*>(conv->ui_data);
	if (handler)
		handler->conversations.removeOne(conv);
	debug() << Q_FUNC_INFO << conv->name;
//	QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(conv->account->ui_data);
////	if (conv->type == PURPLE_CONV_TYPE_IM) {
//		ChatUnit *unit = reinterpret_cast<ChatUnit *>(conv->ui_data);
//		if (unit) {
//			ChatSession *session = ChatLayer::get(unit, false);
//			session->setActive(false);
//			if (!(qobject_cast<QuetzalContact *>(unit))) {
//				acc->removeChatUnit(unit);
//				unit->deleteLater();
//			}
//		}
////	}
}

Message quetzal_convert_message(const char *message, PurpleMessageFlags flags, time_t mtime)
{
	Message mess;
	debug() << QString::number(uint(flags), 16);
	if (!(flags & PURPLE_MESSAGE_RAW)) {
		char *plain_text = purple_markup_strip_html(message);
		mess.setText(plain_text);
		mess.setProperty("html", QString(message));
		g_free(plain_text);
	} else {
		mess.setText(message);
	}
	mess.setTime(QDateTime::fromTime_t(mtime));
	mess.setIncoming(flags & (PURPLE_MESSAGE_RECV | PURPLE_MESSAGE_SYSTEM));
	if (flags & PURPLE_MESSAGE_SYSTEM)
		mess.setProperty("service", true);
	if (flags & PURPLE_MESSAGE_NO_LOG)
		mess.setProperty("store", false);
	return mess;
}

void quetzal_write_chat(PurpleConversation *conv, const char *who,
				   const char *message, PurpleMessageFlags flags,
				   time_t mtime)
{
	debug() << Q_FUNC_INFO << who;
	ChatUnit *unit = reinterpret_cast<ChatUnit *>(conv->ui_data);
	if (QuetzalChat *chat = qobject_cast<QuetzalChat *>(unit)) {
		PurpleConvChat *data = PURPLE_CONV_CHAT(chat->purple());
		// Version older then 2.7.10
		// do always
//		if (purple_version_check(2, 7, 10) && 
		if (g_str_equal(conv->account->protocol_id, "prpl-jabber")) {
			if (!(flags & (PURPLE_MESSAGE_RECV | PURPLE_MESSAGE_SYSTEM)) && who && *who) {
				// It looks like our outgoing message
				chat->setMe(who);
			}
		}
		Message mess = quetzal_convert_message(message, flags, mtime);
		if ((!(flags & PURPLE_MESSAGE_DELAYED)) && !mess.isIncoming())
			return;
		if (!mess.text().contains(data->nick))
			mess.setProperty("silent", true);
		mess.setChatUnit(chat);
		mess.setProperty("senderName", QString::fromUtf8(who));
		ChatLayer::get(unit, true)->appendMessage(mess);
	} else {
		Q_ASSERT(!"Some strange situation.. Every Chat unit must be an QuetzalChat");
	}
}

void quetzal_write_im(PurpleConversation *conv, const char *who,
				 const char *message, PurpleMessageFlags flags,
				 time_t mtime)
{
	if (!conv->ui_data) {
		// Is it possible?
		quetzal_create_conversation(conv);
	}
	QuetzalConversationHandler *handler = reinterpret_cast<QuetzalConversationHandler *>(conv->ui_data);
	debug() << Q_FUNC_INFO << who << handler;
	ChatUnit *unit = handler->isAlive.data()->unit();
	Message mess = quetzal_convert_message(message, flags, mtime);
	if (!mess.isIncoming())
		return;
	mess.setChatUnit(unit);
	handler->isAlive.data()->appendMessage(mess);
}

void quetzal_write_conv(PurpleConversation *conv,
				   const char *name,
				   const char *alias,
				   const char *text,
				   PurpleMessageFlags flags,
				   time_t mtime)
{
	debug() << Q_FUNC_INFO << name << conv->account->username;
	ChatUnit *unit;
	if (conv->type == PURPLE_CONV_TYPE_IM)
		unit = reinterpret_cast<QuetzalConversationHandler *>(conv->ui_data)->isAlive.data()->unit();
	else
		unit = reinterpret_cast<ChatUnit *>(conv->ui_data);
	Message message = quetzal_convert_message(text, flags, mtime);
	debug() << name << alias;
	if (!message.isIncoming())
		return;
	message.setChatUnit(unit);
	ChatLayer::get(unit, true)->appendMessage(message);
}

void quetzal_chat_add_users(PurpleConversation *conv,
					   GList *cbuddies,
					   gboolean new_arrivals)
{
	ChatUnit *unit = reinterpret_cast<ChatUnit *>(conv->ui_data);
	if (QuetzalChat *chat = qobject_cast<QuetzalChat *>(unit)) {
		chat->addUsers(cbuddies, new_arrivals);
	}
}

void quetzal_chat_rename_user(PurpleConversation *conv, const char *old_name,
						 const char *new_name, const char *new_alias)
{
	ChatUnit *unit = reinterpret_cast<ChatUnit *>(conv->ui_data);
	if (QuetzalChat *chat = qobject_cast<QuetzalChat *>(unit)) {
		chat->renameUser(old_name, new_name, new_alias);
	}
}

void quetzal_chat_remove_users(PurpleConversation *conv, GList *users)
{
	ChatUnit *unit = reinterpret_cast<ChatUnit *>(conv->ui_data);
	if (QuetzalChat *chat = qobject_cast<QuetzalChat *>(unit)) {
		chat->removeUsers(users);
	}
}

void quetzal_chat_update_user(PurpleConversation *conv, const char *user)
{
	ChatUnit *unit = reinterpret_cast<ChatUnit *>(conv->ui_data);
	if (QuetzalChat *chat = qobject_cast<QuetzalChat *>(unit)) {
		chat->updateUser(user);
	}
}

void quetzal_present(PurpleConversation *conv)
{
	Q_UNUSED(conv);
}

gboolean quetzal_has_focus(PurpleConversation *conv)
{
	ChatSession *session;
	if (conv->type == PURPLE_CONV_TYPE_IM)
		session = reinterpret_cast<QuetzalConversationHandler *>(conv->ui_data)->isAlive.data();
	else
		session = ChatLayer::get(reinterpret_cast<ChatUnit*>(conv->ui_data), false);
	return session && session->isActive();
}

gboolean quetzal_custom_smiley_add(PurpleConversation *conv, const char *smile, gboolean remote)
{
	Q_UNUSED(conv);
	Q_UNUSED(smile);
	Q_UNUSED(remote);
	return FALSE;
}

void quetzal_custom_smiley_write(PurpleConversation *conv, const char *smile,
							const guchar *data, gsize size)
{
	Q_UNUSED(conv);
	Q_UNUSED(smile);
	Q_UNUSED(data);
	Q_UNUSED(size);
}

void quetzal_custom_smiley_close(PurpleConversation *conv, const char *smile)
{
	Q_UNUSED(conv);
	Q_UNUSED(smile);
}

void quetzal_send_confirm(PurpleConversation *conv, const char *message)
{
	Q_UNUSED(conv);
	Q_UNUSED(message);
}


static PurpleConversationUiOps quetzal_conv_uiops =
{
	quetzal_create_conversation,  /* create_conversation  */
	quetzal_destroy_conversation, /* destroy_conversation */
	quetzal_write_chat,           /* write_chat           */
	quetzal_write_im,             /* write_im             */
	quetzal_write_conv,           /* write_conv           */
	quetzal_chat_add_users,       /* chat_add_users       */
	quetzal_chat_rename_user,     /* chat_rename_user     */
	quetzal_chat_remove_users,    /* chat_remove_users    */
	quetzal_chat_update_user,     /* chat_update_user     */
	NULL,                         /* present              */
	quetzal_has_focus,            /* has_focus            */
	NULL,                         /* custom_smiley_add    */
	NULL,                         /* custom_smiley_write  */
	NULL,                         /* custom_smiley_close  */
	NULL,                         /* send_confirm         */
	NULL,
	NULL,
	NULL,
	NULL
};

void quetzal_debug_print(PurpleDebugLevel level, const char *category, const char *arg_s)
{
	// Why should I read all xml staff in console?..
	if (g_str_equal(category, "jabber") && level < PURPLE_DEBUG_WARNING)
		return;
	QByteArray arg(arg_s);
	arg.chop(1);
	if (level >= PURPLE_DEBUG_FATAL)
		fatal() << "[quetzal/" << category << "]: " << arg.constData();
	else if (level >= PURPLE_DEBUG_ERROR)
		critical() << "[quetzal/" << category << "]: " << arg.constData();
	else if (level >= PURPLE_DEBUG_WARNING)
		warning() << "[quetzal/" << category << "]: " << arg.constData();
	else
		debug() << "[quetzal/" << category << "]: " << arg.constData();
}

gboolean quetzal_debug_is_enabled(PurpleDebugLevel level, const char *category)
{
	Q_UNUSED(level);
	Q_UNUSED(category);
	return TRUE;
}

static PurpleDebugUiOps quetzal_debug_uiops =
{
	quetzal_debug_print,
	quetzal_debug_is_enabled,
	NULL,
	NULL,
	NULL,
	NULL
};

void quetzal_notify_added(PurpleAccount *account,
						  const char *remote_user,
						  const char *id,
						  const char *alias,
						  const char *message)
{
	Q_UNUSED(account);
	Q_UNUSED(remote_user);
	Q_UNUSED(id);
	Q_UNUSED(alias);
	Q_UNUSED(message);
}

void quetzal_status_changed(PurpleAccount *account,
							PurpleStatus *status)
{
	QuetzalAccount *acc = (QuetzalAccount *)account->ui_data;
	if (acc)
		acc->setStatusChanged(status);
	debug() << Q_FUNC_INFO << account->username << account->alias << purple_status_get_name(status);
}

void quetzal_request_add(PurpleAccount *account,
						 const char *remote_user,
						 const char *id,
						 const char *alias,
						 const char *message)
{
	Q_UNUSED(account);
	Q_UNUSED(remote_user);
	Q_UNUSED(id);
	Q_UNUSED(alias);
	Q_UNUSED(message);
}

void *quetzal_request_authorize(PurpleAccount *account,
								const char *remote_user,
								const char *id,
								const char *alias,
								const char *message,
								gboolean on_list,
								PurpleAccountRequestAuthorizationCb authorize_cb,
								PurpleAccountRequestAuthorizationCb deny_cb,
								void *user_data)
{
	Q_UNUSED(account);
	Q_UNUSED(remote_user);
	Q_UNUSED(id);
	Q_UNUSED(alias);
	Q_UNUSED(message);
	Q_UNUSED(on_list);
	Q_UNUSED(authorize_cb);
	Q_UNUSED(deny_cb);
	Q_UNUSED(user_data);
	return 0;
}

void quetzal_close_account_request(void *ui_handle)
{
	Q_UNUSED(ui_handle);
}

PurpleAccountUiOps quetzal_accounts_uiops =
{
	quetzal_notify_added,
	quetzal_status_changed,
	quetzal_request_add,
	quetzal_request_authorize,
	quetzal_close_account_request,
	NULL,
	NULL,
	NULL,
	NULL
};

//PurpleConnectionUiOps;
//PurpleDnsQueryUiOps;
//PurpleXferUiOps;
//PurpleIdleUiOps;
//PurpleNotifyUiOps;
//PurplePrivacyUiOps;
//PurpleRequestUiOps;
//PurpleSoundUiOps;
//PurpleSslOps;
//PurpleWhiteboardUiOps;
//PurpleAccountUiOps;
//PurpleRoomlistUiOps;
//PurpleBlistUiOps;
//PurpleConversationUiOps;
//PurplePluginUiInfo;
//PurpleCertificateVerifier

//static void quetzal_account_status_changed(PurpleAccount *account, PurpleStatus *old_status, PurpleStatus *new_status)
//{
//	Q_UNUSED(old_status);
////	int g;
////	QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(account->ui_data);
////	if (acc)
////		acc->setStatusChanged(new_status);
//}

static void quetzal_conversation_update(PurpleConversation *conv, PurpleConvUpdateType type)
{
	if (conv->type == PURPLE_CONV_TYPE_CHAT) {
		QObject *unit = reinterpret_cast<QObject *>(conv->ui_data);
		if (QuetzalChat *chat = qobject_cast<QuetzalChat *>(unit)) {
			chat->update(type);
		}
	}
}

//static void quetzal_account_signon_cb(PurpleConnection *gc, gpointer z)
//{
//	Q_UNUSED(gc);
//	Q_UNUSED(z);
////	PurpleAccount *acc = purple_connection_get_account(gc);
////	QuetzalAccount *account = reinterpret_cast<QuetzalAccount *>(acc->ui_data);
////	QDialog *dialog = new QuetzalJoinChatDialog(gc);
////	dialog->show();
////	GHashTable *comps = NULL;
////	PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(gc->prpl);
////	if (info->chat_info_defaults != NULL)
////		comps = info->chat_info_defaults(gc, "talks@conference.qutim.org/Yahoo");
////	serv_join_chat(gc, comps);
//}

class QuetzalConnectionKillerEvent : public QEvent
{
public:
	QuetzalConnectionKillerEvent(PurpleAccount *account) 
	    : QEvent(eventType()), m_account(account)
	{
	}
	
	~QuetzalConnectionKillerEvent()
	{
		purple_account_disconnect(m_account);
	}

	static Type eventType()
	{
		static Type type = static_cast<Type>(registerEventType());
		return type;
	}
	
private:
	PurpleAccount *m_account;
};

void quetzal_connection_signing_on_cb(PurpleConnection *gc)
{
	PurpleAccount *acc = purple_connection_get_account(gc);
	QObject *object = reinterpret_cast<QObject*>(acc->ui_data);
	if (QuetzalAccount *account = qobject_cast<QuetzalAccount *>(object)) {
		account->handleSigningOn();
	} else {
		QEvent *ev = new QuetzalConnectionKillerEvent(acc);
		QCoreApplication::postEvent(QuetzalEventLoop::instance(), ev, -1);
	}
}

void quetzal_connection_signed_on_cb(PurpleConnection *gc)
{
	PurpleAccount *acc = purple_connection_get_account(gc);
	QObject *object = reinterpret_cast<QObject*>(acc->ui_data);
	if (QuetzalAccount *account = qobject_cast<QuetzalAccount *>(object))
		account->handleSignedOn();
}

void quetzal_connection_signed_off_cb(PurpleConnection *gc)
{
	PurpleAccount *acc = purple_connection_get_account(gc);
	QObject *object = reinterpret_cast<QObject*>(acc->ui_data);
	if (QuetzalAccount *account = qobject_cast<QuetzalAccount *>(object))
		account->handleSignedOff();
}

static void quetzal_ui_init(void)
{
	purple_debug_set_ui_ops(&quetzal_debug_uiops);
	purple_conversations_set_ui_ops(&quetzal_conv_uiops);
	purple_blist_set_ui_ops(&quetzal_blist_uiops);
	purple_accounts_set_ui_ops(&quetzal_accounts_uiops);
	purple_request_set_ui_ops(&quetzal_request_uiops);

	Event("quetzal-ui-ops-inited").send();
}

static GHashTable *quetzal_ui_info()
{
	static GHashTable *table = NULL;
	if (!table) {
		table = g_hash_table_new(g_str_hash, g_str_equal);
		QByteArray name = qApp->applicationName().toUtf8();
		QByteArray version = qApp->applicationVersion().toUtf8();
		g_hash_table_insert(table, const_cast<char *>("name"), g_strdup(name.constData()));
		g_hash_table_insert(table, const_cast<char *>("version"), g_strdup(version.constData()));
		g_hash_table_insert(table, const_cast<char *>("website"), const_cast<char *>("http://qutim.org/"));
		g_hash_table_insert(table, const_cast<char *>("type"), const_cast<char *>("pc"));
	}
	return table;
}

static PurpleCoreUiOps quetzal_core_uiops =
{
	NULL,
	NULL,
	quetzal_ui_init,
	NULL,
	quetzal_ui_info,

	/* padding */
	NULL,
	NULL,
	NULL
};

void QuetzalPlugin::initLibPurple()
{
//	m_tmpDir = QDir::tempPath();
//	m_tmpDir += "/purple/";
//	m_tmpDir += QString::number(qrand());
//	purple_util_set_user_dir(QDir::toNativeSeparators(m_tmpDir).toUtf8().constData());
	/* Set a custom user directory (optional) */
//	purple_util_set_user_dir("/dev/null");

	/* We do not want any debugging for now to keep the noise to a minimum. */
	purple_debug_set_enabled(FALSE);

	/* Set the core-uiops, which is used to
	 * 	- initialize the ui specific preferences.
	 * 	- initialize the debug ui.
	 * 	- initialize the ui components for all the modules.
	 * 	- uninitialize the ui components for all the modules when the core terminates.
	 */
	purple_core_set_ui_ops(&quetzal_core_uiops);
	purple_eventloop_set_ui_ops(&quetzal_eventloop_uiops);
	purple_notify_set_ui_ops(&quetzal_notify_uiops);

	/* Set path to search for plugins. The core (libpurple) takes care of loading the
	 * core-plugins, which includes the protocol-plugins. So it is not essential to add
	 * any path here, but it might be desired, especially for ui-specific plugins. */
#ifdef Q_OS_WIN
	purple_plugins_add_search_path(g_strdup_printf("%s/plugins", qApp->applicationDirPath().toUtf8().constData()));
#endif

	/* Now that all the essential stuff has been set, let's try to init the core. It's
	 * necessary to provide a non-NULL name for the current ui to the core. This name
	 * is used by stuff that depends on this ui, for example the ui-specific plugins. */
	if (!purple_core_init("qutim")) {
		/* Initializing the core failed. Terminate. */
		fprintf(stderr,
				"libpurple initialization failed. Dumping core.\n"
				"Please report this!\n");
		return;
	}

	/* Create and load the buddylist. */
	purple_set_blist(purple_blist_new());

//	purple_timeout_add(1000, quetzal_test, 0);

//	qDebug() << purple_get_blist();

	/* Load the preferences. */
	purple_prefs_load();

	/* Load the desired plugins. The client should save the list of loaded plugins in
	 * the preferences using purple_plugins_save_loaded(PLUGIN_SAVE_PREF) */
	purple_plugins_load_saved("/dev/null");

	/* Load the pounces. */
	purple_pounces_load();

	static int handle_ptr;
	void *handle = &handle_ptr;
	// connect signals
//	purple_signal_connect(purple_accounts_get_handle(), "account-status-changed", handle,
//						  PURPLE_CALLBACK(quetzal_account_status_changed), NULL);
	purple_signal_connect(purple_conversations_get_handle(), "conversation-updated", handle,
						  PURPLE_CALLBACK(quetzal_conversation_update), NULL);
	purple_signal_connect(purple_connections_get_handle(), "signing-on",
						  handle, PURPLE_CALLBACK(quetzal_connection_signing_on_cb), NULL);
	purple_signal_connect(purple_connections_get_handle(), "signed-on",
						  handle, PURPLE_CALLBACK(quetzal_connection_signed_on_cb), NULL);
	purple_signal_connect(purple_connections_get_handle(), "signed-off",
						  handle, PURPLE_CALLBACK(quetzal_connection_signed_off_cb), NULL);
}

void QuetzalPlugin::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Quetzal"),
			QT_TRANSLATE_NOOP("Plugin", "'Quetzal' is set of protocols, powered by libpurple"),
			PLUGIN_VERSION(0, 0, 1, 0),
			Icon("quetzal"));
	QLibrary lib("purple");
	lib.setLoadHints(QLibrary::ExportExternalSymbolsHint);
	if (!lib.load()) {
#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
		// So try to guess purple path
		QStringList mayBePaths(QLatin1String("/usr/lib"));
#ifdef PURPLE_LIBDIR
		mayBePaths << QString::fromLocal8Bit(PURPLE_LIBDIR);
#endif
#if QT_POINTER_SIZE == 8
		mayBePaths << QLatin1String("/usr/lib64");
#elif QT_POINTER_SIZE == 4
		mayBePaths << QLatin1String("/usr/lib32");
#endif
#ifdef Q_WS_MAEMO_5
		mayBePaths << QLatin1String("/opt/maemo/usr/lib");
#endif
		mayBePaths.removeDuplicates();
		QStringList filter(QLatin1String("libpurple.so*"));
		bool ok = false;
		foreach (const QString &path, mayBePaths) {
			QDir dir(path);
			foreach (const QString &name, dir.entryList(filter, QDir::Files)) {
				lib.setFileName(dir.filePath(name));
				if (ok |= lib.load())
					break;
			}
			if (ok)
				break;
		}
		if (!ok)
#endif
			return;
	}
	initLibPurple();
	QByteArray imPrefix("im-");
	for(GList *it = purple_plugins_get_protocols(); it != NULL; it = it->next)
	{
		PurplePlugin *protocol = (PurplePlugin *)it->data;
		debug() << "Protocol: " << protocol->info->name;
		QuetzalProtocolGenerator *gen = new QuetzalProtocolGenerator(protocol);
		addExtension(LocalizedString(protocol->info->name),
					 QT_TRANSLATE_NOOP("Plugin", "'Quetzal' is set of protocols, powered by libpurple"),
					 gen,
					 ExtensionIcon(QLatin1String(imPrefix + const_cast<const char *>(protocol->info->name))));
		addExtension(LocalizedString(protocol->info->name),
					 QT_TRANSLATE_NOOP("Plugin", "'Quetzal' is set of protocols, powered by libpurple"),
					 new QuetzalProtocolGenerator(gen),
					 ExtensionIcon(QLatin1String(imPrefix + const_cast<const char *>(protocol->info->name))));
	}
//	qDebug() << QAbstractEventDispatcher::instance()->metaObject()->className();
//	QuetzalThread *thread = new QuetzalThread(this);
//	connect(thread, SIGNAL(finished(void*)), this, SLOT(onFinished(void*)));
//	QEventLoop loop;
//	connect(thread, SIGNAL(finished(void*)), &loop, SLOT(quit()));
//	thread->start();
//	loop.exec();
//	qDebug() << __LINE__;
}

void QuetzalPlugin::clearTemporaryDir()
{
//	if (m_tmpDir.isEmpty())
//		return;
	
//	m_tmpDir.clear();
}

void QuetzalPlugin::onFinished(void *data)
{
	Q_UNUSED(data);
//	qDebug() << __LINE__;
}

QUTIM_EXPORT_PLUGIN(QuetzalPlugin)


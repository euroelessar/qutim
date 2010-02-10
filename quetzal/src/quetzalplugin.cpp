#include "quetzalplugin.h"
#include "quetzalprotocol.h"
#include "quetzalaccount.h"
#include "quatzelactiondialog.h"
#include <purple.h>
#include <qutim/messagesession.h>
#include <qutim/debug.h>
#include <QCoreApplication>
#include <QTimerEvent>
#include <QLibrary>
#include <qutim/icon.h>
#include <qutim/notificationslayer.h>
#include <QAbstractEventDispatcher>
#include <QWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QStringBuilder>
#include <QTextDocument>
#include <QDateTime>

QuetzalTimer *QuetzalTimer::m_self = NULL;

QuetzalTimer::QuetzalTimer(QObject *parent):
		QObject(parent), m_socketId(0)
{
}

uint QuetzalTimer::addTimer(guint interval, GSourceFunc function, gpointer data)
{
	int id = startTimer(interval);
	m_timers.insert(id, new TimerInfo(function, data));
	return static_cast<uint>(id);
}

gboolean QuetzalTimer::removeTimer(guint handle)
{
	int id = static_cast<int>(handle);
	QMap<int, TimerInfo *>::iterator it = m_timers.find(id);
	if (it == m_timers.end())
		return FALSE;
	killTimer(id);
	delete it.value();
	m_timers.erase(it);
	return TRUE;
}

void QuetzalTimer::timerEvent(QTimerEvent *event)
{
	QMap<int, TimerInfo *>::iterator it = m_timers.find(event->timerId());
	TimerInfo *info = it.value();
	gboolean result = ( *info->function)(info->data);
	if(result == FALSE) {
		killTimer(it.key());
		delete it.value();
		m_timers.erase(it);
	}
}

guint QuetzalTimer::addIO(int fd, PurpleInputCondition cond, PurpleInputFunction func, gpointer user_data)
{
	if (fd < 0) {
		warning() << "Invalid file descriptor" << fd << "return id" << m_socketId;
		return m_socketId++;
	}

	QSocketNotifier::Type type;
	if (cond & PURPLE_INPUT_READ)
		type = QSocketNotifier::Read;
	else
		type = QSocketNotifier::Write;

	QSocketNotifier *socket = new QSocketNotifier(fd, type, this);
	socket->setProperty("quetzal_id", m_socketId);
	connect(socket, SIGNAL(activated(int)), this, SLOT(onSocket(int)));

	m_files.insert(m_socketId, new FileInfo(fd, socket, cond, func, user_data));
	socket->setEnabled(true);
	return m_socketId++;
}

gboolean QuetzalTimer::removeIO(guint handle)
{
	QMap<uint, FileInfo *>::iterator it = m_files.find(handle);
	if (it == m_files.end())
		return FALSE;
	FileInfo *info = it.value();
	info->socket->deleteLater();
	m_files.erase(it);
	return TRUE;
}

int QuetzalTimer::getIOError(int fd, int *error)
{
	return 0;
}

void QuetzalTimer::onSocket(int fd)
{
	QSocketNotifier *socket = qobject_cast<QSocketNotifier *>(sender());
	guint id = socket->property("quetzal_id").toUInt();
	QMap<uint, FileInfo *>::iterator it = m_files.find(id);
	FileInfo *info = it.value();
	socket->setEnabled(false);
	(*info->func)(info->data, fd, info->cond);
	socket->setEnabled(true);
}

static guint quetzal_timeout_add(guint interval, GSourceFunc function, gpointer data)
{
	return QuetzalTimer::instance()->addTimer(interval, function, data);
}

static gboolean quetzal_timeout_remove(guint handle)
{
	return QuetzalTimer::instance()->removeTimer(handle);
}

static guint quetzal_input_add(int fd, PurpleInputCondition cond, PurpleInputFunction func, gpointer user_data)
{
	return QuetzalTimer::instance()->addIO(fd, cond, func, user_data);
}

static gboolean quetzal_input_remove(guint handle)
{
	return QuetzalTimer::instance()->removeIO(handle);
}

static int quetzal_input_get_error(int fd, int *error)
{
	return QuetzalTimer::instance()->getIOError(fd, error);
}

static guint quetzal_timeout_add_seconds(guint interval, GSourceFunc function, gpointer data)
{
	return quetzal_timeout_add(interval * 1000, function, data);
}

static PurpleEventLoopUiOps quetzal_eventloops =
{
	quetzal_timeout_add,
	quetzal_timeout_remove,
	quetzal_input_add,
	quetzal_input_remove,
	NULL /*quetzal_input_get_error*/,
	quetzal_timeout_add_seconds,

	/* padding */
	NULL,
	NULL,
	NULL
};

/*** End of the eventloop functions. ***/

/*** Conversation uiops ***/
static void
null_write_conv(PurpleConversation *conv, const char *who, const char *alias,
			const char *message, PurpleMessageFlags flags, time_t mtime)
{
	const char *name;
	if (alias && *alias)
		name = alias;
	else if (who && *who)
		name = who;
	else
		name = NULL;

	printf("(%s) %s %s: %s\n", purple_conversation_get_name(conv),
			purple_utf8_strftime("(%H:%M:%S)", localtime(&mtime)),
			name, message);
}


void quetzal_create_conversation(PurpleConversation *conv)
{
	if (conv->type == PURPLE_CONV_TYPE_IM) {
		 QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(conv->account->ui_data);
		 QuetzalContact *contact = qobject_cast<QuetzalContact *>(acc->getUnit(conv->name));
		 conv->ui_data = contact;
	}
}

void quetzal_destroy_conversation(PurpleConversation *conv)
{
	if (conv->type == PURPLE_CONV_TYPE_IM) {
		QuetzalContact *contact = reinterpret_cast<QuetzalContact *>(conv->ui_data);
		if (contact) {
			ChatSession *session = ChatLayer::get(contact, false);
			session->setActive(false);
		}
	}
}

void quetzal_write_chat(PurpleConversation *conv, const char *who,
				   const char *message, PurpleMessageFlags flags,
				   time_t mtime)
{
}

void quetzal_write_im(PurpleConversation *conv, const char *who,
				 const char *message, PurpleMessageFlags flags,
				 time_t mtime)
{
	QuetzalContact *contact = reinterpret_cast<QuetzalContact *>(conv->ui_data);
	Message mess(message);
	mess.setIncoming(!qstrcmp(who, contact->buddy()->name));
	if (!mess.isIncoming())
		return;
	mess.setTime(QDateTime::fromTime_t(mtime));
	mess.setChatUnit(contact);
	ChatLayer::get(contact, true)->appendMessage(mess);
}

void quetzal_write_conv(PurpleConversation *conv,
				   const char *name,
				   const char *alias,
				   const char *message,
				   PurpleMessageFlags flags,
				   time_t mtime)
{
	ChatUnit *contact = reinterpret_cast<ChatUnit *>(conv->ui_data);
	Message mess(message);
	mess.setIncoming(qstrcmp(name, conv->account->username));
	debug() << name << alias;
	if (!mess.isIncoming())
		return;
	mess.setTime(QDateTime::fromTime_t(mtime));
	mess.setChatUnit(contact);
	ChatLayer::get(contact, true)->appendMessage(mess);
}

void quetzal_chat_add_users(PurpleConversation *conv,
					   GList *cbuddies,
					   gboolean new_arrivals)
{
}

void quetzal_chat_rename_user(PurpleConversation *conv, const char *old_name,
						 const char *new_name, const char *new_alias)
{
}

void quetzal_chat_remove_users(PurpleConversation *conv, GList *users)
{
}

void quetzal_chat_update_user(PurpleConversation *conv, const char *user)
{
}

void quetzal_present(PurpleConversation *conv)
{
}

gboolean quetzal_has_focus(PurpleConversation *conv)
{
}

gboolean quetzal_custom_smiley_add(PurpleConversation *conv, const char *smile, gboolean remote)
{
}

void quetzal_custom_smiley_write(PurpleConversation *conv, const char *smile,
							const guchar *data, gsize size)
{
}

void quetzal_custom_smiley_close(PurpleConversation *conv, const char *smile)
{
}

void quetzal_send_confirm(PurpleConversation *conv, const char *message)
{
}


static PurpleConversationUiOps quetzal_conv_uiops =
{
	quetzal_create_conversation,/* create_conversation  */
	quetzal_destroy_conversation,/* destroy_conversation */
	NULL,                      /* write_chat           */
	quetzal_write_im,          /* write_im             */
	quetzal_write_conv,           /* write_conv           */
	NULL,                      /* chat_add_users       */
	NULL,                      /* chat_rename_user     */
	NULL,                      /* chat_remove_users    */
	NULL,                      /* chat_update_user     */
	NULL,                      /* present              */
	NULL,                      /* has_focus            */
	NULL,                      /* custom_smiley_add    */
	NULL,                      /* custom_smiley_write  */
	NULL,                      /* custom_smiley_close  */
	NULL,                      /* send_confirm         */
	NULL,
	NULL,
	NULL,
	NULL
};

static void
null_ui_init(void)
{
	/**
	 * This should initialize the UI components for all the modules. Here we
	 * just initialize the UI for conversations.
	 */
	purple_conversations_set_ui_ops(&quetzal_conv_uiops);
}

inline char *quetzal_copystr(const QByteArray &data)
{
	char *str = reinterpret_cast<char *>(qMalloc(data.size() + 1));
	qMemCopy(str, data, data.size());
	str[data.size()] = '\0';
	return str;
}

static GHashTable *quetzal_ui_info()
{
	static GHashTable *table = NULL;
	if (!table) {
		table = g_hash_table_new(g_str_hash, g_str_equal);
		QByteArray name = qApp->applicationName().toUtf8();
		QByteArray version = qApp->applicationVersion().toUtf8();
		g_hash_table_insert(table, const_cast<char *>("name"), quetzal_copystr(name));
		g_hash_table_insert(table, const_cast<char *>("version"), quetzal_copystr(version));
		g_hash_table_insert(table, const_cast<char *>("website"), const_cast<char *>("http://qutim.org/"));
		g_hash_table_insert(table, const_cast<char *>("type"), const_cast<char *>("pc"));
	}
	return table;
}

static PurpleCoreUiOps quetzal_core_uiops =
{
	NULL,
	NULL,
	null_ui_init,
	NULL,
	quetzal_ui_info,

	/* padding */
	NULL,
	NULL,
	NULL
};

void quetzal_debug_print(PurpleDebugLevel level, const char *category, const char *arg_s)
{
	Q_UNUSED(level);
	QByteArray arg(arg_s);
	arg.chop(1);
	qDebug("[quetzal/%s]: %s", category, arg.constData());
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

//PurpleRequestFieldsCb;

void *quetzal_request_input(const char *title, const char *primary,
							const char *secondary, const char *default_value,
							gboolean multiline, gboolean masked, gchar *hint,
							const char *ok_text, GCallback ok_cb,
							const char *cancel_text, GCallback cancel_cb,
							PurpleAccount *account, const char *who,
							PurpleConversation *conv, void *user_data)
{
	debug() << Q_FUNC_INFO;
	Q_UNUSED(account);
	Q_UNUSED(who);
	Q_UNUSED(conv);
	QString label;
	if (primary) {
		label += QLatin1Literal("<span weight=\"bold\" size=\"larger\">")
				 % Qt::escape(primary)
				 % QLatin1Literal("</span>");
		if (secondary)
			label += "\n\n";
	}
	if (secondary)
		label += Qt::escape(secondary);

	QInputDialog dialog;
	dialog.setWindowTitle(title);
	dialog.setLabelText(label);
	dialog.setTextValue(default_value);
	dialog.setTextEchoMode(masked ? QLineEdit::Password : QLineEdit::Normal);
	if (ok_text)
		dialog.setOkButtonText(ok_text);
	if (cancel_text)
		dialog.setCancelButtonText(cancel_text);

	bool ok = !!dialog.exec();

	PurpleRequestInputCb func = reinterpret_cast<PurpleRequestInputCb>(ok ? ok_cb : cancel_cb);
	if (func)
		(*func)(user_data, ok ? dialog.textValue().toUtf8().constData() : NULL);
	return NULL;
}

void *quetzal_request_choice(const char *title, const char *primary,
							 const char *secondary, int default_value,
							 const char *ok_text, GCallback ok_cb,
							 const char *cancel_text, GCallback cancel_cb,
							 PurpleAccount *account, const char *who,
							 PurpleConversation *conv, void *user_data,
							 va_list choices)
{
	debug() << Q_FUNC_INFO;
}

void *quetzal_request_action(const char *title, const char *primary,
							 const char *secondary, int default_action,
							 PurpleAccount *account, const char *who,
							 PurpleConversation *conv, void *user_data,
							 size_t action_count, va_list actions)
{
	debug() << Q_FUNC_INFO;
	Q_UNUSED(account);
	Q_UNUSED(who);
	Q_UNUSED(conv);
	QuetzalRequestActionList uiActions;
	while (action_count --> 0) {
		QString str = va_arg(actions, gchararray);
		PurpleRequestActionCb cb = va_arg(actions, PurpleRequestActionCb);
		uiActions << qMakePair(str, cb);
	}
	QDialog *dialog = new QuetzalActionDialog(title, primary, secondary,
											  default_action, uiActions, user_data, NULL);
	dialog->show();
	return dialog;
}

void *quetzal_request_fields(const char *title, const char *primary,
							 const char *secondary, PurpleRequestFields *fields,
							 const char *ok_text, GCallback ok_cb,
							 const char *cancel_text, GCallback cancel_cb,
							 PurpleAccount *account, const char *who,
							 PurpleConversation *conv, void *user_data)
{
	debug() << Q_FUNC_INFO;
}

void *quetzal_request_file(const char *title, const char *filename,
						   gboolean savedialog, GCallback ok_cb,
						   GCallback cancel_cb, PurpleAccount *account,
						   const char *who, PurpleConversation *conv,
						   void *user_data)
{
	debug() << Q_FUNC_INFO;
	Q_UNUSED(account);
	Q_UNUSED(who);
	Q_UNUSED(conv);
	QString file = (savedialog ? QFileDialog::getSaveFileName : QFileDialog::getOpenFileName)
				   (NULL, title, filename, QString(), NULL, 0);
	PurpleRequestFileCb func;
	if (file.isEmpty())
		func = reinterpret_cast<PurpleRequestFileCb>(cancel_cb);
	else
		func = reinterpret_cast<PurpleRequestFileCb>(ok_cb);

	if (func)
		(*func)(user_data, file.isEmpty() ? NULL : file.toUtf8().constData());
	return NULL;
}

void quetzal_close_request(PurpleRequestType type, void *ui_handle)
{
	debug() << Q_FUNC_INFO;
	Q_UNUSED(type);
	QWidget *widget = reinterpret_cast<QWidget *>(ui_handle);
	widget->deleteLater();
}

void *quetzal_request_folder(const char *title, const char *dirname,
							 GCallback ok_cb, GCallback cancel_cb,
							 PurpleAccount *account, const char *who,
							 PurpleConversation *conv, void *user_data)
{
	debug() << Q_FUNC_INFO;
	Q_UNUSED(account);
	Q_UNUSED(who);
	Q_UNUSED(conv);
	QString dir = QFileDialog::getExistingDirectory(NULL, title, dirname);
	PurpleRequestFileCb func;
	if (dir.isEmpty())
		func = reinterpret_cast<PurpleRequestFileCb>(cancel_cb);
	else
		func = reinterpret_cast<PurpleRequestFileCb>(ok_cb);

	if (func)
		(*func)(user_data, dir.toUtf8().constData());
	return NULL;
}

PurpleRequestUiOps quetzal_request_uiops =
{
	quetzal_request_input,
	quetzal_request_choice,
	quetzal_request_action,
	quetzal_request_fields,
	quetzal_request_file,
	quetzal_close_request,
	quetzal_request_folder,
	NULL,
	NULL,
	NULL,
	NULL
};

void quetzal_new_list(PurpleBuddyList *list)
{
	debug() << "new_list";
}

void quetzal_new_node(PurpleBlistNode *node)
{
//	debug() << "new_node" << node->type;
	if (PURPLE_BLIST_NODE_IS_BUDDY(node)) {
		PurpleBuddy *buddy = PURPLE_BUDDY(node);
		QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(buddy->account->ui_data);
		if (acc) {
			acc->createNode(node);
		}
	}
}

void quetzal_show(PurpleBuddyList *list)
{
	Q_UNUSED(list);
}

void quetzal_update(PurpleBuddyList *list, PurpleBlistNode *node)
{
	Q_UNUSED(list);
	QObject *obj = reinterpret_cast<QObject *>(node->ui_data);
	if (QuetzalContact *contact = qobject_cast<QuetzalContact *>(obj)) {
		contact->update();
	}
}

void quetzal_remove(PurpleBuddyList *list, PurpleBlistNode *node)
{
}

void quetzal_destroy(PurpleBuddyList *list)
{
}

void quetzal_set_visible(PurpleBuddyList *list, gboolean show)
{
}

void quetzal_request_add_buddy(PurpleAccount *account, const char *username, const char *group, const char *alias)
{
}

void quetzal_request_add_chat(PurpleAccount *account, PurpleGroup *group, const char *alias, const char *name)
{
}

void quetzal_request_add_group(void)
{
}

void quetzal_save_node(PurpleBlistNode *node)
{
	if (PURPLE_BLIST_NODE_IS_BUDDY(node)) {
		PurpleBuddy *buddy = PURPLE_BUDDY(node);
		QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(buddy->account->ui_data);
		if (acc) {
			QuetzalContact *contact = reinterpret_cast<QuetzalContact *>(buddy->node.ui_data);
			acc->save(contact);
		}
	}
}

void quetzal_remove_node(PurpleBlistNode *node)
{
	if (PURPLE_BLIST_NODE_IS_BUDDY(node)) {
		PurpleBuddy *buddy = PURPLE_BUDDY(node);
		QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(buddy->account->ui_data);
		if (acc) {
			QuetzalContact *contact = reinterpret_cast<QuetzalContact *>(buddy->node.ui_data);
			acc->remove(contact);
		}
	}
}

void quetzal_save_account(PurpleAccount *account)
{
	if (account) {
		QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(account->ui_data);
		if (acc)
			acc->save();
	} else {
		for (GList *it = purple_accounts_get_all(); it != NULL; it = it->next) {
			account = reinterpret_cast<PurpleAccount *>(it->data);
			QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(account->ui_data);
			if (acc)
				acc->save();
		}
	}
}

PurpleBlistUiOps quetzal_blist_uiops = {
	quetzal_new_list,
	quetzal_new_node,
	quetzal_show,
	quetzal_update,
	quetzal_remove,
	quetzal_destroy,
	quetzal_set_visible,
	quetzal_request_add_buddy,
	quetzal_request_add_chat,
	quetzal_request_add_group,
	quetzal_save_node,
	quetzal_remove_node,
	quetzal_save_account,
	NULL
};


void quetzal_notify_added(PurpleAccount *account,
						  const char *remote_user,
						  const char *id,
						  const char *alias,
						  const char *message)
{
}

void quetzal_status_changed(PurpleAccount *account,
							PurpleStatus *status)
{
}

void quetzal_request_add(PurpleAccount *account,
						 const char *remote_user,
						 const char *id,
						 const char *alias,
						 const char *message)
{
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
	return 0;
}

void quetzal_close_account_request(void *ui_handle)
{
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

static gboolean quetzal_blist_load(gpointer data)
{
	Q_UNUSED(data);
	purple_blist_load();
	return FALSE;
}

static void
init_libpurple()
{
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
	purple_debug_set_ui_ops(&quetzal_debug_uiops);
	purple_core_set_ui_ops(&quetzal_core_uiops);
	purple_eventloop_set_ui_ops(&quetzal_eventloops);
	purple_blist_set_ui_ops(&quetzal_blist_uiops);
	purple_accounts_set_ui_ops(&quetzal_accounts_uiops);
	purple_request_set_ui_ops(&quetzal_request_uiops);

	/* Set path to search for plugins. The core (libpurple) takes care of loading the
	 * core-plugins, which includes the protocol-plugins. So it is not essential to add
	 * any path here, but it might be desired, especially for ui-specific plugins. */
	purple_plugins_add_search_path("");

	/* Now that all the essential stuff has been set, let's try to init the core. It's
	 * necessary to provide a non-NULL name for the current ui to the core. This name
	 * is used by stuff that depends on this ui, for example the ui-specific plugins. */
	if (!purple_core_init("qutim")) {
		/* Initializing the core failed. Terminate. */
		fprintf(stderr,
				"libpurple initialization failed. Dumping core.\n"
				"Please report this!\n");
		abort();
	}

	/* Create and load the buddylist. */
	purple_set_blist(purple_blist_new());

//	purple_timeout_add(1000, quetzal_blist_load, 0);

//	qDebug() << purple_get_blist();

	/* Load the preferences. */
	purple_prefs_load();

	/* Load the desired plugins. The client should save the list of loaded plugins in
	 * the preferences using purple_plugins_save_loaded(PLUGIN_SAVE_PREF) */
	purple_plugins_load_saved("/dev/null");

	/* Load the pounces. */
	purple_pounces_load();
}

void QuetzalPlugin::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Quetzal"),
			QT_TRANSLATE_NOOP("Plugin", "'Quetzal' is set of protocols, powered by libpurple"),
			PLUGIN_VERSION(0, 0, 1, 0),
			Icon("quetzal"));
	QLibrary lib("purple");
	lib.setLoadHints(QLibrary::ExportExternalSymbolsHint);
	lib.load();
	init_libpurple();
	QByteArray imPrefix("im-");
	for(GList *it = purple_plugins_get_protocols(); it != NULL; it = it->next)
	{
		PurplePlugin *protocol = (PurplePlugin *)it->data;
		qDebug("Protocol: %s", protocol->info->name);
		addExtension(LocalizedString(protocol->info->name),
					 QT_TRANSLATE_NOOP("Plugin", "'Quetzal' is set of protocols, powered by libpurple"),
					 new QuetzalProtocolGenerator(protocol),
					 Icon(QLatin1String(imPrefix + const_cast<const char *>(protocol->info->name))));
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

void QuetzalPlugin::onFinished(void *data)
{
//	qDebug() << __LINE__;
}

QUTIM_EXPORT_PLUGIN(QuetzalPlugin)

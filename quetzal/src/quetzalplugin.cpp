#include "quetzalplugin.h"
#include "quetzalprotocol.h"
#include "quetzalthread.h"
#include <purple.h>
#include <qutim/debug.h>
#include <QTimerEvent>
#include <QLibrary>
#include <qutim/icon.h>
#include <QAbstractEventDispatcher>

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
	killTimer(id);
	delete m_timers.take(id);
}

void QuetzalTimer::timerEvent(QTimerEvent *event)
{
	QMap<int, TimerInfo *>::iterator it = m_timers.find(event->timerId());
	TimerInfo *info = it.value();
	gboolean result = (*info->function)(info->data);
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
	FileInfo *info = it.value();
	delete info->socket;
	m_files.erase(it);
}

int QuetzalTimer::getIOError(int fd, int *error)
{
	return 0;
}

void QuetzalTimer::onSocket(int fd)
{
	QSocketNotifier *socket = static_cast<QSocketNotifier *>(sender());
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
//Should create a callback timer with an interval measured in milliseconds.

static gboolean quetzal_timeout_remove(guint handle)
{
	return QuetzalTimer::instance()->removeTimer(handle);
}
//Should remove a callback timer.

static guint quetzal_input_add(int fd, PurpleInputCondition cond, PurpleInputFunction func, gpointer user_data)
{
	return QuetzalTimer::instance()->addIO(fd, cond, func, user_data);
}
//Should add an input handler.

static gboolean quetzal_input_remove(guint handle)
{
	return QuetzalTimer::instance()->removeIO(handle);
}
//Should remove an input handler.

static int quetzal_input_get_error(int fd, int *error)
{
	return QuetzalTimer::instance()->getIOError(fd, error);
}
//If implemented, should get the current error status for an input.

static guint quetzal_timeout_add_seconds(guint interval, GSourceFunc function, gpointer data)
{
	return quetzal_timeout_add(interval * 1000, function, data);
}
//If implemented, should create a callback timer with an interval measured in seconds.

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

static PurpleConversationUiOps null_conv_uiops =
{
	NULL,                      /* create_conversation  */
	NULL,                      /* destroy_conversation */
	NULL,                      /* write_chat           */
	NULL,                      /* write_im             */
	null_write_conv,           /* write_conv           */
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
	purple_conversations_set_ui_ops(&null_conv_uiops);
}

static PurpleCoreUiOps null_core_uiops =
{
	NULL,
	NULL,
	null_ui_init,
	NULL,

	/* padding */
	NULL,
	NULL,
	NULL,
	NULL
};

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
	purple_core_set_ui_ops(&null_core_uiops);

	/* Set the uiops for the eventloop. If your client is glib-based, you can safely
	 * copy this verbatim. */
	purple_eventloop_set_ui_ops(&quetzal_eventloops);

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
	purple_blist_load();

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

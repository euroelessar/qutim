#include "logger.h"
#include <qutim/config.h>
#include <qutim/systeminfo.h>
#include <qutim/debug.h>
#include <cstdio>
#include <QTime>
#include <qutim/icon.h>
#include <QCheckBox>

namespace Logger
{
using namespace std;
static FILE *logfile = NULL;
//ofstream logfile;

void SimpleLoggingHandler(QtMsgType type, const char *msg)
{
	if (!logfile) {
		if (type == QtFatalMsg)
			abort();
		return;
	}
	QByteArray currentTime = QTime::currentTime().toString().toLatin1();
	switch (type) {
	default:
	case QtDebugMsg:
		fprintf(logfile, "%s Debug: %s\n", currentTime.constData(), msg);
//		logfile << QTime::currentTime().toString().toLatin1().data() << " Debug: " << msg << "\n";
		break;
	case QtCriticalMsg:
		fprintf(logfile, "%s Critical: %s\n", currentTime.constData(), msg);
//		logfile << QTime::currentTime().toString().toLatin1().data() << " Critical: " << msg << "\n";
		break;
	case QtWarningMsg:
		fprintf(logfile, "%s Warning: %s\n", currentTime.constData(), msg);
//		logfile << QTime::currentTime().toString().toLatin1().data() << " Warning: " << msg << "\n";
		break;
	case QtFatalMsg:
		fprintf(logfile, "%s Fatal: %s\n", currentTime.constData(), msg);
//		logfile << QTime::currentTime().toString().toLatin1().data() <<  " Fatal: " << msg << "\n";
		abort();
	}
}

void LoggerPlugin::init()
{
	debug() << Q_FUNC_INFO;
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Logger"),
			QT_TRANSLATE_NOOP("Plugin", "Simple file based logger for debug"),
			PLUGIN_VERSION(0, 1, 0, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author","Aleksey Sidorov"),
			  QT_TRANSLATE_NOOP("Task","Author"),
			  QLatin1String("sauron@citadelspb.com"),
			  QLatin1String("sauron.me"));
	m_settingsItem = NULL;
}

bool LoggerPlugin::load()
{
	if (m_settingsItem)
		return false;
	Config config = Config().group(QLatin1String("Logger"));
	QString path = config.value(QLatin1String("path"),
								SystemInfo::getPath(SystemInfo::ConfigDir).append("/qutim.log"));
	bool enable = config.value(QLatin1String("enable"), true);
	reloadSettings();
	qInstallMsgHandler(SimpleLoggingHandler);
	debug() << tr("New session started, happy debuging ^_^");

	AutoSettingsItem *settingsItem = new AutoSettingsItem(Settings::Plugin,
														  Icon("view-choose"),
														  QT_TRANSLATE_NOOP("Plugin", "Logger"));
	settingsItem->connect(SIGNAL(saved()), this, SLOT(reloadSettings()));
	settingsItem->setConfig(QString(), "Logger");
	ObjectGenerator *gen = new GeneralGenerator<AutoSettingsFileChooser>();
	AutoSettingsItem::Entry *entryItem = settingsItem->addEntry(QT_TRANSLATE_NOOP("Plugin", "Select log path"), gen);
	entryItem->setProperty("path", path);
	entryItem->setName("path");
	gen = new GeneralGenerator<QCheckBox>();
	entryItem = settingsItem->addEntry(QT_TRANSLATE_NOOP("Plugin", "Enable dump of debug info"), gen);
	entryItem->setProperty("checked", enable);
	entryItem->setName("enable");
	m_settingsItem = settingsItem;
	Settings::registerItem(settingsItem);
	return true;
}

void LoggerPlugin::reloadSettings()
{
	Config config = Config().group(QLatin1String("Logger"));
	int maxFileSize = config.value(QLatin1String("maxFileSize"), 512 * 1024);
	QString path = config.value(QLatin1String("path"),
								SystemInfo::getPath(SystemInfo::ConfigDir).append("/qutim.log"));
	bool enable = config.value(QLatin1String("enable"), true);
	if (enable && !logfile) {
		QFileInfo fileInfo(path);
		if (maxFileSize != -1 && fileInfo.size() > maxFileSize)
			logfile = fopen(path.toLocal8Bit(), "w");
//			logfile.open(path.toLocal8Bit(), ios::out | ios::trunc);
		else
			logfile = fopen(path.toLocal8Bit(), "wa");
//			logfile.open(path.toLocal8Bit(), ios::app);
	} else if (!enable && logfile) {
		fflush(logfile);
		fclose(logfile);
		logfile = NULL;
	}
}

bool LoggerPlugin::unload()
{
	if (m_settingsItem) {
		if (logfile) {
			fflush(logfile);
			fclose(logfile);
			logfile = NULL;
		}
		qInstallMsgHandler(NULL);
		Settings::removeItem(m_settingsItem);
		m_settingsItem = NULL;
		return true;
	}
	return false;
}
}

QUTIM_EXPORT_PLUGIN(Logger::LoggerPlugin)

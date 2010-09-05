#include "logger.h"
#include <qutim/config.h>
#include <qutim/systeminfo.h>
#include <qutim/debug.h>
#include <fstream>
#include <QTime>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>

namespace Logger
{
	using namespace std;
	ofstream logfile;

	void SimpleLoggingHandler(QtMsgType type, const char *msg) {
		switch (type) {
			case QtDebugMsg:
				logfile << QTime::currentTime().toString().toAscii().data() << " Debug: " << msg << "\n";
				break;
			case QtCriticalMsg:
				logfile << QTime::currentTime().toString().toAscii().data() << " Critical: " << msg << "\n";
				break;
			case QtWarningMsg:
				logfile << QTime::currentTime().toString().toAscii().data() << " Warning: " << msg << "\n";
				break;
			case QtFatalMsg:
				logfile << QTime::currentTime().toString().toAscii().data() <<  " Fatal: " << msg << "\n";
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
	}

	bool LoggerPlugin::load()
	{
		Config config = Config().group("Logger");
		QString path = config.value("path",SystemInfo::getPath(SystemInfo::ConfigDir).append("/qutim.log"));
		logfile.open(path.toLocal8Bit(), ios::app);
		qInstallMsgHandler(SimpleLoggingHandler);
		debug() << tr("New session started, happy debuging ^_^");

		AutoSettingsItem *settingsItem = new AutoSettingsItem(Settings::Plugin,
															  Icon("view-choose"),
															  QT_TRANSLATE_NOOP("Plugin", "Logger"));
		settingsItem->setConfig(QString(),"Logger");
		ObjectGenerator *gen = new GeneralGenerator<AutoSettingsFileChooser>();
		AutoSettingsItem::Entry *entryItem = settingsItem->addEntry(QT_TRANSLATE_NOOP("Plugin", "Select log path"), gen);
		entryItem->setProperty("path",path);
		entryItem->setName("path");
		Settings::registerItem(settingsItem);
		return true;
	}
	bool LoggerPlugin::unload()
	{
		return false;
	}
}

QUTIM_EXPORT_PLUGIN(Logger::LoggerPlugin)

#ifndef PURPLEPROTOCOLPLUGIN_H
#define PURPLEPROTOCOLPLUGIN_H

#include <qutim/protocol.h>
#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;
#include <QSocketNotifier>
#include <purple.h>

class QuetzalTimer : public QObject
{
	Q_OBJECT
	struct TimerInfo
	{
		TimerInfo(GSourceFunc f, gpointer d) : function(f), data(d) {}
		GSourceFunc function;
		gpointer data;
	};
	struct FileInfo
	{
		FileInfo(int fd_, QSocketNotifier *s, PurpleInputCondition c, PurpleInputFunction f, gpointer d) :
				fd(fd_), socket(s), cond(c), func(f), data(d) {}
		int fd;
		QSocketNotifier *socket;
		PurpleInputCondition cond;
		PurpleInputFunction func;
		gpointer data;
	};

public:
	static QuetzalTimer *instance()
	{
		if (!m_self)
			m_self = new QuetzalTimer();
		return m_self;
	}
	uint addTimer(guint interval, GSourceFunc function, gpointer data);
	gboolean removeTimer(guint handle);
	guint addIO(int fd, PurpleInputCondition cond, PurpleInputFunction func, gpointer user_data);
	gboolean removeIO(guint handle);
	int getIOError(int fd, int *error);
protected:
	virtual void timerEvent(QTimerEvent *event);
private slots:
	void onSocket(int fd);

private:
	explicit QuetzalTimer(QObject *parent = 0);
	static QuetzalTimer *m_self;
	QMap<int, TimerInfo *> m_timers;
	QMap<guint, FileInfo *> m_files;
	guint m_socketId;
};

class QuetzalPlugin : public Plugin
{
    Q_OBJECT
	Q_CLASSINFO("DebugName", "quetzal")
public:
	virtual void init();
	virtual bool load() { return true; }
	virtual bool unload() { return false; }
private slots:
	void onFinished(void *data);
};

#endif // PURPLEPROTOCOLPLUGIN_H

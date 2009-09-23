#include "event_test_p.h"
#include <QSettings>
#include <QStringList>
#include <QTime>
#include <QDebug>
#include <QCoreApplication>

namespace qutim_sdk_0_3
{
	void EventTest::testE(Event &event)
	{
		QSettings settings("/home/elessar/.config/qutim/qutim.EuroElessar/ICQ.3485140/accountsettings.ini");
		Q_UNUSED(settings.allKeys());
	}

	bool EventTest::eventFilter(QObject *, QEvent *ev)
	{
		if(ev->type() == QEvent::User)
		{
			QSettings settings("/home/elessar/.config/qutim/qutim.EuroElessar/ICQ.3485140/accountsettings.ini");
			Q_UNUSED(settings.allKeys());
			return true;
		}
		return false;
	}

	void EventTest::test()
	{
		QSettings settings("/home/elessar/.config/qutim/qutim.EuroElessar/ICQ.3485140/accountsettings.ini");
		Q_UNUSED(settings.allKeys());
	}

	void testEventSystemSpeed()
	{
		const int NUM = 50000;
		QScopedPointer<EventTest> test(new EventTest(0));
		QObject::connect(test.data(), SIGNAL(signal()), test.data(), SLOT(test()));
		EventManager::registerEventHandler("TestSystem", test.data(), &EventTest::testE);
		Event event("TestSystem");
		QutimEvent qevent;
		qApp->installEventFilter(test.data());
		qDebug("There are %d calls of method:", NUM);
		QTime t;
		{
			t.start();
			for(volatile int i = 0; i < NUM; i++)
				test->signal();
			qDebug() << "by direct access" << t.elapsed() << "ms";
		}
		{
			t.start();
			for(volatile int i = 0; i < NUM; i++)
				test->signal();
			qDebug() << "by signal" << t.elapsed() << "ms";
		}
		{
			t.start();
			for(volatile int i = 0; i < NUM; i++)
				event.send();
			qDebug() << "by event" << t.elapsed() << "ms";
		}
		{
			t.start();
			for(volatile int i = 0; i < NUM; i++)
				QCoreApplication::sendEvent(qApp, &qevent);
			qDebug() << "by qevent" << t.elapsed() << "ms";
		}
		qApp->removeEventFilter(test.data());
	}
}


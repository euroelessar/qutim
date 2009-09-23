#ifndef EVENT_TEST_P_H
#define EVENT_TEST_P_H

#include "event.h"
#include <QEvent>

namespace qutim_sdk_0_3
{
	class LIBQUTIM_EXPORT EventTest : public QObject
	{
		Q_OBJECT
	public:
		inline EventTest(QObject *parent) : QObject(parent) {}
		void testE(Event &event);
		virtual bool eventFilter(QObject *, QEvent *);
	public slots:
		void test();
	signals:
	#ifndef Q_MOC_RUN
	public:
	#endif
		void signal();
	};

	LIBQUTIM_EXPORT void testEventSystemSpeed();
}

class QutimEvent : public QEvent
{
public:
	QutimEvent() : QEvent(QEvent::User) {}
};

#endif // EVENT_TEST_P_H

#ifndef MACIDLE_H
#define MIDLE_H

#include <QObject>
#include <qutim/protocol.h>

namespace MacIntegration
{
using namespace qutim_sdk_0_3;

struct MacIdlePrivate;

class MacIdle : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "AutoAway")
	Q_DECLARE_PRIVATE(MacIdle)
public:
	enum Reason {
		Screensaver,
		Away,
		Inactive,
		NoIdle
	};
	MacIdle();
	~MacIdle();
	void setIdleOn(Reason reason);
	void setIdleOff();
	void reloadSettings();
private slots:
	void onTimeout();
private:
	QScopedPointer<MacIdlePrivate> d_ptr;
};
}

#endif // MACIDLE_H

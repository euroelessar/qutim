#ifndef MIDLE_H
#define MIDLE_H

#include <QObject>
#include <qutim/protocol.h>

namespace MacIntegration
{
	using namespace qutim_sdk_0_3;

	struct MIdlePrivate;

	class MIdle : public QObject
	{
		Q_OBJECT
		Q_CLASSINFO("Service", "AutoAway")
		Q_DECLARE_PRIVATE(MIdle)
		public:
			enum Reason {
				Screensaver,
				Away,
				Inactive,
				NoIdle
			};
			MIdle();
			~MIdle();
			void setIdleOn(Reason reason);
			void setIdleOff();
			void reloadSettings();
		private slots:
			void onTimeout();
		private:
			QScopedPointer<MIdlePrivate> d_ptr;
	};
}

#endif // MIDLE_H

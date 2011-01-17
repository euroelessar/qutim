#ifndef CRASHPLUGIN_H
#define CRASHPLUGIN_H

#include <QObject>
#include <qutim/libqutim_global.h>
#include <kcrash.h>

using namespace qutim_sdk_0_3;

class KdeCrashHandler : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "CrashHandler")
public:
	KdeCrashHandler();
};

#endif // CRASHPLUGIN_H

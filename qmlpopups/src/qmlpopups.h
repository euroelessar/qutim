#ifndef QMLPOPUPSPLUGIN_H
#define QMLPOPUPSPLUGIN_H
#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

namespace QmlPopups {

	class QmlPopupsPlugin : public Plugin
	{
		Q_OBJECT
		Q_CLASSINFO("DebugName", "QmlPopups")
	public:
		virtual void init();
		virtual bool load();
		virtual bool unload();
	};

}
#endif // QMLPOPUPSPLUGIN_H

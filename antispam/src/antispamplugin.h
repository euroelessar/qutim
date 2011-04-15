#ifndef ANTISPAMPLUGIN_H
#define ANTISPAMPLUGIN_H
#include <qutim/plugin.h>
#include <QPointer>

namespace Antispam {

using namespace qutim_sdk_0_3;

class Handler;
class AntispamPlugin : public Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "Antispam")
	Q_CLASSINFO("Uses", "IconLoader")
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
private:
	QPointer<Handler> m_handler;
};

}

#endif // ANTISPAMPLUGIN_H

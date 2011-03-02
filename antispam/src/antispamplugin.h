#ifndef ANTISPAMPLUGIN_H
#define ANTISPAMPLUGIN_H
#include <qutim/plugin.h>

namespace qutim_sdk_0_3 {
class ChatSession;
}

namespace Antispam {

class CaptchaSender;

using namespace qutim_sdk_0_3;

class AntispamPlugin : public Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "Antispam")
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
protected slots:
	void sessionCreated(qutim_sdk_0_3::ChatSession*);
private:
	CaptchaSender *m_captcha_sender;
};

}

#endif // ANTISPAMPLUGIN_H

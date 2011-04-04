#ifndef PLUGMAN_H
#define PLUGMAN_H
#include <qutim/plugin.h>
#include <attica/provider.h>
#include <attica/providermanager.h>
#include <QNetworkAccessManager>
//#include "packageengine.h"

namespace qutim_sdk_0_3 {
class ActionGenerator;
}

namespace PackageManager
{
class PackageManagerPlugin : public qutim_sdk_0_3::Plugin
{
    Q_OBJECT
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();

public slots:
	void onManagerButtonClicked();

private:
	qutim_sdk_0_3::ActionGenerator *m_gen;
//	PackageEngine *m_emoticonsEngine;
//	PackageEngine *m_stylesEngine;
};
}

#endif

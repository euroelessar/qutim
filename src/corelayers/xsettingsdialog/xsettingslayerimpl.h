#ifndef XSETTINGSLAYERIMPL_H
#define XSETTINGSLAYERIMPL_H

#include "libqutim/settingslayer.h"

class XSettingsDialog;
using namespace qutim_sdk_0_3;

class XSettingsLayerImpl : public SettingsLayer
{
	Q_OBJECT
public:
    XSettingsLayerImpl();
    virtual ~XSettingsLayerImpl();
	virtual void close();
    virtual void show ( const SettingsItemList& settings );
private:
	XSettingsDialog *m_dialog;
};

#endif // XSETTINGSLAYERIMPL_H

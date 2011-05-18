#ifndef SETTINGSLAYERIMPL_H
#define SETTINGSLAYERIMPL_H

#include <qutim/settingslayer.h>

using namespace qutim_sdk_0_3;
class SettingsDialog;
class SettingsLayerImpl : public SettingsLayer
{
	Q_OBJECT
public:
	SettingsLayerImpl();
	virtual ~SettingsLayerImpl();
	virtual void close();
	virtual void show ( const SettingsItemList& settings );
private:
	QPointer<SettingsDialog> m_dialog;
};

#endif // SETTINGSLAYERIMPL_H

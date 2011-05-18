#ifndef HUNSPELLSETTINGS_H
#define HUNSPELLSETTINGS_H

#include <qutim/settingswidget.h>

using namespace qutim_sdk_0_3;
class QComboBox;

class HunSpellSettings : public SettingsWidget
{
	Q_OBJECT
	Q_DISABLE_COPY(HunSpellSettings)
public:
	explicit HunSpellSettings();
	virtual ~HunSpellSettings();
protected:
	void loadImpl();
	void saveImpl();
	void cancelImpl() { loadImpl(); }
private:
	QComboBox *m_languagesBox;
};

#endif // HUNSPELLSETTINGS_H

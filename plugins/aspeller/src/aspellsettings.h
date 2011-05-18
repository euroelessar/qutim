#ifndef ASPELLSETTINGS_H
#define ASPELLSETTINGS_H

#include <qutim/settingswidget.h>

using namespace qutim_sdk_0_3;
class QComboBox;

class AspellSettings : public SettingsWidget
{
	Q_OBJECT
	Q_DISABLE_COPY(AspellSettings)
public:
	explicit AspellSettings();
	virtual ~AspellSettings();
protected:
	void loadImpl();
	void saveImpl();
	void cancelImpl() { loadImpl(); }
private:
	QComboBox *m_languagesBox;
};

#endif // ASPELLSETTINGS_H

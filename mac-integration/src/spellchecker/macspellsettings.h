#ifndef MACSPELLSETTINGS_H
#define MACSPELLSETTINGS_H

#include <qutim/settingswidget.h>

using namespace qutim_sdk_0_3;
class QComboBox;

namespace MacIntegration
{
	class MacSpellSettings : public SettingsWidget
	{
		Q_OBJECT
			Q_DISABLE_COPY(MacSpellSettings)
		public:
			explicit MacSpellSettings();
			virtual ~MacSpellSettings();
		protected:
			void loadImpl();
			void saveImpl();
			void cancelImpl() { loadImpl(); }
		private:
			QComboBox *m_languagesBox;
	};
}

#endif // MACSPELLSETTINGS_H

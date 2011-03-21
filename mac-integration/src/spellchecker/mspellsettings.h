#ifndef MSPELLSETTINGS_H
#define MSPELLSETTINGS_H

#include <qutim/settingswidget.h>

using namespace qutim_sdk_0_3;
class QComboBox;

namespace MacIntegration
{
	class MSpellSettings : public SettingsWidget
	{
		Q_OBJECT
			Q_DISABLE_COPY(MSpellSettings)
		public:
			explicit MSpellSettings();
			virtual ~MSpellSettings();
		protected:
			void loadImpl();
			void saveImpl();
			void cancelImpl() { loadImpl(); }
		private:
			QComboBox *m_languagesBox;
	};
}

#endif // MSPELLSETTINGS_H

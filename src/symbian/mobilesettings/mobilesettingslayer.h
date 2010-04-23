#ifndef MOBILESETTINGSLAYER_H
#define MOBILESETTINGSLAYER_H
#include <libqutim/settingslayer.h>

namespace Core
{

	namespace Mobile
	{
		using namespace qutim_sdk_0_3;
		class SettingsDialog;
		class MobileSettingsLayer : public SettingsLayer
		{
			Q_OBJECT
		public:
			virtual void show(const qutim_sdk_0_3::SettingsItemList& settings);
			virtual void close();
		private:
			QPointer<SettingsDialog> m_widget;
		};
	}

}
#endif // MOBILESETTINGSLAYER_H


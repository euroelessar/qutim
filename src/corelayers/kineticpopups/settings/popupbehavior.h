#ifndef POPUPBEHAVIOR_H
#define POPUPBEHAVIOR_H
#include <libqutim/settingswidget.h>
#include "../backend.h"

namespace KineticPopups
{

	class PopupBehavior : public SettingsWidget
	{
		Q_OBJECT
	public:
		PopupBehavior();
		virtual void cancelImpl();
		virtual void loadImpl();
		virtual void saveImpl();
	};

}
#endif // POPUPBEHAVIOR_H

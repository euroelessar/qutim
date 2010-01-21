#ifndef POPUPBEHAVIOR_H
#define POPUPBEHAVIOR_H
#include <libqutim/settingswidget.h>
#include "../backend.h"

class QToolButton;
class QGridLayout;
namespace Ui
{
	class BehaviorSettings;
}

namespace Core
{

	class PopupBehavior : public SettingsWidget
	{
		Q_OBJECT
	public:
		PopupBehavior();
		virtual ~PopupBehavior();
		virtual void cancelImpl();
		virtual void loadImpl();
		virtual void saveImpl();
	private slots:
		void onAnimationSpeedIndexChanged(int index);
		void onTimeoutTextChanged(const QString &str);
		void addNotification(const QString &localized_string, int index);
		void onPopupBtnToggled(bool checked);
		void onSoundBtnToggled(bool checked);
	private:
		int indexOfDuration (int duration);
		void loadFlags();
		Ui::BehaviorSettings *ui;
		QGridLayout *m_layout;
		QList<QToolButton *> m_popup_btn_list;
		QList<QToolButton *> m_sound_btn_list;
		int m_sound_flags;
		int m_popup_flags;
	};

}
#endif // POPUPBEHAVIOR_H

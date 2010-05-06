#ifndef POPUPBEHAVIOR_H
#define POPUPBEHAVIOR_H
#include <libqutim/settingswidget.h>
#include "../backend.h"

class QStandardItemModel;
class QStandardItem;
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
		enum ItemRole
		{
			ItemTypeRole = Qt::UserRole,
			ItemSoundCheckedRole,
			ItemPopupCheckedRole
		};
		PopupBehavior();
		virtual ~PopupBehavior();
		virtual void cancelImpl();
		virtual void loadImpl();
		virtual void saveImpl();
	private slots:
		void onAnimationSpeedIndexChanged(int index);
		void onTimeoutTextChanged(const QString &str);
		void addNotification(const QString &localized_string, int index);
		void onItemChanged(QStandardItem *item);
	private:
		int indexOfDuration (int duration);
		void loadFlags();
		Ui::BehaviorSettings *ui;
		QStandardItemModel *m_model;
		int m_sound_flags;
		int m_popup_flags;
	};

}
#endif // POPUPBEHAVIOR_H

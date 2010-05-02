#ifndef SHORTCUTSETTINGSWIDGET_H
#define SHORTCUTSETTINGSWIDGET_H

#include <QWidget>
#include <libqutim/settingswidget.h>

namespace Ui {
    class ShortcutSettingsWidget;
}
class QStandardItemModel;
namespace Core
{
	using namespace qutim_sdk_0_3;
	class ShortcutSettingsWidget : public SettingsWidget
	{
		Q_OBJECT

	public:
		explicit ShortcutSettingsWidget();
		~ShortcutSettingsWidget();
		virtual void loadImpl();
		virtual void saveImpl();
		virtual void cancelImpl();
	protected:
		void changeEvent(QEvent *e);

	private:
		Ui::ShortcutSettingsWidget *ui;
		QStandardItemModel *m_model;
	};

}

#endif // SHORTCUTSETTINGSWIDGET_H

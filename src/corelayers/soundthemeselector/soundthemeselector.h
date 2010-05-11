#ifndef SOUNDTHEMESELECTOR_H
#define SOUNDTHEMESELECTOR_H

#include <QWidget>
#include <libqutim/settingswidget.h>

namespace Ui {
    class SoundThemeSelector;
}

namespace Core
{
	using namespace qutim_sdk_0_3;

	class SoundThemeSelector : public SettingsWidget
	{
		Q_OBJECT

	public:
		explicit SoundThemeSelector();
		~SoundThemeSelector();
		virtual void loadImpl();
		virtual void cancelImpl();
		virtual void saveImpl();
	protected:
		void changeEvent(QEvent *e);
	private slots:
		void currentIndexChanged(const QString &text);
	private:
		Ui::SoundThemeSelector *ui;
	};

}
#endif // SOUNDTHEMESELECTOR_H

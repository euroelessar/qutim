#include "soundthemeselector.h"
#include "ui_soundthemeselector.h"
#include <libqutim/notificationslayer.h>

namespace Core
{
	using namespace qutim_sdk_0_3;

	SoundThemeSelector::SoundThemeSelector() :
			ui(new Ui::SoundThemeSelector)
	{
		ui->setupUi(this);
		connect(ui->themeSelector,SIGNAL(currentIndexChanged(QString)),SLOT(currentIndexChanged(QString)));
	}

	SoundThemeSelector::~SoundThemeSelector()
	{
		delete ui;
	}

	void SoundThemeSelector::changeEvent(QEvent *e)
	{
		QWidget::changeEvent(e);
		switch (e->type()) {
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
		}
	}
	
	void SoundThemeSelector::loadImpl()
	{
		QStringList theme_list =  Sound::themeList();
		ui->themeSelector->addItems(theme_list);
		int index = ui->themeSelector->findText(Sound::currentThemeName());
		ui->themeSelector->setCurrentIndex(index);
	}
	void SoundThemeSelector::saveImpl()
	{
		Sound::setTheme(ui->themeSelector->currentText());
	}
	void SoundThemeSelector::cancelImpl()
	{

	}
	
	void SoundThemeSelector::currentIndexChanged(const QString& text)
	{
		emit modifiedChanged(true);
	}

}

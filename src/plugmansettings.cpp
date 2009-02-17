#include "plugmansettings.h"

plugmanSettings::plugmanSettings(const QString &profile_name) 
{
	ui.setupUi(this);
	m_profile_name = profile_name;
	connect(ui.installfromfileBtn, SIGNAL(clicked()), this,  SIGNAL(installfromfileclick()));
	ui.progressBar->setVisible(false);
}

plugmanSettings::~plugmanSettings()
{
	
}
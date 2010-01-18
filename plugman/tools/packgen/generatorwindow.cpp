#include "generatorwindow.h"
#include "choosecategorypage.h"
#include "configpackagepage.h"

GeneratorWindow::GeneratorWindow(QWidget *parent)
	: QWizard(parent)
{
	setPage(ChooseType, new ChooseCategoryPage(this));
	setPage(Config, new ConfigPackagePage(this));
}

GeneratorWindow::~GeneratorWindow()
{
}

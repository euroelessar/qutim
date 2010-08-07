#include "vaccountsettings.h"
#include "ui_vaccountsettings.h"

VAccountSettings::VAccountSettings(QWidget *parent) :
	ui(new Ui::VAccountSettings)
{
	setParent(parent);
    ui->setupUi(this);
}

VAccountSettings::~VAccountSettings()
{
    delete ui;
}

void VAccountSettings::loadImpl()
{

}

void VAccountSettings::saveImpl()
{

}

void VAccountSettings::cancelImpl()
{

}

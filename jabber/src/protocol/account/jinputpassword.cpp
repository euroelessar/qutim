#include "jinputpassword.h"

namespace Jabber
{
	JInputPassword::JInputPassword(const QString &id, QWidget *parent) :
			QDialog(parent), ui(new Ui::JInputPassword)
	{
		ui->setupUi(this);
		setWindowTitle(id);
	}

	JInputPassword::~JInputPassword()
	{
		delete ui;
	}
}

#ifndef JINPUTPASSWORD_H
#define JINPUTPASSWORD_H

#include <QDialog>
#include "ui_jinputpassword.h"

namespace Jabber
{

	class JInputPassword : public QDialog
	{
		Q_OBJECT
		public:
			JInputPassword(const QString &id, QWidget *parent = 0);
			~JInputPassword();
			QString passwd() {return ui->passwdEdit->text();}
			bool isSave() {return ui->isSaveCheck->isChecked();}
		private:
			Ui::JInputPassword *ui;
	};
}

#endif // JINPUTPASSWORD_H

#ifndef SIMPLESTATUSDIALOG_H
#define SIMPLESTATUSDIALOG_H

#include <QDialog>

namespace Ui {
    class SimpleStatusDialog;
}

namespace Core
{
	namespace SimpleContactList
	{

		class SimpleStatusDialog : public QDialog
		{
			Q_OBJECT

		public:
			explicit SimpleStatusDialog(const QString &status);
			~SimpleStatusDialog();
			QString statusText() const;
		protected:
			void changeEvent(QEvent *e);

		private:
			Ui::SimpleStatusDialog *ui;
		};

	}
}
#endif // SIMPLESTATUSDIALOG_H

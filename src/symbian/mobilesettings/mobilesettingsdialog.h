#ifndef MOBILESETTINGSDIALOG_H
#define MOBILESETTINGSDIALOG_H

#include <QWidget>
#include "mobilesettingslayer.h"

class QListWidgetItem;
namespace Ui {
    class MobileSettingsDialog;
}

namespace Core
{

	namespace Mobile
	{

		class SettingsDialog : public QWidget {
			Q_OBJECT
		public:
			SettingsDialog(const SettingsItemList &settings,QWidget *parent = 0);
			~SettingsDialog();
		protected slots:
			void itemClicked(QListWidgetItem* item);
		protected:
			void changeEvent(QEvent *e);

		private:
			Ui::MobileSettingsDialog *ui;
			SettingsItemList m_list;
		};

	}

}
#endif // MOBILESETTINGSDIALOG_H


#include "mobilesettingsitem.h"
#include <QVBoxLayout>
#include <libqutim/settingswidget.h>
#include <QAction>

namespace Core
{

	namespace Mobile
	{
		SettingsDialogItem::SettingsDialogItem(SettingsItem* item):
				m_item(item)
		{
			setAttribute(Qt::WA_DeleteOnClose);
			QVBoxLayout *layout = new QVBoxLayout(this);
			SettingsWidget *widget = item->widget();
			connect(this,SIGNAL(finished(int)),SLOT(deleteLater()));
			widget->setParent(this);
			widget->load();
			layout->addWidget(widget);

			QAction *act = new QAction(tr("Accept"), this);
			act->setSoftKeyRole(QAction::PositiveSoftKey);
			connect(act, SIGNAL(triggered()),this,SLOT(accept()));
			addAction(act);

			act = new QAction(tr("Cancel"),this);
			act->setSoftKeyRole(QAction::NegativeSoftKey);
			connect(act, SIGNAL(triggered()),this,SLOT(reject()));
			addAction(act);

			setWindowTitle(tr("qutIM | %1").arg(item->text()));
		}

		void SettingsDialogItem::accept()
		{
			m_item->widget()->save();
			QDialog::accept();
		}

		void SettingsDialogItem::reject()
		{
			m_item->widget()->cancel();
			QDialog::reject();
		}


	}

}

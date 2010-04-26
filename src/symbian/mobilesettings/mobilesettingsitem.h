#ifndef MOBILESETTINGSITEM_H
#define MOBILESETTINGSITEM_H

#include <QDialog>
#include "mobilesettingsdialog.h"

namespace Core
{

	namespace Mobile
	{

		class SettingsDialogItem : public QDialog
		{
			Q_OBJECT
		public:
			SettingsDialogItem(SettingsItem *item);
		public slots:
			virtual void accept();
			virtual void reject();
		private:
			SettingsItem *m_item;
		};

	}

}
#endif // MOBILESETTINGSITEM_H


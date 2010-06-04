#ifndef TAGSFILTERDIALOG_H
#define TAGSFILTERDIALOG_H

#include <QDialog>
#include <QHash>

class QListWidgetItem;

namespace Ui {
    class TagsFilterDialog;
}

namespace Core
{
	namespace SimpleContactList
	{

		class TagsFilterDialog : public QDialog
		{
			Q_OBJECT

		public:
			explicit TagsFilterDialog(QSet<QString> tags, QWidget *parent = 0);
			~TagsFilterDialog();
			QSet<QString> selectedTags() const;
			void setSelectedTags(QSet<QString>);
		protected:
			void changeEvent(QEvent *e);

		private:
			Ui::TagsFilterDialog *ui;
			QHash<QString, QListWidgetItem*> m_items;
		};

	}
}
#endif // TAGSFILTERDIALOG_H

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
			explicit TagsFilterDialog(const QStringList &tags, QWidget *parent = 0);
			~TagsFilterDialog();
			QStringList selectedTags() const;
			void setSelectedTags(const QStringList &);
		protected:
			void changeEvent(QEvent *e);

		private:
			Ui::TagsFilterDialog *ui;
			QHash<QString, QListWidgetItem*> m_items;
		};

	}
}
#endif // TAGSFILTERDIALOG_H

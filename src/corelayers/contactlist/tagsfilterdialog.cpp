#include "tagsfilterdialog.h"
#include "ui_tagsfilterdialog.h"
#include <libqutim/icon.h>

namespace Core
{
	namespace SimpleContactList
	{
		using namespace qutim_sdk_0_3;

		TagsFilterDialog::TagsFilterDialog(QSet<QString> tags,QWidget *parent) :
				QDialog(parent),
				ui(new Ui::TagsFilterDialog)
		{
			ui->setupUi(this);
			setWindowIcon(Icon("feed-subscribe"));
			foreach (QString tag,tags) {
				QListWidgetItem *item = new QListWidgetItem (tag,ui->listWidget);
				item->setCheckState(Qt::Unchecked);
				ui->listWidget->addItem(item);
				m_items.insert(tag,item);
			}
		}

		TagsFilterDialog::~TagsFilterDialog()
		{
			delete ui;
		}

		void TagsFilterDialog::changeEvent(QEvent *e)
		{
			QDialog::changeEvent(e);
			switch (e->type()) {
			case QEvent::LanguageChange:
				ui->retranslateUi(this);
				break;
			default:
				break;
			}
		}

		QSet<QString> TagsFilterDialog::selectedTags() const
		{
			QSet<QString> selected_tags;
			for (int i = 0;i!=ui->listWidget->count();i++) {
				QListWidgetItem *item = ui->listWidget->item(i);
				if (item->checkState() == Qt::Checked)
					selected_tags.insert(item->text());
			}
			return selected_tags;
		}

		void TagsFilterDialog::setSelectedTags(QSet<QString> tags)
		{
			foreach (QString tag,tags) {
				m_items[tag]->setCheckState(Qt::Checked);
			}
		}

	}
}

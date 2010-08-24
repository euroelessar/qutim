#include "tagsfilterdialog.h"
#include "ui_tagsfilterdialog.h"
#include <qutim/icon.h>

namespace Core
{
	namespace SimpleContactList
	{
		using namespace qutim_sdk_0_3;

		TagsFilterDialog::TagsFilterDialog(const QStringList &tags,QWidget *parent) :
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

		QStringList TagsFilterDialog::selectedTags() const
		{
			QStringList selected_tags;
			for (int i = 0;i!=ui->listWidget->count();i++) {
				QListWidgetItem *item = ui->listWidget->item(i);
				if (item->checkState() == Qt::Checked)
					selected_tags.append(item->text());
			}
			return selected_tags;
		}

		void TagsFilterDialog::setSelectedTags(const QStringList &tags)
		{
			foreach (QString tag,tags) {
				m_items[tag]->setCheckState(Qt::Checked);
			}
		}

	}
}

#include "mobilesettingsdialog.h"
#include "ui_mobilesettingsdialog.h"
#include <QEvent>
#include <QListWidgetItem>
#include <libqutim/icon.h>
#include "mobilesettingsitem.h"
#include <QAction>

namespace Core
{

	namespace Mobile

	{

		SettingsDialog::SettingsDialog(const SettingsItemList &settings,QWidget *parent) :
				QWidget(parent),
				ui(new Ui::MobileSettingsDialog),
				m_list(settings)
		{
			ui->setupUi(this);
			setAttribute(Qt::WA_DeleteOnClose);

			foreach (SettingsItem *item,settings) {
				QListWidget *list_widget;
				switch (item->type())
				{
				case Settings::General:
					list_widget = ui->generalListWidget;
					break;
				case Settings::Protocol:
					list_widget = ui->protocolListWidget;
					break;
				case Settings::Appearance:
					list_widget = ui->appearanceListWidget;
					break;
				case Settings::Plugin:
					list_widget = ui->pluginListWidget;
					break;
				default:
					continue;
				}
				QListWidgetItem *widget_item = new QListWidgetItem(item->icon(),item->text(),list_widget);
				widget_item->setData(Qt::UserRole,settings.indexOf(item));
			}

			QAction *act = new QAction(tr("Close"),this);
			act->setSoftKeyRole(QAction::NegativeSoftKey);
			connect(act, SIGNAL(triggered()),this,SLOT(close()));
			addAction(act);

			connect(ui->generalListWidget,SIGNAL(itemClicked(QListWidgetItem*)),SLOT(itemClicked(QListWidgetItem*)));
			connect(ui->protocolListWidget,SIGNAL(itemClicked(QListWidgetItem*)),SLOT(itemClicked(QListWidgetItem*)));
			connect(ui->appearanceListWidget,SIGNAL(itemClicked(QListWidgetItem*)),SLOT(itemClicked(QListWidgetItem*)));
			connect(ui->pluginListWidget,SIGNAL(itemClicked(QListWidgetItem*)),SLOT(itemClicked(QListWidgetItem*)));
		}

		SettingsDialog::~SettingsDialog()
		{
			delete ui;
		}

		void SettingsDialog::changeEvent(QEvent *e)
		{
			QWidget::changeEvent(e);
			switch (e->type()) {
			case QEvent::LanguageChange:
				ui->retranslateUi(this);
				break;
			default:
				break;
			}
		}

		void SettingsDialog::itemClicked(QListWidgetItem* item)
		{
			Q_ASSERT(item);
			int index = item->data(Qt::UserRole).toInt();
			SettingsDialogItem *settings_item = new SettingsDialogItem(m_list.at(index));
			settings_item->showMaximized();
		}

	}

}

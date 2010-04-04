#include "filetransferdialog.h"
#include "ui_filetransferdialog.h"

namespace Core
{
FileTransferDialog::FileTransferDialog(FileTransferEngine *engine) :
    ui(new Ui::FileTransferDialog)
{
    ui->setupUi(this);
	m_engine = engine;
//	connect(m_engine, SIGNAL(totalSizeChanged(qint64)),
	setAttribute(Qt::WA_DeleteOnClose, true);
	setAttribute(Qt::WA_QuitOnClose, false);
}

FileTransferDialog::~FileTransferDialog()
{
    delete ui;
}

void FileTransferDialog::changeEvent(QEvent *e)
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
}

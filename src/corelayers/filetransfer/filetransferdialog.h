#ifndef FILETRANSFERDIALOG_H
#define FILETRANSFERDIALOG_H

#include <QDialog>
#include "libqutim/filetransfer.h"

namespace Ui {
    class FileTransferDialog;
}

namespace Core
{
using namespace qutim_sdk_0_3;
class FileTransferDialog : public QDialog
{
    Q_OBJECT
public:
	FileTransferDialog(FileTransferEngine *engine);
    ~FileTransferDialog();

protected:
    void changeEvent(QEvent *e);

private:
	FileTransferEngine *m_engine;
    Ui::FileTransferDialog *ui;
};
}

#endif // FILETRANSFERDIALOG_H

#ifndef FILETRANSFERDIALOG_H
#define FILETRANSFERDIALOG_H

#include <QDialog>
#include <QFileInfo>
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
	void setPath(const QString &path);
protected:
    void changeEvent(QEvent *e);
private slots:
	void currentFileUpdated(int current);
	void totalSizeChanged(qint64 size);
	void fileSizeChanged(qint64 size);
	void remoteFilesChanged(QStringList files);
private:
	FileTransferEngine *m_engine;
    Ui::FileTransferDialog *ui;
	QFileInfo m_path;
};
}

#endif // FILETRANSFERDIALOG_H

#ifndef FILETRANSFERDIALOG_H
#define FILETRANSFERDIALOG_H

#include <QDialog>
#include <QFileInfo>
#include <qutim/filetransfer.h>

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
	FileTransferDialog(FileTransferJob *job);
    ~FileTransferDialog();
	void setPath(const QString &path);
	void setJob(FileTransferJob *job);
	QIODevice *openFile();
protected:
    void changeEvent(QEvent *e);
private slots:
	void onFileNameChanged(const QString &fileName);
	void onTotalSizeChanged(qint64 size);
	void onFileSizeChanged(qint64 size);
	void onProgressChanged(qint64 progress);
private:
	FileTransferJob *m_job;
    Ui::FileTransferDialog *ui;
	QString m_path;
};
}

#endif // FILETRANSFERDIALOG_H

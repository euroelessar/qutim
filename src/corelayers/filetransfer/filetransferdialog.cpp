#include "filetransferdialog.h"
#include "ui_filetransferdialog.h"
#include <QFileDialog>

namespace Core
{
FileTransferDialog::FileTransferDialog(FileTransferJob *job) :
    ui(new Ui::FileTransferDialog)
{
	ui->setupUi(this);
	m_job = 0;
	setAttribute(Qt::WA_DeleteOnClose, true);
	setAttribute(Qt::WA_QuitOnClose, false);
	setJob(job);
}

FileTransferDialog::~FileTransferDialog()
{
	delete ui;
	m_job->stop();
}

void FileTransferDialog::setJob(FileTransferJob *job)
{
	if (m_job)
		disconnect(m_job, 0, this, 0);
	m_job = job;
	onTotalSizeChanged(job->totalSize());
	if (job->currentIndex() == -1) {
		onFileNameChanged(QString());
		onFileSizeChanged(0);
	} else {
		onFileNameChanged(job->fileName());
		onFileSizeChanged(job->fileSize());
	}
	connect(job, SIGNAL(fileNameChanged(QString)), SLOT(onFileNameChanged(QString)));
	connect(job, SIGNAL(fileSizeChanged(qint64)), SLOT(onFileSizeChanged(qint64)));
	connect(job, SIGNAL(totalSizeChanged(qint64)), SLOT(totalSizeChanged(qint64)));
	connect(job, SIGNAL(progressChanged(qint64)), SLOT(onProgressChanged(qint64)));
}

QIODevice *FileTransferDialog::openFile()
{
	QString path;
	if (m_job->direction() == FileTransferJob::Incoming) {
		if (m_path.isNull()) {
			if (m_job->filesCount() == 1) {
				m_path = QFileDialog::getSaveFileName(0, QString(),
													  QDir::home().filePath(m_job->fileName()));
			} else {
				m_path = QFileDialog::getExistingDirectory(0, QString(), QDir::homePath());
			}
			if (m_path.isEmpty()) {
				m_job->stop();
				return 0;
			}
		}
		QFileInfo info = m_path;
		if (info.isFile())
			path = info.absoluteFilePath();
		else
			path = QDir(m_path).filePath(m_job->fileName());
	} else {
		path = m_job->baseDir().filePath(m_job->fileName());
	}
	return new QFile(path);
}

//void FileTransferDialog::setPath(const QString &path)
//{
//	m_path.setFile(path);
//	remoteFilesChanged(m_engine->remoteFiles());
//}

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

void FileTransferDialog::onFileNameChanged(const QString &fileName)
{
	ui->fileName->setText(fileName);
}

void FileTransferDialog::onTotalSizeChanged(qint64 size)
{
	ui->totalSize->setText(QString::number(size));
}

void FileTransferDialog::onFileSizeChanged(qint64 size)
{
	ui->fileSize->setText(QString::number(size));
}

void FileTransferDialog::onProgressChanged(qint64 progress)
{
	int percent = progress * 100 / m_job->totalSize();
	if (ui->progressBar->value() != percent)
		ui->progressBar->setValue(percent);
}

//void FileTransferDialog::remoteFilesChanged(QStringList files)
//{
//	if (files.count() == 1) {
//		if (m_path.isDir())
//			files.first() = m_path.absolutePath() + "/" + files.first();
//		else
//			files.first() = m_path.absoluteFilePath();
//	} else {
//		QStringList::iterator itr = files.begin();
//		QStringList::iterator endItr = files.end();
//		while (itr != endItr) {
//			if (!itr->isEmpty())
//				*itr = m_path.absolutePath() + "/" + *itr;
//			++itr;
//		}
//	}
//	m_engine->setFiles(files);
//}

}

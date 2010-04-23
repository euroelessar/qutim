#include "filetransferdialog.h"
#include "ui_filetransferdialog.h"

namespace Core
{
FileTransferDialog::FileTransferDialog(FileTransferEngine *engine) :
    ui(new Ui::FileTransferDialog)
{
	ui->setupUi(this);
	m_engine = engine;
	setAttribute(Qt::WA_DeleteOnClose, true);
	setAttribute(Qt::WA_QuitOnClose, false);
	currentFileUpdated(engine->currentFile());
	totalSizeChanged(engine->totalSize());
	fileSizeChanged(engine->fileSize());
	connect(engine, SIGNAL(progressChanged(int)), ui->progressBar, SLOT(setValue(int)));
	connect(engine, SIGNAL(currentFileChanged(int)), SLOT(currentFileUpdated(int)));
	connect(engine, SIGNAL(totalSizeChanged(qint64)), SLOT(totalSizeChanged(qint64)));
	connect(engine, SIGNAL(fileSizeChanged(qint64)), SLOT(fileSizeChanged(qint64)));
	if (engine->direction() == FileTransferEngine::Receive) {
		connect(engine, SIGNAL(remoteFilesChanged(QStringList)), SLOT(remoteFilesChanged(QStringList)));
	}
}

FileTransferDialog::~FileTransferDialog()
{
	delete ui;
	delete m_engine;
}

void FileTransferDialog::setPath(const QString &path)
{
	m_path.setFile(path);
	remoteFilesChanged(m_engine->remoteFiles());
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

void FileTransferDialog::currentFileUpdated(int current)
{
	ui->fileName->setText(m_engine->remoteFiles().value(current));
}

void FileTransferDialog::totalSizeChanged(qint64 size)
{
	ui->totalSize->setText(QString::number(size));
}

void FileTransferDialog::fileSizeChanged(qint64 size)
{
	ui->fileSize->setText(QString::number(size));
}

void FileTransferDialog::remoteFilesChanged(QStringList files)
{
	if (files.count() == 1) {
		if (m_path.isDir())
			files.first() = m_path.absolutePath() + "/" + files.first();
		else
			files.first() = m_path.absoluteFilePath();
	} else {
		QStringList::iterator itr = files.begin();
		QStringList::iterator endItr = files.end();
		while (itr != endItr) {
			if (!itr->isEmpty())
				*itr = m_path.absolutePath() + "/" + *itr;
			++itr;
		}
	}
	m_engine->setFiles(files);
}

}

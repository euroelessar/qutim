#ifndef SIMPLEFILETRANSFER_H
#define SIMPLEFILETRANSFER_H

#include <qutim/filetransfer.h>
#include "filetransferdialog.h"

using namespace qutim_sdk_0_3;
namespace Core
{
	class SimpleFileTransfer : public Games::FileTransferManager
	{
		Q_OBJECT
		Q_CLASSINFO("Uses", "IconLoader")
	public:
		explicit SimpleFileTransfer();
		virtual QIODevice *doOpenFile(FileTransferJob *job);
		virtual void handleJob(FileTransferJob *job, FileTransferJob *oldJob);
		bool event(QEvent *ev);
	private slots:
		void onSendFile(QObject *controller);
		void onDialogDeath(QObject *dialog);
	private:
		QMap<FileTransferJob*, FileTransferDialog*> m_jobs;
	};

}

#endif // SIMPLEFILETRANSFER_H

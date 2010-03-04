#include "filetransfer.h"

namespace qutim_sdk_0_3
{
	FileTransferEngine::FileTransferEngine()
	{
	}

	void FileTransferEngine::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}

	FileTransfer::FileTransfer()
	{
	}
}

#include "aescryptomodule.h"
#include "aescryptoservice.h"

namespace AesCrypto
{

	void AesCryptoModule::init()
	{
		setInfo(QT_TRANSLATE_NOOP("Plugin", "AES crypto service loader"),
				QT_TRANSLATE_NOOP("Plugin", "Default qutIM crypto implementation. Based on algorithm aes256"),
				PLUGIN_VERSION(0, 0, 1, 0));
		addAuthor(QT_TRANSLATE_NOOP("Author", "Ruslan Nigmatullin"),
				  QT_TRANSLATE_NOOP("Task", "Developer"),
				  QLatin1String("euroelessar@gmail.com"));
		addExtension<AesCryptoService>(QT_TRANSLATE_NOOP("Plugin", "AES crypto"),
									   QT_TRANSLATE_NOOP("Plugin", "Default qutIM crypto implementation. Based on algorithm aes256")
									   );	
	}

	bool AesCryptoModule::load()
	{
		return false;
	}

	bool AesCryptoModule::unload()
	{
		return false;
	}


}

QUTIM_EXPORT_PLUGIN(AesCrypto::AesCryptoModule)

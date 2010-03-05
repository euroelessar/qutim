/****************************************************************************
 *  filetransfer.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "filetransfer.h"
#include "objectgenerator.h"

namespace qutim_sdk_0_3
{
	struct FileTransferEnginePrivate
	{
		ChatUnit *chatUnit;
		FileTransferFactory *factory;
		FileTransferEngine::Direction direction;
	};

	struct FileTransferManagerPrivate
	{
	};

	struct FileTransferData
	{
		QList<FileTransferFactory *> factories;
		QPointer<FileTransferManager> manager;
	};

	void init(FileTransferData *data)
	{
		GeneratorList gens = moduleGenerators<FileTransferFactory>();
		foreach(const ObjectGenerator *gen, gens)
			data->factories << gen->generate<FileTransferFactory>();
		gens = moduleGenerators<FileTransferManager>();
		if (!gens.isEmpty())
			data->manager = gens.first()->generate<FileTransferManager>();
	}

	Q_GLOBAL_STATIC_WITH_INITIALIZER(FileTransferData, data, init(x.data()));

	FileTransferEngine::FileTransferEngine(ChatUnit *chatUnit, Direction direction, FileTransferFactory *factory) :
			d_ptr(new FileTransferEnginePrivate)
	{
		Q_D(FileTransferEngine);
		d->chatUnit = chatUnit;
		d->factory = factory;
		d->direction = direction;
	}

	FileTransferEngine::~FileTransferEngine()
	{
	}

	ChatUnit *FileTransferEngine::chatUnit() const
	{
		return d_func()->chatUnit;
	}

	FileTransferEngine::Direction FileTransferEngine::direction() const
	{
		return d_func()->direction;
	}

	void FileTransferEngine::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}

	FileTransferManager::FileTransferManager() : d_ptr(new FileTransferManagerPrivate)
	{
	}

	FileTransferManager::~FileTransferManager()
	{
	}

	FileTransferManager *FileTransferManager::instance()
	{
		return data()->manager;
	}

	bool FileTransferManager::checkAbility(ChatUnit *unit)
	{
		foreach (FileTransferFactory *factory, data()->factories) {
			if (factory->check(unit))
				return true;
		}
		return false;
	}

	FileTransferEngine *FileTransferManager::getEngine(ChatUnit *unit, FileTransferEngine *last)
	{
		FileTransferFactory *lastFactory = last ? last->d_func()->factory : 0;
		FileTransferData *d = data();
		int index = lastFactory ? d->factories.indexOf(lastFactory) : -1;
		index++;
		for (; index < d->factories.size(); index++) {
			if (d->factories.at(index)->check(unit))
				return d->factories.at(index)->create(unit);
		}
		return 0;
	}
}

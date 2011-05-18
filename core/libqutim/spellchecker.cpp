/****************************************************************************
 *  spellchecker.cpp
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "spellchecker.h"
#include "servicemanager.h"
#include "objectgenerator.h"

namespace qutim_sdk_0_3 {

SpellChecker::SpellChecker()
{
}

SpellChecker::~SpellChecker()
{
}

SpellChecker *SpellChecker::instance()
{
	static ServicePointer<SpellChecker> speller;
	return speller.data();
}

void SpellChecker::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}

} // namespace qutim_sdk_0_3

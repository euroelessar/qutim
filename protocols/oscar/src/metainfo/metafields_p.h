/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef METAFIELDS_H
#define METAFIELDS_H

#include "metainfo/metafield.h"

namespace qutim_sdk_0_3 {

namespace oscar {

const FieldNamesList &countries();
const FieldNamesList &interests();
const FieldNamesList &languages();
const FieldNamesList &pasts();
const FieldNamesList &genders();
const FieldNamesList &study_levels();
const FieldNamesList &industries();
const FieldNamesList &occupations();
const FieldNamesList &affilations();
const AgesList &ages();
const FieldNamesList &fields();
const FieldNamesList &fields_names();

} } // namespace qutim_sdk_0_3::Oscar

#endif // METAFIELDS_H


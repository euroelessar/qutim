/****************************************************************************
 *  metafields_p.h
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

/****************************************************************************
 *  snac.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef SNAC_H
#define SNAC_H

#include "dataunit.h"

namespace Icq {

class SNAC : public DataUnit
{
public:
	inline SNAC(quint16 family = 0, quint16 subtype = 0) { reset(family, subtype); }
//	template <typename Family>
//	SNAC(Family subtype);
	void reset(quint16 family, quint16 subtype);
	inline quint16 family() const { return m_family; }
	inline void setFamily(SnacFamily family) { m_family = family; }
	inline quint16 subtype() const { return m_subtype; }
	inline void setSubtype(quint16 subtype) { m_subtype = subtype; }
	inline quint16 flags() const { return m_flags; }
	inline void setFlags(quint16 flags) { m_flags = flags; }
	inline quint32 id() const { return m_id; }
	inline void setId(quint32 id) { m_id = id; }
	static SNAC fromByteArray(const QByteArray &data);
	QByteArray toByteArray() const;
	QByteArray header() const;
	inline operator QByteArray() const { return toByteArray(); }
private:
	quint16 m_family;
	quint16 m_subtype;
	quint16 m_flags;
	quint32 m_id;
};
//
//template<>
//Q_INLINE_TEMPLATE SNAC::SNAC<ServiceFamilySubtype>(ServiceFamilySubtype subtype)
//{ reset(ServiceFamily, subtype); }
//
//template<>
//Q_INLINE_TEMPLATE SNAC::SNAC<LocationFamilySubtype>(LocationFamilySubtype subtype)
//{ reset(LocationFamily, subtype); }
//
//template<>
//Q_INLINE_TEMPLATE SNAC::SNAC<BuddyFamilySubtype>(BuddyFamilySubtype subtype)
//{ reset(BuddyFamily, subtype); }
//
//template<>
//Q_INLINE_TEMPLATE SNAC::SNAC<MessageFamilySubtype>(MessageFamilySubtype subtype)
//{ reset(MessageFamily, subtype); }
//
//template<>
//Q_INLINE_TEMPLATE SNAC::SNAC<PrivacyFamilySubtype>(PrivacyFamilySubtype subtype)
//{ reset(BosFamily, subtype); }
//
//template<>
//Q_INLINE_TEMPLATE SNAC::SNAC<LookupFamilySubtype>(LookupFamilySubtype subtype)
//{ reset(LookupFamily, subtype); }
//
//template<>
//Q_INLINE_TEMPLATE SNAC::SNAC<StatsFamilySubtype>(StatsFamilySubtype subtype)
//{ reset(StatsFamily, subtype); }
//
////template<>
////Q_INLINE_TEMPLATE SNAC::SNAC<>( subtype)
////{ reset(ChatNavigationFamily, subtype); }
//
////template<>
////Q_INLINE_TEMPLATE SNAC::SNAC<>( subtype)
////{ reset(ChatFamily, subtype); }
//
//template<>
//Q_INLINE_TEMPLATE SNAC::SNAC<AvatarFamilySubtype>(AvatarFamilySubtype subtype)
//{ reset(AvatarFamily, subtype); }
//
//template<>
//Q_INLINE_TEMPLATE SNAC::SNAC<ListsFamilySubtype>(ListsFamilySubtype subtype)
//{ reset(ListsFamily, subtype); }
//
//template<>
//Q_INLINE_TEMPLATE SNAC::SNAC<ExtensionsFamilySubtype>(ExtensionsFamilySubtype subtype)
//{ reset(ExtensionsFamily, subtype); }
//
//template<>
//Q_INLINE_TEMPLATE SNAC::SNAC<AuthorizationFamilySubtype>(AuthorizationFamilySubtype subtype)
//{ reset(AuthorizationFamily, subtype); }
//
////template<>
////Q_INLINE_TEMPLATE SNAC::SNAC<>( subtype)
////{ reset(DirectoryFamily, subtype); }
//
//template<typename Family>
//Q_INLINE_TEMPLATE SNAC::SNAC(Family subtype)
//{
//	subtype = 0x01; // Invalid Family
//}

} // namespace Icq

#endif // SNAC_H

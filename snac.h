#ifndef SNAC_H
#define SNAC_H

#include "dataunit.h"

class SNAC : public DataUnit
{
public:
	inline SNAC(quint16 family, quint16 subtype) { reset(family, subtype); }
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

#endif // SNAC_H

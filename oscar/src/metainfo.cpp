/****************************************************************************
 *  metainfo.cpp
 *
 *  Copyright (c) 2009 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "metainfo.h"
#include "icqaccount.h"
#include "oscarconnection.h"
#include <QStringList>

namespace Icq
{

static QDebug operator<<(QDebug dbg, const MetaInfo::Category &cat)
{
	dbg.nospace() << "{" << cat.category << ", " << cat.keyword << "}";
	return dbg.space();
}

MetaInfo::MetaInfo(QObject *parent) :
	SNACHandler(parent), m_sequence(0)
{
	m_infos << SNACInfo(ExtensionsFamily, ExtensionsMetaSrvReply)
		<< SNACInfo(ExtensionsFamily, ExtensionsMetaError);
}

void MetaInfo::handleSNAC(AbstractConnection *conn, const SNAC &snac)
{
	if (snac.family() == ExtensionsFamily && snac.subtype() == ExtensionsMetaSrvReply) {
		TLVMap tlvs = snac.readTLVChain();
		if (tlvs.contains(0x01)) {
			DataUnit data(tlvs.value(0x01));
			data.skipData(6); // skip length field + my uin
			quint16 metaType = data.readSimple<quint16>(LittleEndian);
			if (metaType == 0x07da) {
				quint16 reqNumber = data.readSimple<quint16>(LittleEndian);
				QHash<quint16, QObject*>::iterator objItr = m_requests.find(reqNumber);
				if (objItr == m_requests.end()) {
					debug() << "Unexpected metainfo response";
					return;
				}
				quint16 dataType = data.readSimple<quint16>(LittleEndian);
				quint8 success = data.readSimple<quint8>(LittleEndian);
				if (success == 0x0a) {
					switch (dataType) {
					case (0x0104):
						handleShortInfo(*objItr, data);
						m_requests.erase(objItr);
						break;
					case (0x00c8):
						handleBasicInfo(*objItr, data);
						break;
					case (0x00dc):
						handleMoreInfo(*objItr, data);
						break;
					case (0x00eb):
						handleEmails(*objItr, data);
						break;
					case (0x010e):
						handleHomepage(*objItr, data);
						break;
					case (0x00d2):
						handleWork(*objItr, data);
						break;
					case (0x00e6):
						handleNotes(*objItr, data);
						break;
					case (0x00f0):
						handleInterests(*objItr, data);
						break;
					case (0x00fa):
						handleAffilations(*objItr, data);
						m_requests.erase(objItr);
						break;
					default:
						debug() << "Unhandled metatype response" << (*objItr)->property("name").toString() << hex << dataType;
						m_requests.erase(objItr);
					}
				}
			}
		}
	} else if (snac.family() == ExtensionsFamily && snac.subtype() == ExtensionsMetaError) {
		ProtocolError error(snac);
		debug() << QString("Error (%1, %2): %3")
				.arg(error.code, 2, 16)
				.arg(error.subcode, 2, 16)
				.arg(error.str);
	}
}

void MetaInfo::sendShortInfoRequest(OscarConnection *conn, QObject *reqObject)
{
	sendInfoRequest(conn, reqObject, 0x04ba);
}

void MetaInfo::sendFullInfoRequest(OscarConnection *conn, QObject *reqObject)
{
	sendInfoRequest(conn, reqObject, 0x04b2);
}

void MetaInfo::sendRequest(OscarConnection *conn, QObject *reqObject, quint16 type, const DataUnit &extend_data)
{
	SNAC snac(ExtensionsFamily, ExtensionsMetaCliRequest);
	DataUnit tlv_data;
	DataUnit data;
	data.appendSimple<quint32>(conn->account()->id().toUInt(), LittleEndian);
	data.appendSimple<quint16>(0x07d0, LittleEndian);
	data.appendSimple<quint16>(++m_sequence, LittleEndian);
	data.appendSimple<quint16>(type, LittleEndian);
	data.appendData(extend_data.data());
	tlv_data.appendSimple<quint16>(data.data().size(), LittleEndian);
	tlv_data.appendData(data.data());
	snac.appendTLV(1, tlv_data);
	m_requests.insert(m_sequence, reqObject);
	conn->send(snac);
}

void MetaInfo::sendInfoRequest(OscarConnection *conn, QObject *reqObject, quint16 type)
{
	quint32 id = reqObject->property("id").toUInt();
	if (id != 0) {
		DataUnit data;
		data.appendSimple<quint32>(id, LittleEndian);
		sendRequest(conn, reqObject, type, data);
	}
}

void MetaInfo::handleShortInfo(QObject *reqObject, const DataUnit &data)
{
	QString nick = readString(data);
	QString first_name = readString(data);
	QString last_name = readString(data);
	QString email = readString(data);
	quint8 auth = data.readSimple<quint8>();
	data.skipData(2); // 0x00 unknown
	qint8 gender = data.readSimple<qint8>();

	if (reqObject) {
		if (reqObject->property("name").toString() == reqObject->property("id").toString())
			reqObject->setProperty("name", nick);
	}

	debug(Verbose) << "Short info" << nick << first_name << last_name << email << auth << gender;
}

void MetaInfo::handleBasicInfo(QObject *reqObject, const DataUnit &data)
{
	QString nick = readString(data);
	QString first_name = readString(data);
	QString last_name = readString(data);
	QString email = readString(data);
	QString home_city = readString(data);
	QString home_state = readString(data);
	QString home_phone = readString(data);
	QString home_fax = readString(data);
	QString home_address = readString(data);
	QString cell_phone = readString(data);
	QString home_zip_code = readString(data);
	quint16 home_country_code = data.readSimple<quint16>(LittleEndian);
	quint8 GMT = data.readSimple<quint8>();
	// flags
	quint8 auth = data.readSimple<quint8>();
	quint8 webaware = data.readSimple<quint8>();
	quint8 direct_connection = data.readSimple<quint8>();
	quint8 publish_primary_email = data.readSimple<quint8>();

	debug(Verbose) << "Basic info" << nick << first_name << last_name << email
			<< home_city << home_state << home_phone << home_fax << home_address
			<< cell_phone << home_zip_code << home_country_code << GMT << auth
			<< webaware << direct_connection << publish_primary_email;
}

void MetaInfo::handleMoreInfo(QObject *reqObject, const DataUnit &data)
{
	quint16 age = data.readSimple<quint16>(LittleEndian);
	qint8 gender = data.readSimple<qint8>();
	QString homepage = readString(data);
	quint16 birth_year = data.readSimple<quint16>(LittleEndian);
	quint8 birth_month = data.readSimple<quint8>();
	quint8 birth_day = data.readSimple<quint8>();
	quint8 language1 = data.readSimple<quint8>();
	quint8 language2 = data.readSimple<quint8>();
	quint8 language3 = data.readSimple<quint8>();
	data.skipData(2); // 0x0000 unknown
	QString original_city = readString(data);
	QString original_state = readString(data);
	quint16 original_country_code = data.readSimple<quint16>(LittleEndian);
	quint8 time_zone = data.readSimple<quint8>();

	debug(Verbose) << "More info" << age << gender << homepage << birth_year
			<< birth_month << birth_day << language1 << language2 << language3
			<< original_city << original_state << original_country_code << time_zone;
}

void MetaInfo::handleEmails(QObject *reqObject, const DataUnit &data)
{
	// TODO: test it
	QStringList publish_emails;
	QStringList private_emails;
	quint8 count = data.readSimple<quint8>();
	for (int i = 0; i < count; ++i) {
		bool is_publish = data.readSimple<quint8>();
		QString email = readString(data);
		if (is_publish)
			publish_emails << email;
		else
			private_emails << email;
	}

	debug(Verbose) << "Emails" << publish_emails << private_emails;
}

void MetaInfo::handleHomepage(QObject *reqObject, const DataUnit &data)
{
	bool is_enabled = data.readSimple<quint8>();
	quint16 homepage_category_code = data.readSimple<quint16>(LittleEndian);
	QString homepage_keyword = readString(data);

	debug(Verbose) << "Homepage" << is_enabled << homepage_category_code << homepage_keyword;
}

void MetaInfo::handleWork(QObject *reqObject, const DataUnit &data)
{
	QString work_city = readString(data);
	QString work_state = readString(data);
	QString work_phone = readString(data);
	QString work_fax = readString(data);
	QString work_address = readString(data);
	QString work_zip = readString(data);
	quint16 work_country_code = data.readSimple<quint16>(LittleEndian);
	QString work_company = readString(data);
	QString work_department = readString(data);
	QString work_position = readString(data);
	quint16 work_ocupation_code = data.readSimple<quint16>(LittleEndian);
	QString work_webpage = readString(data);

	debug(Verbose) << "Work info" << work_city << work_state << work_phone << work_fax
			<< work_address << work_zip << work_country_code << work_company << work_department
			<< work_position << work_ocupation_code << work_webpage;
}

void MetaInfo::handleNotes(QObject *reqObject, const DataUnit &data)
{
	QString notes = readString(data);
	debug(Verbose) << "Notes" << notes;
}

void MetaInfo::handleInterests(QObject *reqObject, const DataUnit &data)
{
	QList<Category> interests = handleCatagories(data);
	debug(Verbose) << "Interests" << interests;
}

void MetaInfo::handleAffilations(QObject *reqObject, const DataUnit &data)
{
	QList<Category> pasts = handleCatagories(data);
	debug(Verbose) << "Pasts" << pasts;

	QList<Category> affilations = handleCatagories(data);
	debug(Verbose) << "Affilations" << affilations;
}

QList<MetaInfo::Category> MetaInfo::handleCatagories(const DataUnit &data)
{
	QList<Category> result;
	quint8 count = data.readSimple<quint8>();
	Category category;
	for (int i = 0; i < count; ++i) {
		category.category = data.readSimple<quint16>(LittleEndian);
		category.keyword = readString(data);
		result << category;
	}
	return result;
}

inline QString MetaInfo::readString(const DataUnit &data)
{
	QString str = data.readString<quint16>(Util::asciiCodec(), LittleEndian);
	str.truncate(str.length() - 1);
	return str;
}

} //namespace Icq

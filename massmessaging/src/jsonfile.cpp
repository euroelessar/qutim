#include "jsonfile.h"
#include "k8json.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>

	struct JsonFilePrivate
	{
		QFile file;
	};

	JsonFile::JsonFile(const QString &name) : p(new JsonFilePrivate)
	{
		p->file.setFileName(name);
	}

	JsonFile::~JsonFile()
	{
		delete p;
	}

	void JsonFile::setFileName(const QString &name)
	{
		p->file.setFileName(name);
	}

	bool JsonFile::load(QVariant &variant)
	{
		variant.clear();
		if (!p->file.open(QIODevice::ReadOnly | QIODevice::Text))
			return false;
		int len = p->file.size();
		QByteArray array;
		const uchar *fmap = p->file.map(0, p->file.size());
		if(!fmap)
		{
			array = p->file.readAll();
			fmap = (uchar *)array.constData();
		}
		const uchar *s = K8JSON::skipBlanks(fmap, &len);
		bool result = (K8JSON::parseRec(variant, s, &len) != 0);
		p->file.close();
		return result;
	}

	bool json_file_generate (QTextStream &res, const QVariant &val, int indent) {
		switch (val.type()) {
			case QVariant::Invalid: res << "null"; break;
			case QVariant::Bool: res << (val.toBool() ? "true" : "false"); break;
			case QVariant::Char: res << K8JSON::quote(QString(val.toChar())).toUtf8(); break;
			case QVariant::Double: res << QString::number(val.toDouble()).toAscii(); break; //CHECKME: is '.' always '.'?
			case QVariant::Int: res << QString::number(val.toInt()).toAscii(); break;
			case QVariant::String: res << K8JSON::quote(val.toString()).toUtf8(); break;
			case QVariant::UInt: res << QString::number(val.toUInt()).toAscii(); break;
			case QVariant::ULongLong: res << QString::number(val.toULongLong()).toAscii(); break;
			case QVariant::Map: {
				//for (int c = indent; c > 0; c--) res << ' ';
				res << "{";
				indent++; bool comma = false;
				QVariantMap m(val.toMap());
				QVariantMap::const_iterator i;
				for (i = m.constBegin(); i != m.constEnd(); ++i) {
					if (comma) res << ",\n"; else { res << '\n'; comma = true; }
					for (int c = indent; c > 0; c--) res << ' ';
					res << K8JSON::quote(i.key()).toUtf8();
					res << ": ";
					if (!json_file_generate(res, i.value(), indent)) return false;
				}
				indent--;
				if (comma) {
					res << '\n';
					for (int c = indent; c > 0; c--) res << ' ';
				}
				res << '}';
				indent--;
			} break;
			case QVariant::List: {
				//for (int c = indent; c > 0; c--) res << ' ';
				res << "[";
				indent++; bool comma = false;
				QVariantList m(val.toList());
				foreach (const QVariant &v, m) {
					if (comma) res << ",\n"; else { res << '\n'; comma = true; }
					for (int c = indent; c > 0; c--) res << ' ';
					if (!json_file_generate(res, v, indent)) return false;
				}
				indent--;
				if (comma) {
					res << '\n';
					for (int c = indent; c > 0; c--) res << ' ';
				}
				res << ']';
				indent--;
			} break;
			case QVariant::StringList: {
				//for (int c = indent; c > 0; c--) res << ' ';
				res << "[";
				indent++; bool comma = false;
				QStringList m(val.toStringList());
				foreach (const QString &v, m) {
					if (comma) res << ",\n"; else { res << '\n'; comma = true; }
					for (int c = indent; c > 0; c--) res << ' ';
					res << K8JSON::quote(v).toUtf8();
				}
				indent--;
				if (comma) {
					res << '\n';
					for (int c = indent; c > 0; c--) res << ' ';
				}
				res << ']';
				indent--;
			} break;
			default: return false;
		}
		return true;
	}

	bool JsonFile::save(const QVariant &variant)
	{
		if (!p->file.open(QIODevice::WriteOnly | QIODevice::Text))
			return false;
		QTextStream stream(&p->file);
                stream.setCodec("UTF-8");
		bool result = json_file_generate(stream, variant, 0);
		stream.flush();
		p->file.close();
		return result;
	}


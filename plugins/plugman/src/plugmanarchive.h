#ifndef PLUGMANARCHIVE_H
#define PLUGMANARCHIVE_H

#include <QString>

class PlugmanArchive
{
public:
    PlugmanArchive();

    static bool extract(const QString &file, const QString &path, QString *error);
};

#endif // PLUGMANARCHIVE_H

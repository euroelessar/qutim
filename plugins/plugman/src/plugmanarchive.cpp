#include "plugmanarchive.h"
#include <QFile>
#include <QDir>
#include <QLibrary>

#define DEFINE_FUNCTION(RETURN_TYPE, NAME, ARGUMENTS) \
    typedef RETURN_TYPE (*NAME ## _) ARGUMENTS; \
    static NAME ## _ NAME = NULL;

#define RESOLVE_FUNCTION(NAME) \
    NAME = reinterpret_cast<NAME ## _>(lib.resolve(#NAME)); \
    if (!NAME) \
        return false

#define RESOLVE_FUNCTION_2(NAME, NAME2) \
    NAME = reinterpret_cast<NAME ## _>(lib.resolve(#NAME)); \
    if (!NAME) \
        NAME = reinterpret_cast<NAME ## _>(lib.resolve(#NAME2)); \
    if (!NAME) \
        return false

namespace PlugmanLibArchive {

#define	ARCHIVE_EOF	  1
#define	ARCHIVE_OK	  0

DEFINE_FUNCTION(struct archive *, archive_read_new, ())
DEFINE_FUNCTION(int, archive_read_data_block, (struct archive *a, const void **buff, size_t *size, qint64 *offset))
DEFINE_FUNCTION(int, archive_read_support_compression_all, (struct archive *))
DEFINE_FUNCTION(int, archive_read_support_format_all, (struct archive *))
DEFINE_FUNCTION(int, archive_read_open_file, (struct archive *, const char *_filename, size_t _block_size))
DEFINE_FUNCTION(int, archive_read_next_header, (struct archive *, struct archive_entry **))
DEFINE_FUNCTION(const wchar_t *, archive_entry_pathname_w, (struct archive_entry *))
DEFINE_FUNCTION(int, archive_read_free, (struct archive *))
DEFINE_FUNCTION(int, archive_read_close, (struct archive *))
DEFINE_FUNCTION(const char	*, archive_error_string, (struct archive *))


static bool initLibArchive_helper()
{
    QLibrary lib(QLatin1String("archive"));
    if (!lib.load()) {
        lib.setFileName(QLatin1String("libarchive"));
        if (!lib.load()) {
            return false;
        }
    }
    RESOLVE_FUNCTION(archive_read_new);
    RESOLVE_FUNCTION(archive_read_data_block);
    RESOLVE_FUNCTION(archive_read_support_compression_all);
    RESOLVE_FUNCTION(archive_read_support_format_all);
    RESOLVE_FUNCTION(archive_read_open_file);
    RESOLVE_FUNCTION(archive_read_next_header);
    RESOLVE_FUNCTION(archive_entry_pathname_w);
    RESOLVE_FUNCTION_2(archive_read_free, archive_read_finish);
    RESOLVE_FUNCTION(archive_read_close);
    RESOLVE_FUNCTION(archive_error_string);
    return true;
}

static bool initLibArchive()
{
    static bool inited = false;
    static bool result = false;
    if (!inited) {
        result = initLibArchive_helper();
        inited = true;
    }
    return result;
}

}

using namespace PlugmanLibArchive;

PlugmanArchive::PlugmanArchive()
{
}

static bool plugman_copy_data(struct archive *ar, const QDir &dir, const QString &entry)
{
    // TODO: Add check for ../.-like hacks
    QString filePath = dir.filePath(entry);
    const QFileInfo fileInfo(filePath);
    const QString path = fileInfo.absolutePath();
    if (!dir.exists(path))
        dir.mkpath(path);
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly))
        return false;

    int r;
    const void *buff;
    size_t size;
    off_t offset;

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF)
            return true;
        if (r != ARCHIVE_OK)
            return false;
        qint64 written = file.write(reinterpret_cast<const char *>(buff), size);
        if (written != qint64(size))
            return false;
    }
    return true;
}

bool PlugmanArchive::extract(const QString &file, const QString &path, QString *error)
{
    if (!initLibArchive()) {
        *error = QLatin1String("Can not load \"libarchive\"");
        return false;
    }
    const QDir dir = path;
    const bool doExtract = true;
    const bool verbose = true;

    if (verbose)
        qDebug("Try to extract \"%s\" to \"%s\"", qPrintable(file), qPrintable(path));

    int r;
    struct archive *arc = archive_read_new();
    archive_read_support_compression_all(arc);
    archive_read_support_format_all(arc);
    struct archive_entry *entry;
    if ((r = archive_read_open_file(arc, QFile::encodeName(file).constData(), 10240))) {
        *error = QString::fromLocal8Bit(archive_error_string(arc));
        return false;
    }
    for (;;) {
        r = archive_read_next_header(arc, &entry);
        if (r == ARCHIVE_EOF)
            break;
        if (r != ARCHIVE_OK) {
            *error = QString::fromLocal8Bit(archive_error_string(arc));
            return false;
        }
        const QString fileName = QString::fromWCharArray(archive_entry_pathname_w(entry));
        if (verbose)
            qDebug("%s%s", doExtract ? "x " : "", qPrintable(fileName));
        if (doExtract)
            plugman_copy_data(arc, dir, fileName);
    }
    archive_read_close(arc);
    archive_read_free(arc);
    return true;
}

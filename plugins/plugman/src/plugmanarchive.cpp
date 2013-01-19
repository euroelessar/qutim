#include "plugmanarchive.h"
#include <QFile>
#include <QDir>
#include <QLibrary>

extern "C" {
struct archive* archive_read_new();
int archive_read_data_block(struct archive *a,
                                          const void **buff, size_t *size, qint64 *offset);
int archive_read_support_compression_all(struct archive *);
int archive_read_support_format_all(struct archive *);
int archive_read_open_file(struct archive *,
                                         const char *_filename, size_t _block_size);
int archive_read_next_header(struct archive *,
                                           struct archive_entry **);
const wchar_t * archive_entry_pathname_w(struct archive_entry *);
int archive_read_free(struct archive *);
int archive_read_close(struct archive *);
const char *archive_error_string(struct archive *);
}

#define	ARCHIVE_EOF	  1
#define	ARCHIVE_OK	  0

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
    qint64 offset;

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

/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "packagemodel.h"
#include <qutim/debug.h>
#include <qutim/systeminfo.h>

using namespace qutim_sdk_0_3;

enum Roles {
    ContentRole = Qt::UserRole,
    StateRole,
    DetailRole,
    AuthorNameRole,
    AuthorEmailRole,
    AuthorPageRole,
    DescriptionPage
};

PackageModel::PackageModel(QObject *parent)
    : QAbstractListModel(parent), m_engine(new PackageEngine(this)), m_mode(Newest),
      m_isLoading(false)
{
    m_pageSize = 20;
    m_pagesCount = 0;
    m_requestId = -1;
    QHash<int, QByteArray> names;
    names.insert(Qt::DisplayRole, "title");
    names.insert(Qt::DecorationRole, "imageSource");
    names.insert(DescriptionPage, "description");
    names.insert(StateRole, "status");
    names.insert(DetailRole, "detailPage");
    names.insert(AuthorNameRole, "authorName");
    names.insert(AuthorEmailRole, "authorEmail");
    names.insert(AuthorPageRole, "authorPage");
    setRoleNames(names);
    connect(m_engine, SIGNAL(contentsReceived(PackageEntry::List,qint64)),
            SLOT(onContentsReceived(PackageEntry::List,qint64)));
    connect(m_engine, SIGNAL(entryChanged(QString)), SLOT(onEntryChanged(QString)));
    if (m_engine->isInitialized()) {
        requestNextPage();
    } else {
        connect(m_engine, SIGNAL(engineInitialized()), SLOT(requestNextPage()));
    }
}

void PackageModel::setFilter(const QString &filter)
{
    if (m_filter != filter) {
        m_filter = filter;
        reset();
        emit filterChanged(m_filter);
    }
}

QString PackageModel::filter() const
{
    return m_filter;
}

void PackageModel::setSortMode(SortMode mode)
{
    if (m_mode == mode)
        return;
    m_mode = mode;
    reset();
    emit sortModeChanged(m_mode);
}

PackageModel::SortMode PackageModel::sortMode() const
{
    return m_mode;
}

void PackageModel::setPath(const QString &path)
{
    if (m_path == path)
        return;
    m_path = path;
    emit pathChanged(m_path);
}

QString PackageModel::path() const
{
    return m_path;
}

void PackageModel::setCategories(const QStringList &categories)
{
    if (categories == m_categories)
        return;
    m_categories = categories;
    //	m_engine->resolveCategories(categories);
    emit categoriesChanged(m_categories);
}

QStringList PackageModel::categories() const
{
    return m_categories;
}

bool PackageModel::isLoading() const
{
    return m_isLoading;
}

PackageEngine *PackageModel::engine() const
{
    return m_engine;
}

void PackageModel::reset()
{
    beginResetModel();
    m_pagesCount = 0;
    m_requestId = -1;
    m_indexes.clear();
    m_contents.clear();
    endResetModel();
    if (m_engine->isInitialized())
        requestNextPage();
}

int PackageModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_contents.size();
}

QVariant PackageModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_contents.size())
        return QVariant();
    const PackageEntry &entry = m_contents.at(index.row());
    switch (role) {
    case Qt::DecorationRole:
        return entry.content().smallPreviewPicture();
    case Qt::DisplayRole:
        return entry.content().name();
    case ContentRole:
        return qVariantFromValue(entry);
    case StateRole:
        return entry.status();
    case DetailRole:
        return entry.content().detailpage();
    case DescriptionPage:
        return entry.content().description().replace("\r", QString());
    case AuthorNameRole:
        return entry.content().author();
    case AuthorEmailRole:
        return entry.content().attribute(QLatin1String("email"));
    case AuthorPageRole:
        return entry.content().attribute(QLatin1String("profilepage"));
    default:
        return QVariant();
    }
}

void PackageModel::onContentsReceived(const PackageEntry::List &list, qint64 id)
{
    setIsLoading(false);
    if (m_requestId != id)
        return;
    debug() << "Contents received" << list.size();
    m_requestId = -1;
    if (list.size() == 0)
        return;
    beginInsertRows(QModelIndex(), m_contents.size(), m_contents.size() + list.size() - 1);
    for (int i = 0; i < list.size(); ++i) {
        const PackageEntry &entry = list.at(i);
        m_indexes.insert(entry.id(), m_contents.size());
        m_contents.append(entry);
    }
    endInsertRows();
    ++m_pagesCount;
}

void PackageModel::onEntryChanged(const QString &id)
{
    setIsLoading(false);
    const int index = m_indexes.value(id, -1);
    if (index == -1)
        return;
    const QModelIndex modelIndex = QAbstractListModel::index(index);
    emit dataChanged(modelIndex, modelIndex);
}

void PackageModel::setIsLoading(bool isLoading)
{
    if (m_isLoading != isLoading) {
        m_isLoading = isLoading;
        emit isLoadingChanged(isLoading);
    }
}

void PackageModel::requestNextPage()
{
    if (m_requestId != -1)
        return;

    m_requestId = m_engine->requestContents(
                m_engine->resolveCategories(m_categories),
                m_filter, static_cast<Attica::Provider::SortMode>(m_mode),
                m_pagesCount, m_pageSize);

    setIsLoading(true);
}

void PackageModel::remove(int index)
{
    m_engine->remove(m_contents.at(index));
}

void PackageModel::install(int index)
{
    m_engine->install(m_contents.at(index), m_path);

    setIsLoading(true);
}


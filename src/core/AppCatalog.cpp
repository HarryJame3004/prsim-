#include "AppCatalog.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSet>
#include <algorithm>
#pragma push_macro("signals")
#undef signals
#include <gio/gdesktopappinfo.h>
#include <gio/gio.h>
#pragma pop_macro("signals")

AppCatalog::AppCatalog(QStringList applicationDirs, QObject *parent)
    : QAbstractListModel(parent), applicationDirs_(std::move(applicationDirs)),
      settings_(QSettings::IniFormat, QSettings::UserScope, QStringLiteral("PrismDesktop"),
                QStringLiteral("PrismDesktop")) {
    favorites_ = settings_.value(QStringLiteral("applications/favorites")).toStringList();
    refresh();
}

int AppCatalog::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : static_cast<int>(visible_.size());
}

QVariant AppCatalog::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= visible_.size()) {
        return {};
    }
    const Entry &entry = entries_.at(visible_.at(index.row()));
    switch (role) {
    case IdRole:
        return entry.id;
    case NameRole:
        return entry.name;
    case DescriptionRole:
        return entry.description;
    case FavoriteRole:
        return favorites_.contains(entry.id);
    default:
        return {};
    }
}

QHash<int, QByteArray> AppCatalog::roleNames() const {
    return {{IdRole, "appId"},
            {NameRole, "appName"},
            {DescriptionRole, "description"},
            {FavoriteRole, "favorite"}};
}

QString AppCatalog::search() const { return search_; }
QString AppCatalog::lastError() const { return lastError_; }
QStringList AppCatalog::favorites() const { return favorites_; }

void AppCatalog::setSearch(const QString &value) {
    if (search_ == value) {
        return;
    }
    search_ = value;
    rebuildVisible();
    emit searchChanged();
}

void AppCatalog::refresh() {
    beginResetModel();
    entries_.clear();
    QSet<QString> seen;
    auto addInfo = [this, &seen](GAppInfo *info, const QString &id) {
        if (!info || id.isEmpty() || seen.contains(id) || !g_app_info_should_show(info)) {
            return;
        }
        const char *name = g_app_info_get_display_name(info);
        if (!name || !*name) {
            return;
        }
        seen.insert(id);
        const char *description = g_app_info_get_description(info);
        entries_.append({id, QString::fromUtf8(name),
                         description ? QString::fromUtf8(description) : QString()});
    };
    if (applicationDirs_.isEmpty()) {
        GList *list = g_app_info_get_all();
        for (GList *it = list; it; it = it->next) {
            auto *info = G_APP_INFO(it->data);
            const char *id = g_app_info_get_id(info);
            addInfo(info, id ? QString::fromUtf8(id) : QString());
        }
        g_list_free_full(list, g_object_unref);
    } else {
        for (const QString &directory : applicationDirs_) {
            QDirIterator iterator(directory, {QStringLiteral("*.desktop")}, QDir::Files,
                                  QDirIterator::Subdirectories);
            while (iterator.hasNext()) {
                const QString path = iterator.next();
                const QByteArray bytes = QFile::encodeName(path);
                GDesktopAppInfo *info = g_desktop_app_info_new_from_filename(bytes.constData());
                if (info) {
                    addInfo(G_APP_INFO(info), QFileInfo(path).fileName());
                    g_object_unref(info);
                }
            }
        }
    }
    std::sort(entries_.begin(), entries_.end(),
              [](const Entry &a, const Entry &b) { return a.name.localeAwareCompare(b.name) < 0; });
    endResetModel();
    rebuildVisible();
}

bool AppCatalog::launch(const QString &id) {
    const auto found = std::find_if(entries_.cbegin(), entries_.cend(),
                                    [&id](const Entry &entry) { return entry.id == id; });
    if (found == entries_.cend()) {
        setError(QStringLiteral("Application is not in the catalog"));
        return false;
    }
    const QByteArray bytes = id.toUtf8();
    GDesktopAppInfo *info = g_desktop_app_info_new(bytes.constData());
    if (!info && !applicationDirs_.isEmpty()) {
        for (const QString &directory : applicationDirs_) {
            const QByteArray path = QFile::encodeName(QDir(directory).filePath(id));
            info = g_desktop_app_info_new_from_filename(path.constData());
            if (info)
                break;
        }
    }
    if (!info) {
        setError(QStringLiteral("Application entry is no longer available"));
        return false;
    }
    GError *error = nullptr;
    const gboolean result = g_app_info_launch(G_APP_INFO(info), nullptr, nullptr, &error);
    if (!result) {
        setError(error ? QString::fromUtf8(error->message) : QStringLiteral("Launch failed"));
    } else {
        setError({});
    }
    if (error)
        g_error_free(error);
    g_object_unref(info);
    return result;
}

void AppCatalog::toggleFavorite(const QString &id) {
    const auto found = std::find_if(entries_.cbegin(), entries_.cend(),
                                    [&id](const Entry &entry) { return entry.id == id; });
    if (found == entries_.cend())
        return;
    if (favorites_.contains(id))
        favorites_.removeAll(id);
    else
        favorites_.append(id);
    settings_.setValue(QStringLiteral("applications/favorites"), favorites_);
    rebuildVisible();
    emit favoritesChanged();
}

void AppCatalog::moveFavorite(const QString &id, int offset) {
    const qsizetype from = favorites_.indexOf(id);
    const qsizetype to = from + offset;
    if (from < 0 || to < 0 || to >= favorites_.size())
        return;
    favorites_.move(from, to);
    settings_.setValue(QStringLiteral("applications/favorites"), favorites_);
    rebuildVisible();
    emit favoritesChanged();
}

void AppCatalog::rebuildVisible() {
    beginResetModel();
    visible_.clear();
    for (int i = 0; i < entries_.size(); ++i) {
        const Entry &entry = entries_.at(i);
        if (search_.isEmpty() || entry.name.contains(search_, Qt::CaseInsensitive) ||
            entry.description.contains(search_, Qt::CaseInsensitive)) {
            visible_.append(i);
        }
    }
    std::stable_sort(visible_.begin(), visible_.end(), [this](int a, int b) {
        const qsizetype ia = favorites_.indexOf(entries_.at(a).id);
        const qsizetype ib = favorites_.indexOf(entries_.at(b).id);
        if (ia < 0)
            return false;
        if (ib < 0)
            return true;
        return ia < ib;
    });
    endResetModel();
}

void AppCatalog::setError(const QString &message) {
    if (lastError_ == message)
        return;
    lastError_ = message;
    emit lastErrorChanged();
}

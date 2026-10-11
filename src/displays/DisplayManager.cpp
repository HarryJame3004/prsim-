#include "DisplayManager.h"

#include <QGuiApplication>
#include <QScreen>
#include <QSet>

DisplayManager::DisplayManager(bool autoDetect, QObject *parent)
    : QAbstractListModel(parent),
      settings_(QSettings::IniFormat, QSettings::UserScope, QStringLiteral("PrismDesktop"),
                QStringLiteral("PrismDesktop")) {
    if (!autoDetect)
        return;
    auto watchScreen = [this](QScreen *screen) {
        connect(screen, &QScreen::geometryChanged, this, &DisplayManager::refreshFromScreens);
    };
    for (QScreen *screen : QGuiApplication::screens())
        watchScreen(screen);
    connect(qGuiApp, &QGuiApplication::screenAdded, this, [this, watchScreen](QScreen *screen) {
        watchScreen(screen);
        refreshFromScreens();
    });
    connect(qGuiApp, &QGuiApplication::screenRemoved, this, &DisplayManager::refreshFromScreens);
    connect(qGuiApp, &QGuiApplication::primaryScreenChanged, this,
            &DisplayManager::refreshFromScreens);
    refreshFromScreens();
}

int DisplayManager::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : static_cast<int>(displays_.size());
}

QVariant DisplayManager::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= displays_.size())
        return {};
    const DisplayInfo &display = displays_.at(index.row());
    switch (role) {
    case IdRole:
        return display.id;
    case NameRole:
        return display.name;
    case GeometryRole:
        return display.geometry;
    case InternalRole:
        return display.internal;
    case SystemPrimaryRole:
        return display.systemPrimary;
    case PreviewPrimaryRole:
        return display.id == previewPrimaryId_;
    default:
        return {};
    }
}

QHash<int, QByteArray> DisplayManager::roleNames() const {
    return {{IdRole, "displayId"},
            {NameRole, "displayName"},
            {GeometryRole, "displayGeometry"},
            {InternalRole, "internalDisplay"},
            {SystemPrimaryRole, "systemPrimary"},
            {PreviewPrimaryRole, "previewPrimary"}};
}

QString DisplayManager::previewPrimaryId() const { return previewPrimaryId_; }
int DisplayManager::displayCount() const { return static_cast<int>(displays_.size()); }

bool DisplayManager::choosePreviewPrimary(const QString &id) {
    bool found = false;
    for (const DisplayInfo &display : displays_)
        if (display.id == id)
            found = true;
    if (!found)
        return false;
    if (previewPrimaryId_ == id)
        return true;
    previewPrimaryId_ = id;
    if (!displays_.isEmpty())
        emit dataChanged(index(0), index(static_cast<int>(displays_.size() - 1)),
                         {PreviewPrimaryRole});
    emit displaysChanged();
    return true;
}

bool DisplayManager::saveProfile(const QString &name) {
    if (name.isEmpty() || name.size() > 64 || name.contains(QLatin1Char('/')) ||
        name.contains(QLatin1Char('\\')) || displays_.isEmpty())
        return false;
    QStringList ids;
    for (const DisplayInfo &display : displays_)
        ids.append(display.id);
    const QString prefix = QStringLiteral("displays/profiles/") + name + QLatin1Char('/');
    settings_.setValue(prefix + QStringLiteral("ids"), ids);
    settings_.setValue(prefix + QStringLiteral("primary"), previewPrimaryId_);
    settings_.sync();
    return settings_.status() == QSettings::NoError;
}

bool DisplayManager::loadProfile(const QString &name) {
    const QString prefix = QStringLiteral("displays/profiles/") + name + QLatin1Char('/');
    const QString id = settings_.value(prefix + QStringLiteral("primary")).toString();
    return choosePreviewPrimary(id);
}

void DisplayManager::updateDisplays(const QVector<DisplayInfo> &displays) {
    beginResetModel();
    displays_.clear();
    QSet<QString> seen;
    for (const DisplayInfo &display : displays) {
        if (display.id.isEmpty() || seen.contains(display.id))
            continue;
        seen.insert(display.id);
        displays_.append(display);
    }
    endResetModel();
    bool previousPresent = false;
    for (const DisplayInfo &display : displays_)
        if (display.id == previewPrimaryId_)
            previousPresent = true;
    if (!previousPresent) {
        previewPrimaryId_.clear();
        for (const DisplayInfo &display : displays_)
            if (display.systemPrimary) {
                previewPrimaryId_ = display.id;
                break;
            }
        if (previewPrimaryId_.isEmpty() && !displays_.isEmpty())
            previewPrimaryId_ = displays_.first().id;
    }
    emit displaysChanged();
}

void DisplayManager::refreshFromScreens() {
    QVector<DisplayInfo> screens;
    for (QScreen *screen : QGuiApplication::screens()) {
        const QString id = screen->name();
        screens.append({id, screen->model().isEmpty() ? id : screen->model(), screen->geometry(),
                        id.startsWith(QStringLiteral("eDP")) ||
                            id.startsWith(QStringLiteral("LVDS")) ||
                            id.startsWith(QStringLiteral("DSI")),
                        screen == QGuiApplication::primaryScreen()});
    }
    updateDisplays(screens);
}

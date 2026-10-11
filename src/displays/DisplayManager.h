#pragma once

#include <QAbstractListModel>
#include <QRect>
#include <QSettings>
#include <QVector>

struct DisplayInfo {
    QString id;
    QString name;
    QRect geometry;
    bool internal = false;
    bool systemPrimary = false;
};

class DisplayManager final : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QString previewPrimaryId READ previewPrimaryId NOTIFY displaysChanged)
    Q_PROPERTY(int displayCount READ displayCount NOTIFY displaysChanged)

  public:
    enum Role {
        IdRole = Qt::UserRole + 1,
        NameRole,
        GeometryRole,
        InternalRole,
        SystemPrimaryRole,
        PreviewPrimaryRole
    };
    explicit DisplayManager(bool autoDetect = true, QObject *parent = nullptr);
    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] QString previewPrimaryId() const;
    [[nodiscard]] int displayCount() const;
    Q_INVOKABLE bool choosePreviewPrimary(const QString &id);
    Q_INVOKABLE bool saveProfile(const QString &name);
    Q_INVOKABLE bool loadProfile(const QString &name);
    void updateDisplays(const QVector<DisplayInfo> &displays);
    void refreshFromScreens();

  signals:
    void displaysChanged();

  private:
    QVector<DisplayInfo> displays_;
    QString previewPrimaryId_;
    QSettings settings_;
};

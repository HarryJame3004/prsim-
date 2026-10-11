#pragma once

#include <QAbstractListModel>
#include <QSettings>
#include <QStringList>
#include <QVector>

class AppCatalog final : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QString search READ search WRITE setSearch NOTIFY searchChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(QStringList favorites READ favorites NOTIFY favoritesChanged)

  public:
    enum Role { IdRole = Qt::UserRole + 1, NameRole, DescriptionRole, FavoriteRole };
    explicit AppCatalog(QStringList applicationDirs = {}, QObject *parent = nullptr);
    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] QString search() const;
    [[nodiscard]] QString lastError() const;
    [[nodiscard]] QStringList favorites() const;
    void setSearch(const QString &search);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE bool launch(const QString &id);
    Q_INVOKABLE void toggleFavorite(const QString &id);
    Q_INVOKABLE void moveFavorite(const QString &id, int offset);

  signals:
    void searchChanged();
    void lastErrorChanged();
    void favoritesChanged();

  private:
    struct Entry {
        QString id;
        QString name;
        QString description;
    };
    void rebuildVisible();
    void setError(const QString &message);

    QStringList applicationDirs_;
    QVector<Entry> entries_;
    QVector<int> visible_;
    QStringList favorites_;
    QString search_;
    QString lastError_;
    QSettings settings_;
};

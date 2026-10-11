#pragma once

#include <QObject>
#include <QSettings>

class AppearanceSettings final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(QString accent READ accent WRITE setAccent NOTIFY accentChanged)
    Q_PROPERTY(double opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
    Q_PROPERTY(
        QString visualProfile READ visualProfile WRITE setVisualProfile NOTIFY visualProfileChanged)
    Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius NOTIFY cornerRadiusChanged)

  public:
    explicit AppearanceSettings(QObject *parent = nullptr);
    [[nodiscard]] QString mode() const;
    [[nodiscard]] QString accent() const;
    [[nodiscard]] double opacity() const;
    [[nodiscard]] QString visualProfile() const;
    [[nodiscard]] int cornerRadius() const;
    void setMode(const QString &mode);
    void setAccent(const QString &accent);
    void setOpacity(double opacity);
    void setVisualProfile(const QString &profile);
    void setCornerRadius(int radius);

  signals:
    void modeChanged();
    void accentChanged();
    void opacityChanged();
    void visualProfileChanged();
    void cornerRadiusChanged();

  private:
    QSettings settings_;
};

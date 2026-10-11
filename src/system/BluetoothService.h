#pragma once

#include <QObject>
#include <QProcess>
#include <QTimer>

class BluetoothService final : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool available READ available NOTIFY changed)
    Q_PROPERTY(bool powered READ powered NOTIFY changed)
    Q_PROPERTY(QString controllerName READ controllerName NOTIFY changed)
    Q_PROPERTY(QString lastError READ lastError NOTIFY changed)

  public:
    explicit BluetoothService(QString program = QStringLiteral("bluetoothctl"),
                              QObject *parent = nullptr);
    [[nodiscard]] bool available() const;
    [[nodiscard]] bool powered() const;
    [[nodiscard]] QString controllerName() const;
    [[nodiscard]] QString lastError() const;
    Q_INVOKABLE void refresh();

  signals:
    void changed();

  private:
    QString program_;
    QProcess process_;
    QTimer pollTimer_;
    QTimer commandTimer_;
    bool available_ = false;
    bool powered_ = false;
    QString controllerName_;
    QString lastError_;
};

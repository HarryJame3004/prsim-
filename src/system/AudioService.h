#pragma once

#include <QObject>
#include <QProcess>
#include <QString>
#include <QTimer>

class AudioService final : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool available READ available NOTIFY changed)
    Q_PROPERTY(int volumePercent READ volumePercent NOTIFY changed)
    Q_PROPERTY(bool muted READ muted NOTIFY changed)
    Q_PROPERTY(QString lastError READ lastError NOTIFY changed)

  public:
    explicit AudioService(QString pactlPath = QStringLiteral("pactl"), QObject *parent = nullptr);
    [[nodiscard]] bool available() const;
    [[nodiscard]] int volumePercent() const;
    [[nodiscard]] bool muted() const;
    [[nodiscard]] QString lastError() const;
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void setVolume(int percent);
    Q_INVOKABLE void toggleMute();

  signals:
    void changed();

  private:
    enum class Operation { None, QueryVolume, QueryMute, SetVolume, ToggleMute };
    void start(Operation operation, const QStringList &arguments);
    void finish(int exitCode, QProcess::ExitStatus status);
    void fail(const QString &message);

    QString pactlPath_;
    QProcess process_;
    QTimer timer_;
    QTimer commandTimer_;
    Operation operation_ = Operation::None;
    bool available_ = false;
    int volumePercent_ = -1;
    bool muted_ = false;
    QString lastError_;
};

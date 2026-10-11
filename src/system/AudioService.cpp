#include "AudioService.h"

#include <QRegularExpression>
#include <QtGlobal>

AudioService::AudioService(QString pactlPath, QObject *parent)
    : QObject(parent), pactlPath_(std::move(pactlPath)) {
    connect(&process_, &QProcess::finished, this, &AudioService::finish);
    connect(&process_, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart) {
            commandTimer_.stop();
            operation_ = Operation::None;
            fail(process_.errorString());
        }
    });
    commandTimer_.setSingleShot(true);
    commandTimer_.setInterval(3000);
    connect(&commandTimer_, &QTimer::timeout, this, [this] {
        if (process_.state() != QProcess::NotRunning)
            process_.kill();
    });
    timer_.setInterval(5000);
    connect(&timer_, &QTimer::timeout, this, &AudioService::refresh);
    timer_.start();
    refresh();
}

bool AudioService::available() const { return available_; }
int AudioService::volumePercent() const { return volumePercent_; }
bool AudioService::muted() const { return muted_; }
QString AudioService::lastError() const { return lastError_; }

void AudioService::refresh() {
    if (operation_ != Operation::None)
        return;
    start(Operation::QueryVolume,
          {QStringLiteral("get-sink-volume"), QStringLiteral("@DEFAULT_SINK@")});
}

void AudioService::setVolume(int percent) {
    if (operation_ != Operation::None)
        return;
    percent = qBound(0, percent, 100);
    start(Operation::SetVolume,
          {QStringLiteral("set-sink-volume"), QStringLiteral("@DEFAULT_SINK@"),
           QString::number(percent) + QLatin1Char('%')});
}

void AudioService::toggleMute() {
    if (operation_ != Operation::None)
        return;
    start(Operation::ToggleMute, {QStringLiteral("set-sink-mute"), QStringLiteral("@DEFAULT_SINK@"),
                                  QStringLiteral("toggle")});
}

void AudioService::start(Operation operation, const QStringList &arguments) {
    operation_ = operation;
    commandTimer_.start();
    process_.start(pactlPath_, arguments);
}

void AudioService::finish(int exitCode, QProcess::ExitStatus status) {
    commandTimer_.stop();
    const Operation completed = operation_;
    operation_ = Operation::None;
    if (status != QProcess::NormalExit || exitCode != 0) {
        const QString message = QString::fromUtf8(process_.readAllStandardError()).trimmed();
        fail(message.isEmpty() ? QStringLiteral("Audio service unavailable") : message);
        return;
    }
    const QString output = QString::fromUtf8(process_.readAllStandardOutput());
    if (completed == Operation::QueryVolume) {
        const QRegularExpression expression(QStringLiteral("(\\d+)%"));
        const QRegularExpressionMatch match = expression.match(output);
        if (!match.hasMatch()) {
            fail(QStringLiteral("Audio volume response was invalid"));
            return;
        }
        volumePercent_ = qBound(0, match.captured(1).toInt(), 100);
        start(Operation::QueryMute,
              {QStringLiteral("get-sink-mute"), QStringLiteral("@DEFAULT_SINK@")});
        return;
    }
    if (completed == Operation::QueryMute) {
        if (!output.contains(QStringLiteral("yes"), Qt::CaseInsensitive) &&
            !output.contains(QStringLiteral("no"), Qt::CaseInsensitive)) {
            fail(QStringLiteral("Audio mute response was invalid"));
            return;
        }
        muted_ = output.contains(QStringLiteral("yes"), Qt::CaseInsensitive);
        available_ = true;
        lastError_.clear();
        emit changed();
        return;
    }
    if (completed == Operation::SetVolume || completed == Operation::ToggleMute)
        refresh();
}

void AudioService::fail(const QString &message) {
    available_ = false;
    volumePercent_ = -1;
    lastError_ = message;
    emit changed();
}

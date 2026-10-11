#include "BluetoothService.h"

#include <QRegularExpression>

BluetoothService::BluetoothService(QString program, QObject *parent)
    : QObject(parent), program_(std::move(program)) {
    commandTimer_.setSingleShot(true);
    commandTimer_.setInterval(3000);
    connect(&commandTimer_, &QTimer::timeout, this, [this] {
        if (process_.state() != QProcess::NotRunning)
            process_.kill();
    });
    connect(&process_, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        if (error != QProcess::FailedToStart)
            return;
        commandTimer_.stop();
        available_ = false;
        lastError_ = process_.errorString();
        emit changed();
    });
    connect(
        &process_, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus status) {
            commandTimer_.stop();
            if (status != QProcess::NormalExit || exitCode != 0) {
                available_ = false;
                const QString error = QString::fromUtf8(process_.readAllStandardError()).trimmed();
                lastError_ = error.isEmpty() ? QStringLiteral("Bluetooth unavailable") : error;
                emit changed();
                return;
            }
            const QString output = QString::fromUtf8(process_.readAllStandardOutput());
            const QRegularExpression poweredPattern(
                QStringLiteral("(?:^|\\n)\\s*Powered:\\s*(yes|no)"),
                QRegularExpression::CaseInsensitiveOption);
            const QRegularExpressionMatch match = poweredPattern.match(output);
            if (!match.hasMatch()) {
                available_ = false;
                lastError_ = QStringLiteral("No Bluetooth controller found");
                emit changed();
                return;
            }
            available_ = true;
            powered_ = match.captured(1).compare(QStringLiteral("yes"), Qt::CaseInsensitive) == 0;
            const QRegularExpression namePattern(QStringLiteral("(?:^|\\n)\\s*Name:\\s*([^\\n]+)"));
            const QRegularExpressionMatch nameMatch = namePattern.match(output);
            controllerName_ = nameMatch.hasMatch() ? nameMatch.captured(1).trimmed() : QString();
            lastError_.clear();
            emit changed();
        });
    pollTimer_.setInterval(10000);
    connect(&pollTimer_, &QTimer::timeout, this, &BluetoothService::refresh);
    pollTimer_.start();
    refresh();
}

bool BluetoothService::available() const { return available_; }
bool BluetoothService::powered() const { return powered_; }
QString BluetoothService::controllerName() const { return controllerName_; }
QString BluetoothService::lastError() const { return lastError_; }

void BluetoothService::refresh() {
    if (process_.state() != QProcess::NotRunning)
        return;
    commandTimer_.start();
    process_.start(program_, {QStringLiteral("show")});
}

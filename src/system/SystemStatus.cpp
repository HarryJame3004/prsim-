#include "SystemStatus.h"

#include <QDir>
#include <QFile>
#include <QNetworkInterface>
#include <QRegularExpression>
#include <QtConcurrent>

namespace {
QByteArray readSmallFile(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return {};
    return file.read(4096);
}
} // namespace

SystemSnapshot LinuxSystemProbe::read() {
    SystemSnapshot result;
    const QList<QByteArray> cpuFields =
        readSmallFile(QStringLiteral("/proc/stat")).split('\n').value(0).simplified().split(' ');
    if (cpuFields.size() >= 5 && cpuFields.first() == "cpu") {
        quint64 total = 0;
        for (int i = 1; i < cpuFields.size(); ++i)
            total += cpuFields.at(i).toULongLong();
        const quint64 idle = cpuFields.at(4).toULongLong() + cpuFields.value(5).toULongLong();
        if (previousTotal_ && total > previousTotal_) {
            const quint64 deltaTotal = total - previousTotal_;
            const quint64 deltaIdle = idle >= previousIdle_ ? idle - previousIdle_ : 0;
            result.cpuPercent = 100.0 * (1.0 - double(deltaIdle) / double(deltaTotal));
        }
        previousTotal_ = total;
        previousIdle_ = idle;
    }

    const QByteArray memory = readSmallFile(QStringLiteral("/proc/meminfo"));
    auto memoryValue = [&memory](const QByteArray &name) -> quint64 {
        const QRegularExpression expression(QString::fromUtf8(name) +
                                            QStringLiteral(":\\s+(\\d+)"));
        const QRegularExpressionMatch match = expression.match(QString::fromUtf8(memory));
        return match.hasMatch() ? match.captured(1).toULongLong() : 0;
    };
    const quint64 totalMemory = memoryValue("MemTotal");
    const quint64 availableMemory = memoryValue("MemAvailable");
    if (totalMemory && availableMemory <= totalMemory)
        result.memoryPercent = 100.0 * double(totalMemory - availableMemory) / double(totalMemory);

    const QDir supplies(QStringLiteral("/sys/class/power_supply"));
    for (const QString &name : supplies.entryList({QStringLiteral("BAT*")}, QDir::Dirs)) {
        const QString base = supplies.filePath(name);
        bool valid = false;
        const int capacity =
            readSmallFile(base + QStringLiteral("/capacity")).trimmed().toInt(&valid);
        if (!valid || capacity < 0 || capacity > 100)
            continue;
        result.batteryPercent = capacity;
        result.charging = readSmallFile(base + QStringLiteral("/status")).trimmed() == "Charging";
        break;
    }

    for (const QNetworkInterface &interface : QNetworkInterface::allInterfaces()) {
        if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
            interface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack) &&
            !interface.addressEntries().isEmpty()) {
            result.networkConnected = true;
            break;
        }
    }
    return result;
}

SystemStatus::SystemStatus(std::shared_ptr<ISystemProbe> probe, QObject *parent)
    : QObject(parent), probe_(std::move(probe)) {
    timer_.setInterval(5000);
    connect(&timer_, &QTimer::timeout, this, &SystemStatus::refresh);
    connect(&watcher_, &QFutureWatcher<SystemSnapshot>::finished, this, [this] {
        snapshot_ = watcher_.result();
        emit updated();
    });
    timer_.start();
    refresh();
}

SystemStatus::~SystemStatus() {
    timer_.stop();
    watcher_.waitForFinished();
}

double SystemStatus::cpuPercent() const { return snapshot_.cpuPercent; }
double SystemStatus::memoryPercent() const { return snapshot_.memoryPercent; }
int SystemStatus::batteryPercent() const { return snapshot_.batteryPercent; }
bool SystemStatus::charging() const { return snapshot_.charging; }
bool SystemStatus::networkConnected() const { return snapshot_.networkConnected; }
int SystemStatus::pollingInterval() const { return timer_.interval(); }

void SystemStatus::setPollingInterval(int milliseconds) {
    const int validated = qBound(3000, milliseconds, 60000);
    if (validated == timer_.interval())
        return;
    timer_.setInterval(validated);
    emit pollingIntervalChanged();
}

void SystemStatus::refresh() {
    if (watcher_.isRunning())
        return;
    const auto probe = probe_;
    watcher_.setFuture(QtConcurrent::run([probe] { return probe->read(); }));
}

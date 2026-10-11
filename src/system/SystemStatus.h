#pragma once

#include <QFutureWatcher>
#include <QObject>
#include <QTimer>
#include <memory>

struct SystemSnapshot {
    double cpuPercent = -1;
    double memoryPercent = -1;
    int batteryPercent = -1;
    bool charging = false;
    bool networkConnected = false;
};

class ISystemProbe {
  public:
    virtual ~ISystemProbe() = default;
    [[nodiscard]] virtual SystemSnapshot read() = 0;
};

class LinuxSystemProbe final : public ISystemProbe {
  public:
    [[nodiscard]] SystemSnapshot read() override;

  private:
    quint64 previousTotal_ = 0;
    quint64 previousIdle_ = 0;
};

class SystemStatus final : public QObject {
    Q_OBJECT
    Q_PROPERTY(double cpuPercent READ cpuPercent NOTIFY updated)
    Q_PROPERTY(double memoryPercent READ memoryPercent NOTIFY updated)
    Q_PROPERTY(int batteryPercent READ batteryPercent NOTIFY updated)
    Q_PROPERTY(bool charging READ charging NOTIFY updated)
    Q_PROPERTY(bool networkConnected READ networkConnected NOTIFY updated)
    Q_PROPERTY(int pollingInterval READ pollingInterval WRITE setPollingInterval NOTIFY
                   pollingIntervalChanged)

  public:
    explicit SystemStatus(
        std::shared_ptr<ISystemProbe> probe = std::make_shared<LinuxSystemProbe>(),
        QObject *parent = nullptr);
    ~SystemStatus() override;
    [[nodiscard]] double cpuPercent() const;
    [[nodiscard]] double memoryPercent() const;
    [[nodiscard]] int batteryPercent() const;
    [[nodiscard]] bool charging() const;
    [[nodiscard]] bool networkConnected() const;
    [[nodiscard]] int pollingInterval() const;
    void setPollingInterval(int milliseconds);
    Q_INVOKABLE void refresh();

  signals:
    void updated();
    void pollingIntervalChanged();

  private:
    std::shared_ptr<ISystemProbe> probe_;
    SystemSnapshot snapshot_;
    QTimer timer_;
    QFutureWatcher<SystemSnapshot> watcher_;
};

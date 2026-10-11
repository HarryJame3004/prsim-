#include "SystemStatus.h"

#include <QTest>
#include <atomic>

class FakeProbe final : public ISystemProbe {
  public:
    [[nodiscard]] SystemSnapshot read() override {
        ++reads;
        return {42.0, 55.0, 73, true, true};
    }
    std::atomic_int reads = 0;
};

class SystemStatusTests final : public QObject {
    Q_OBJECT

  private slots:
    void injectedProbeUpdatesProperties();
    void linuxProbeReturnsValidRanges();
};

void SystemStatusTests::injectedProbeUpdatesProperties() {
    auto probe = std::make_shared<FakeProbe>();
    SystemStatus status(probe);
    QTRY_COMPARE(status.cpuPercent(), 42.0);
    QCOMPARE(status.memoryPercent(), 55.0);
    QCOMPARE(status.batteryPercent(), 73);
    QVERIFY(status.charging());
    QVERIFY(status.networkConnected());
    QVERIFY(probe->reads.load() >= 1);
    status.setPollingInterval(100);
    QCOMPARE(status.pollingInterval(), 3000);
    status.setPollingInterval(15000);
    QCOMPARE(status.pollingInterval(), 15000);
}

void SystemStatusTests::linuxProbeReturnsValidRanges() {
    LinuxSystemProbe probe;
    const SystemSnapshot initial = probe.read();
    QCOMPARE(initial.cpuPercent, -1.0);
    QTest::qWait(100);
    const SystemSnapshot snapshot = probe.read();
    QVERIFY(snapshot.cpuPercent >= 0 && snapshot.cpuPercent <= 100);
    QVERIFY(snapshot.memoryPercent >= 0 && snapshot.memoryPercent <= 100);
    QVERIFY(snapshot.batteryPercent >= -1 && snapshot.batteryPercent <= 100);
}

QTEST_GUILESS_MAIN(SystemStatusTests)
#include "SystemStatusTests.moc"

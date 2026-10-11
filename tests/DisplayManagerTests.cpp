#include "DisplayManager.h"

#include <QTemporaryDir>
#include <QTest>

class DisplayManagerTests final : public QObject {
    Q_OBJECT

  private slots:
    void initTestCase();
    void hotplugFallbackAndProfile();

  private:
    QTemporaryDir config_;
};

void DisplayManagerTests::initTestCase() {
    QVERIFY(config_.isValid());
    qputenv("XDG_CONFIG_HOME", config_.path().toUtf8());
}

void DisplayManagerTests::hotplugFallbackAndProfile() {
    DisplayManager manager(false);
    QCOMPARE(manager.displayCount(), 0);
    QVERIFY(!manager.choosePreviewPrimary(QStringLiteral("missing")));
    manager.updateDisplays(
        {{QStringLiteral("eDP-1"), QStringLiteral("Laptop"), {0, 0, 1920, 1080}, true, true},
         {QStringLiteral("HDMI-1"),
          QStringLiteral("External"),
          {1920, 0, 2560, 1440},
          false,
          false}});
    QCOMPARE(manager.displayCount(), 2);
    QCOMPARE(manager.previewPrimaryId(), QStringLiteral("eDP-1"));
    QVERIFY(manager.choosePreviewPrimary(QStringLiteral("HDMI-1")));
    QVERIFY(!manager.saveProfile(QStringLiteral("nested/profile")));
    QVERIFY(manager.saveProfile(QStringLiteral("desk")));
    manager.updateDisplays(
        {{QStringLiteral("eDP-1"), QStringLiteral("Laptop"), {0, 0, 1920, 1080}, true, true}});
    QCOMPARE(manager.previewPrimaryId(), QStringLiteral("eDP-1"));
    QVERIFY(!manager.loadProfile(QStringLiteral("desk")));
    manager.updateDisplays(
        {{QStringLiteral("eDP-1"), QStringLiteral("Laptop"), {0, 0, 1920, 1080}, true, true},
         {QStringLiteral("HDMI-1"),
          QStringLiteral("External"),
          {1920, 0, 2560, 1440},
          false,
          false}});
    QVERIFY(manager.loadProfile(QStringLiteral("desk")));
    QCOMPARE(manager.previewPrimaryId(), QStringLiteral("HDMI-1"));
}

QTEST_GUILESS_MAIN(DisplayManagerTests)
#include "DisplayManagerTests.moc"

#include "AppCatalog.h"
#include "AppearanceSettings.h"
#include "AudioService.h"
#include "BluetoothService.h"
#include "DisplayManager.h"
#include "SystemStatus.h"

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QTemporaryDir>
#include <QTest>
#include <QUrl>

class PreviewTests final : public QObject {
    Q_OBJECT

  private slots:
    void initTestCase();
    void previewLoadsAndCloses();
    void shellPanelsOpen();

  private:
    QTemporaryDir config_;
};

void PreviewTests::initTestCase() {
    QVERIFY(config_.isValid());
    qputenv("XDG_CONFIG_HOME", config_.path().toUtf8());
}

void PreviewTests::previewLoadsAndCloses() {
    AppCatalog catalog({QStringLiteral("/nonexistent/prism-test-apps")});
    AppearanceSettings settings;
    SystemStatus systemStatus;
    DisplayManager displays(false);
    AudioService audio(QStringLiteral("/nonexistent/prism-pactl"));
    BluetoothService bluetooth(QStringLiteral("/nonexistent/prism-bluetoothctl"));
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("appCatalog"), &catalog);
    engine.rootContext()->setContextProperty(QStringLiteral("appearanceSettings"), &settings);
    engine.rootContext()->setContextProperty(QStringLiteral("systemStatus"), &systemStatus);
    engine.rootContext()->setContextProperty(QStringLiteral("displayManager"), &displays);
    engine.rootContext()->setContextProperty(QStringLiteral("audioService"), &audio);
    engine.rootContext()->setContextProperty(QStringLiteral("bluetoothService"), &bluetooth);
    engine.load(QUrl(QStringLiteral("qrc:/prism/qml/PreviewWindow.qml")));
    QCOMPARE(engine.rootObjects().size(), 1);
    auto *window = qobject_cast<QQuickWindow *>(engine.rootObjects().constFirst());
    QVERIFY(window);
    QTRY_VERIFY(window->isVisible());
    const QImage frame = window->grabWindow();
    QVERIFY(!frame.isNull());
    const QString screenshotPath = qEnvironmentVariable("PRISM_SCREENSHOT_PATH");
    if (!screenshotPath.isEmpty())
        QVERIFY(frame.save(screenshotPath));
    QVERIFY(window->findChild<QObject *>(QStringLiteral("topBar")));
    QVERIFY(window->findChild<QObject *>(QStringLiteral("floatingDock")));
    auto *clock = window->findChild<QObject *>(QStringLiteral("clockLabel"));
    QVERIFY(clock);
    QVERIFY(!clock->property("text").toString().isEmpty());
    auto *button = window->findChild<QQuickItem *>(QStringLiteral("previewCloseButton"));
    QVERIFY(button);
    const QPoint point =
        button->mapToScene(QPointF(button->width() / 2, button->height() / 2)).toPoint();
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, point);
    QTRY_VERIFY(!window->isVisible());
}

void PreviewTests::shellPanelsOpen() {
    AppCatalog catalog({QStringLiteral("/nonexistent/prism-test-apps")});
    AppearanceSettings settings;
    SystemStatus systemStatus;
    DisplayManager displays(false);
    AudioService audio(QStringLiteral("/nonexistent/prism-pactl"));
    BluetoothService bluetooth(QStringLiteral("/nonexistent/prism-bluetoothctl"));
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("appCatalog"), &catalog);
    engine.rootContext()->setContextProperty(QStringLiteral("appearanceSettings"), &settings);
    engine.rootContext()->setContextProperty(QStringLiteral("systemStatus"), &systemStatus);
    engine.rootContext()->setContextProperty(QStringLiteral("displayManager"), &displays);
    engine.rootContext()->setContextProperty(QStringLiteral("audioService"), &audio);
    engine.rootContext()->setContextProperty(QStringLiteral("bluetoothService"), &bluetooth);
    engine.load(QUrl(QStringLiteral("qrc:/prism/qml/PreviewWindow.qml")));
    QCOMPARE(engine.rootObjects().size(), 1);
    auto *window = qobject_cast<QQuickWindow *>(engine.rootObjects().constFirst());
    QVERIFY(window);
    auto *dock = window->findChild<QQuickItem *>(QStringLiteral("floatingDock"));
    QVERIFY(dock);
    auto clickTile = [window, dock](int index) {
        const QPoint point = dock->mapToScene(QPointF(16 + index * 66 + 27, 39)).toPoint();
        QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, point);
    };
    clickTile(1);
    auto *search = window->findChild<QObject *>(QStringLiteral("searchPanel"));
    QVERIFY(search);
    QTRY_VERIFY(search->property("visible").toBool());
    clickTile(2);
    auto *panel = window->findChild<QObject *>(QStringLiteral("settingsPanel"));
    QVERIFY(panel);
    QTRY_VERIFY(panel->property("visible").toBool());
    QVERIFY(!search->property("visible").toBool());
    settings.setMode(QStringLiteral("Developer"));
    QTRY_VERIFY(!dock->isVisible());
    window->setProperty("dockReveal", true);
    QTRY_VERIFY(dock->isVisible());
    window->requestActivate();
    QTest::keyClick(window, Qt::Key_P, Qt::ControlModifier | Qt::AltModifier);
    QTRY_VERIFY(search->property("visible").toBool());
    auto *controlButton = window->findChild<QQuickItem *>(QStringLiteral("controlButton"));
    QVERIFY(controlButton);
    const QPoint controlPoint =
        controlButton->mapToScene(QPointF(controlButton->width() / 2, controlButton->height() / 2))
            .toPoint();
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, controlPoint);
    auto *controlCenter = window->findChild<QObject *>(QStringLiteral("controlCenter"));
    QVERIFY(controlCenter);
    QTRY_VERIFY(controlCenter->property("visible").toBool());
    const QString panelScreenshot = qEnvironmentVariable("PRISM_PANEL_SCREENSHOT_PATH");
    if (!panelScreenshot.isEmpty())
        QVERIFY(window->grabWindow().save(panelScreenshot));
}

QTEST_MAIN(PreviewTests)
#include "PreviewTests.moc"

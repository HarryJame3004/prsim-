#include "AppCatalog.h"
#include "AppearanceSettings.h"
#include "AudioService.h"
#include "BluetoothService.h"
#include "DisplayManager.h"
#include "SystemStatus.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Prism Desktop Preview"));
    AppCatalog catalog;
    AppearanceSettings settings;
    SystemStatus systemStatus;
    auto applyProfile = [&settings, &systemStatus] {
        const QString profile = settings.visualProfile();
        systemStatus.setPollingInterval(profile == QStringLiteral("Eco")    ? 15000
                                        : profile == QStringLiteral("Rich") ? 3000
                                                                            : 5000);
    };
    QObject::connect(&settings, &AppearanceSettings::visualProfileChanged, &app, applyProfile);
    applyProfile();
    DisplayManager displays;
    AudioService audio;
    BluetoothService bluetooth;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("appCatalog"), &catalog);
    engine.rootContext()->setContextProperty(QStringLiteral("appearanceSettings"), &settings);
    engine.rootContext()->setContextProperty(QStringLiteral("systemStatus"), &systemStatus);
    engine.rootContext()->setContextProperty(QStringLiteral("displayManager"), &displays);
    engine.rootContext()->setContextProperty(QStringLiteral("audioService"), &audio);
    engine.rootContext()->setContextProperty(QStringLiteral("bluetoothService"), &bluetooth);
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
        [] { QCoreApplication::exit(1); }, Qt::QueuedConnection);
    engine.load(QUrl(QStringLiteral("qrc:/prism/qml/PreviewWindow.qml")));
    if (engine.rootObjects().isEmpty()) {
        return 1;
    }
    return app.exec();
}

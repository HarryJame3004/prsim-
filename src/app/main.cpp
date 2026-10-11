#include "AppInfo.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Prism Desktop Foundation"));

    AppInfo appInfo;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("appInfo"), &appInfo);
    const QUrl url(QStringLiteral("qrc:/prism/qml/FoundationWindow.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
        [] { QCoreApplication::exit(1); }, Qt::QueuedConnection);
    engine.load(url);
    if (engine.rootObjects().isEmpty()) {
        return 1;
    }
    return app.exec();
}

#include "AppInfo.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QTest>
#include <QUrl>

class FoundationTests final : public QObject {
    Q_OBJECT

  private slots:
    void versionIsDefined();
    void qmlLoadsAndCloseButtonWorks();
};

void FoundationTests::versionIsDefined() {
    AppInfo appInfo;
    QCOMPARE(appInfo.version(), QStringLiteral("0.1.0"));
}

void FoundationTests::qmlLoadsAndCloseButtonWorks() {
    AppInfo appInfo;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("appInfo"), &appInfo);
    engine.load(QUrl(QStringLiteral("qrc:/prism/qml/FoundationWindow.qml")));
    QCOMPARE(engine.rootObjects().size(), 1);

    auto *window = qobject_cast<QQuickWindow *>(engine.rootObjects().constFirst());
    QVERIFY(window);
    QTRY_VERIFY(window->isVisible());

    auto *message = window->findChild<QObject *>(QStringLiteral("foundationMessage"));
    auto *version = window->findChild<QObject *>(QStringLiteral("versionLabel"));
    auto *button = window->findChild<QQuickItem *>(QStringLiteral("closeButton"));
    QVERIFY(message);
    QVERIFY(version);
    QVERIFY(button);
    QCOMPARE(message->property("text").toString(),
             QStringLiteral("Prism Desktop — Foundation Ready"));
    QCOMPARE(version->property("text").toString(), QStringLiteral("Version 0.1.0"));

    const QPoint clickPoint =
        button->mapToScene(QPointF(button->width() / 2, button->height() / 2)).toPoint();
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, clickPoint);
    QTRY_VERIFY(!window->isVisible());
}

QTEST_MAIN(FoundationTests)
#include "FoundationTests.moc"

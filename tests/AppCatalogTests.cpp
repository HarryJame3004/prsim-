#include "AppCatalog.h"
#include "AppearanceSettings.h"

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QTemporaryDir>
#include <QTest>

class AppCatalogTests final : public QObject {
    Q_OBJECT

  private slots:
    void initTestCase();
    void discoverySearchAndFavorites();
    void settingsValidateAndPersist();

  private:
    QTemporaryDir config_;
};

void AppCatalogTests::initTestCase() {
    QVERIFY(config_.isValid());
    qputenv("XDG_CONFIG_HOME", config_.path().toUtf8());
}

void AppCatalogTests::discoverySearchAndFavorites() {
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    QDir dir(temp.path());
    QVERIFY(dir.mkpath(QStringLiteral("applications")));
    auto writeEntry = [&dir](const QString &file, const QString &name, const QString &extra) {
        QFile out(dir.filePath(QStringLiteral("applications/") + file));
        if (!out.open(QIODevice::WriteOnly))
            return false;
        out.write((QStringLiteral("[Desktop Entry]\nType=Application\nName=") + name +
                   QStringLiteral("\nExec=/bin/true\n") + extra)
                      .toUtf8());
        return true;
    };
    QVERIFY(writeEntry(QStringLiteral("alpha.desktop"), QStringLiteral("Alpha Editor"), {}));
    QVERIFY(writeEntry(QStringLiteral("beta.desktop"), QStringLiteral("Beta Terminal"), {}));
    QVERIFY(writeEntry(QStringLiteral("hidden.desktop"), QStringLiteral("Hidden"),
                       QStringLiteral("NoDisplay=true\n")));

    AppCatalog catalog({dir.filePath(QStringLiteral("applications"))});
    QCOMPARE(catalog.rowCount(), 2);
    QVERIFY(!catalog.launch(QStringLiteral("not-an-app.desktop")));
    QCOMPARE(catalog.lastError(), QStringLiteral("Application is not in the catalog"));
    catalog.setSearch(QStringLiteral("terminal"));
    QCOMPARE(catalog.rowCount(), 1);
    QCOMPARE(catalog.data(catalog.index(0), AppCatalog::NameRole).toString(),
             QStringLiteral("Beta Terminal"));
    catalog.setSearch({});
    catalog.toggleFavorite(QStringLiteral("beta.desktop"));
    QCOMPARE(catalog.favorites(), QStringList{QStringLiteral("beta.desktop")});
    QCOMPARE(catalog.data(catalog.index(0), AppCatalog::IdRole).toString(),
             QStringLiteral("beta.desktop"));
    catalog.toggleFavorite(QStringLiteral("alpha.desktop"));
    catalog.moveFavorite(QStringLiteral("alpha.desktop"), -1);
    QCOMPARE(catalog.favorites().first(), QStringLiteral("alpha.desktop"));
    QVERIFY(catalog.launch(QStringLiteral("alpha.desktop")));
}

void AppCatalogTests::settingsValidateAndPersist() {
    AppearanceSettings settings;
    settings.setMode(QStringLiteral("invalid"));
    QVERIFY(settings.mode() == QStringLiteral("Everyday") ||
            settings.mode() == QStringLiteral("Developer"));
    settings.setMode(QStringLiteral("Developer"));
    settings.setAccent(QStringLiteral("not-a-color"));
    settings.setAccent(QStringLiteral("#83D7FF"));
    settings.setOpacity(4.0);
    settings.setVisualProfile(QStringLiteral("Eco"));
    settings.setCornerRadius(100);
    QCOMPARE(settings.mode(), QStringLiteral("Developer"));
    QCOMPARE(settings.accent(), QStringLiteral("#83d7ff"));
    QCOMPARE(settings.opacity(), 1.0);
    QCOMPARE(settings.visualProfile(), QStringLiteral("Eco"));
    QCOMPARE(settings.cornerRadius(), 32);
    AppearanceSettings reloaded;
    QCOMPARE(reloaded.mode(), QStringLiteral("Developer"));
    QCOMPARE(reloaded.accent(), QStringLiteral("#83d7ff"));
    QCOMPARE(reloaded.opacity(), 1.0);
    QCOMPARE(reloaded.visualProfile(), QStringLiteral("Eco"));
    QCOMPARE(reloaded.cornerRadius(), 32);
}

QTEST_MAIN(AppCatalogTests)
#include "AppCatalogTests.moc"

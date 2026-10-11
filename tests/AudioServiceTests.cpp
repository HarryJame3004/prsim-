#include "AudioService.h"

#include <QFile>
#include <QTemporaryDir>
#include <QTest>

class AudioServiceTests final : public QObject {
    Q_OBJECT

  private slots:
    void unavailableExecutableIsReported();
    void queryAndSetUseValidatedArguments();
    void hungCommandTimesOut();
};

void AudioServiceTests::unavailableExecutableIsReported() {
    AudioService service(QStringLiteral("/nonexistent/prism-pactl"));
    QTRY_VERIFY(!service.lastError().isEmpty());
    QVERIFY(!service.available());
}

void AudioServiceTests::queryAndSetUseValidatedArguments() {
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    const QString statePath = temp.filePath(QStringLiteral("volume"));
    QFile state(statePath);
    QVERIFY(state.open(QIODevice::WriteOnly));
    state.write("35%");
    state.close();
    const QString scriptPath = temp.filePath(QStringLiteral("fake-pactl"));
    QFile script(scriptPath);
    QVERIFY(script.open(QIODevice::WriteOnly));
    script.write((QStringLiteral("#!/bin/sh\ncase \"$1\" in\n") +
                  QStringLiteral("get-sink-volume) printf 'Volume: '; cat '") + statePath +
                  QStringLiteral("';;\nget-sink-mute) echo 'Mute: no';;\n") +
                  QStringLiteral("set-sink-volume) printf '%s' \"$3\" > '") + statePath +
                  QStringLiteral("';;\n*) exit 2;;\nesac\n"))
                     .toUtf8());
    script.close();
    QVERIFY(script.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner));

    AudioService service(scriptPath);
    QTRY_VERIFY(service.available());
    QCOMPARE(service.volumePercent(), 35);
    QVERIFY(!service.muted());
    service.setVolume(200);
    QTRY_COMPARE(service.volumePercent(), 100);
    QVERIFY(state.open(QIODevice::ReadOnly));
    QCOMPARE(state.readAll(), QByteArray("100%"));
}

void AudioServiceTests::hungCommandTimesOut() {
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    QFile script(temp.filePath(QStringLiteral("slow-pactl")));
    QVERIFY(script.open(QIODevice::WriteOnly));
    script.write("#!/bin/sh\nexec sleep 30\n");
    script.close();
    QVERIFY(script.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner));
    AudioService service(script.fileName());
    QTRY_VERIFY(!service.lastError().isEmpty());
    QVERIFY(!service.available());
}

QTEST_GUILESS_MAIN(AudioServiceTests)
#include "AudioServiceTests.moc"

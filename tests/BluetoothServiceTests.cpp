#include "BluetoothService.h"

#include <QFile>
#include <QTemporaryDir>
#include <QTest>

class BluetoothServiceTests final : public QObject {
    Q_OBJECT

  private slots:
    void unavailableExecutableIsReported();
    void controllerStateIsParsed();
};

void BluetoothServiceTests::unavailableExecutableIsReported() {
    BluetoothService service(QStringLiteral("/nonexistent/prism-bluetoothctl"));
    QTRY_VERIFY(!service.lastError().isEmpty());
    QVERIFY(!service.available());
}

void BluetoothServiceTests::controllerStateIsParsed() {
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    QFile script(temp.filePath(QStringLiteral("fake-bluetoothctl")));
    QVERIFY(script.open(QIODevice::WriteOnly));
    script.write("#!/bin/sh\n[ \"$1\" = show ] || exit 2\n"
                 "echo 'Controller 00:11:22:33:44:55 Laptop'\n"
                 "echo '    Name: Laptop Bluetooth'\n"
                 "echo '    Powered: yes'\n");
    script.close();
    QVERIFY(script.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner));
    BluetoothService service(script.fileName());
    QTRY_VERIFY(service.available());
    QVERIFY(service.powered());
    QCOMPARE(service.controllerName(), QStringLiteral("Laptop Bluetooth"));
}

QTEST_GUILESS_MAIN(BluetoothServiceTests)
#include "BluetoothServiceTests.moc"

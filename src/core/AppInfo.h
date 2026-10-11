#pragma once

#include <QObject>
#include <QString>

class AppInfo final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString version READ version CONSTANT)

  public:
    explicit AppInfo(QObject *parent = nullptr);
    [[nodiscard]] QString version() const;
};

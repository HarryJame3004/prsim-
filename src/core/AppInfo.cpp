#include "AppInfo.h"

AppInfo::AppInfo(QObject *parent) : QObject(parent) {}

QString AppInfo::version() const { return QStringLiteral(PRISM_VERSION); }

#include "AppearanceSettings.h"

#include <QColor>
#include <QtGlobal>

AppearanceSettings::AppearanceSettings(QObject *parent)
    : QObject(parent), settings_(QSettings::IniFormat, QSettings::UserScope,
                                 QStringLiteral("PrismDesktop"), QStringLiteral("PrismDesktop")) {}

QString AppearanceSettings::mode() const {
    const QString stored = settings_.value(QStringLiteral("appearance/mode")).toString();
    return stored == QStringLiteral("Developer") ? stored : QStringLiteral("Everyday");
}

QString AppearanceSettings::accent() const {
    const QString stored = settings_.value(QStringLiteral("appearance/accent")).toString();
    const QColor color(stored);
    return color.isValid() ? color.name(QColor::HexRgb) : QStringLiteral("#bba7ff");
}

double AppearanceSettings::opacity() const {
    const double stored = settings_.value(QStringLiteral("appearance/opacity"), 0.88).toDouble();
    return qBound(0.55, stored, 1.0);
}

QString AppearanceSettings::visualProfile() const {
    const QString stored = settings_.value(QStringLiteral("appearance/visualProfile")).toString();
    if (stored == QStringLiteral("Rich") || stored == QStringLiteral("Eco"))
        return stored;
    return QStringLiteral("Balanced");
}

int AppearanceSettings::cornerRadius() const {
    return qBound(8, settings_.value(QStringLiteral("appearance/cornerRadius"), 20).toInt(), 32);
}

void AppearanceSettings::setMode(const QString &value) {
    if ((value != QStringLiteral("Everyday") && value != QStringLiteral("Developer")) ||
        value == mode()) {
        return;
    }
    settings_.setValue(QStringLiteral("appearance/mode"), value);
    emit modeChanged();
}

void AppearanceSettings::setAccent(const QString &value) {
    const QColor color(value);
    if (!color.isValid() || color.name(QColor::HexRgb) == accent()) {
        return;
    }
    settings_.setValue(QStringLiteral("appearance/accent"), color.name(QColor::HexRgb));
    emit accentChanged();
}

void AppearanceSettings::setOpacity(double value) {
    if (!qIsFinite(value)) {
        return;
    }
    value = qBound(0.55, value, 1.0);
    if (qFuzzyCompare(value, opacity())) {
        return;
    }
    settings_.setValue(QStringLiteral("appearance/opacity"), value);
    emit opacityChanged();
}

void AppearanceSettings::setVisualProfile(const QString &value) {
    if ((value != QStringLiteral("Rich") && value != QStringLiteral("Balanced") &&
         value != QStringLiteral("Eco")) ||
        value == visualProfile())
        return;
    settings_.setValue(QStringLiteral("appearance/visualProfile"), value);
    emit visualProfileChanged();
}

void AppearanceSettings::setCornerRadius(int value) {
    value = qBound(8, value, 32);
    if (value == cornerRadius())
        return;
    settings_.setValue(QStringLiteral("appearance/cornerRadius"), value);
    emit cornerRadiusChanged();
}

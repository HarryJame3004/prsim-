#include "LayoutPlanner.h"

#include <QTest>

class LayoutPlannerTests final : public QObject {
    Q_OBJECT

  private slots:
    void floatingPreservesGeometry();
    void tilingPlacesWindowsWithoutOverlap();
    void hybridKeepsExceptionFloating();
    void smartChoosesFocus();
    void unsupportedAdapterReportsFailure();
};

void LayoutPlannerTests::floatingPreservesGeometry() {
    const QVector<WindowRecord> windows{{QStringLiteral("one"), {5, 6, 100, 80}, 0, false}};
    const LayoutPlan plan = LayoutPlanner::plan(windows, {0, 0, 800, 600}, 0, LayoutMode::Floating);
    QCOMPARE(plan.placements.size(), 1);
    QCOMPARE(plan.placements.first().geometry, QRect(5, 6, 100, 80));
    QVERIFY(!plan.placements.first().managed);
}

void LayoutPlannerTests::tilingPlacesWindowsWithoutOverlap() {
    const QVector<WindowRecord> windows{{QStringLiteral("one"), {}, 0, false},
                                        {QStringLiteral("two"), {}, 0, false},
                                        {QStringLiteral("other"), {}, 1, false}};
    const QRect area(0, 0, 800, 600);
    const LayoutPlan plan = LayoutPlanner::plan(windows, area, 0, LayoutMode::Tiling);
    QCOMPARE(plan.placements.size(), 2);
    for (const auto &placement : plan.placements) {
        QVERIFY(placement.managed);
        QVERIFY(area.contains(placement.geometry));
    }
    QVERIFY(!plan.placements.at(0).geometry.intersects(plan.placements.at(1).geometry));
}

void LayoutPlannerTests::hybridKeepsExceptionFloating() {
    const QVector<WindowRecord> windows{{QStringLiteral("editor"), {0, 0, 20, 20}, 0, false},
                                        {QStringLiteral("dialog"), {30, 40, 200, 100}, 0, true}};
    const LayoutPlan plan = LayoutPlanner::plan(windows, {0, 0, 800, 600}, 0, LayoutMode::Hybrid);
    QVERIFY(plan.placements.at(0).managed);
    QVERIFY(!plan.placements.at(1).managed);
    QCOMPARE(plan.placements.at(1).geometry, QRect(30, 40, 200, 100));
}

void LayoutPlannerTests::smartChoosesFocus() {
    const QVector<WindowRecord> windows{{QStringLiteral("editor"), {}, 0, false},
                                        {QStringLiteral("terminal"), {}, 0, false}};
    const LayoutPlan plan = LayoutPlanner::plan(windows, {0, 0, 800, 600}, 0, LayoutMode::Smart);
    QCOMPARE(plan.resolvedMode, LayoutMode::Focus);
    QVERIFY(plan.placements.at(0).geometry.width() > plan.placements.at(1).geometry.width());
}

void LayoutPlannerTests::unsupportedAdapterReportsFailure() {
    UnsupportedCompositorAdapter adapter;
    QVERIFY(!adapter.supportsWindowPlacement());
    const ApplyResult result = adapter.apply({});
    QVERIFY(!result.success);
    QVERIFY(!result.message.isEmpty());
}

QTEST_GUILESS_MAIN(LayoutPlannerTests)
#include "LayoutPlannerTests.moc"

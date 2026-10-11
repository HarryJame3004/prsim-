#include "LayoutPlanner.h"

#include <QtGlobal>
#include <cmath>

LayoutPlan LayoutPlanner::plan(const QVector<WindowRecord> &windows, const QRect &workArea,
                               int workspace, LayoutMode mode, int gap) {
    LayoutPlan result;
    if (mode == LayoutMode::Smart) {
        int count = 0;
        for (const WindowRecord &window : windows)
            if (window.workspace == workspace && !window.floatingException)
                ++count;
        mode = count <= 1 ? LayoutMode::Floating : LayoutMode::Focus;
    }
    result.resolvedMode = mode;
    QVector<int> managed;
    for (const WindowRecord &window : windows) {
        if (window.workspace != workspace)
            continue;
        const bool tile = mode != LayoutMode::Floating &&
                          !(mode == LayoutMode::Hybrid && window.floatingException);
        result.placements.append({window.id, window.geometry, tile});
        if (tile)
            managed.append(static_cast<int>(result.placements.size() - 1));
    }
    if (managed.isEmpty() || !workArea.isValid())
        return result;
    gap = qBound(0, gap, 64);
    const QRect inner = workArea.adjusted(gap, gap, -gap, -gap);
    if (inner.width() <= 0 || inner.height() <= 0)
        return result;
    if (mode == LayoutMode::Focus && managed.size() > 1) {
        const int mainWidth = qMax(1, int(std::round(inner.width() * 0.62)) - gap / 2);
        result.placements[managed.first()].geometry =
            QRect(inner.left(), inner.top(), mainWidth, inner.height());
        const int sideX = inner.left() + mainWidth + gap;
        const int sideWidth = qMax(1, inner.right() - sideX + 1);
        const int rows = static_cast<int>(managed.size() - 1);
        for (int row = 0; row < rows; ++row) {
            const int top = inner.top() + row * (inner.height() + gap) / rows;
            const int bottom = inner.top() + (row + 1) * (inner.height() + gap) / rows - gap;
            result.placements[managed.at(row + 1)].geometry =
                QRect(sideX, top, sideWidth, qMax(1, bottom - top));
        }
        return result;
    }
    const int columns = qMax(1, int(std::ceil(std::sqrt(double(managed.size())))));
    const int rows = static_cast<int>((managed.size() + columns - 1) / columns);
    for (int i = 0; i < managed.size(); ++i) {
        const int col = i % columns;
        const int row = i / columns;
        const int left = inner.left() + col * (inner.width() + gap) / columns;
        const int right = inner.left() + (col + 1) * (inner.width() + gap) / columns - gap;
        const int top = inner.top() + row * (inner.height() + gap) / rows;
        const int bottom = inner.top() + (row + 1) * (inner.height() + gap) / rows - gap;
        result.placements[managed.at(i)].geometry =
            QRect(left, top, qMax(1, right - left), qMax(1, bottom - top));
    }
    return result;
}

bool UnsupportedCompositorAdapter::supportsWindowPlacement() const { return false; }

ApplyResult UnsupportedCompositorAdapter::apply(const LayoutPlan &) {
    return {false, QStringLiteral("No validated compositor window-control adapter is available")};
}

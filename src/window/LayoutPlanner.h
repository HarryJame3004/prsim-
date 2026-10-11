#pragma once

#include <QRect>
#include <QString>
#include <QVector>

enum class LayoutMode { Floating, Tiling, Hybrid, Focus, Smart };

struct WindowRecord {
    QString id;
    QRect geometry;
    int workspace = 0;
    bool floatingException = false;
};

struct WindowPlacement {
    QString id;
    QRect geometry;
    bool managed = false;
};

struct LayoutPlan {
    LayoutMode resolvedMode = LayoutMode::Floating;
    QVector<WindowPlacement> placements;
};

class LayoutPlanner final {
  public:
    [[nodiscard]] static LayoutPlan plan(const QVector<WindowRecord> &windows,
                                         const QRect &workArea, int workspace, LayoutMode mode,
                                         int gap = 10);
};

struct ApplyResult {
    bool success = false;
    QString message;
};

class ICompositorAdapter {
  public:
    virtual ~ICompositorAdapter() = default;
    [[nodiscard]] virtual bool supportsWindowPlacement() const = 0;
    [[nodiscard]] virtual ApplyResult apply(const LayoutPlan &plan) = 0;
};

class UnsupportedCompositorAdapter final : public ICompositorAdapter {
  public:
    [[nodiscard]] bool supportsWindowPlacement() const override;
    [[nodiscard]] ApplyResult apply(const LayoutPlan &plan) override;
};

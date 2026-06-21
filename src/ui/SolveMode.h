#pragma once
#include <set>
#include <functional>
#include "../cube/Move.h"

enum class SolveAxis {
    HORIZONTAL,
    VERTICAL,
    NONE
};

enum class SolveVerticalSubAxis {
    X_AXIS, // R/L slices (along X)
    Z_AXIS  // F/B slices (along Z)
};

class SolveMode {
private:
    bool active;
    SolveAxis direction;
    std::set<int> selectedLayers;
    SolveVerticalSubAxis verticalSubAxis;

public:
    SolveMode();
    ~SolveMode();

    bool isActive() const;
    void toggle();
    void setActive(bool val);

    void setDirection(SolveAxis axis);
    SolveAxis getDirection() const;

    void toggleLayer(int layerIndex);
    void toggleVerticalLayer(int layerIndex, SolveVerticalSubAxis subAxis);
    void clearSelection();
    bool isLayerSelected(int layer) const;
    const std::set<int>& getSelectedLayers() const;
    bool hasSelection() const;
    SolveVerticalSubAxis getVerticalSubAxis() const;

    void executeRotation(Direction dir, int cubeSize,
                         std::function<void(const Move&)> queueFn);
};

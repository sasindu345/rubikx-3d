#include "SolveMode.h"

SolveMode::SolveMode() : active(false), direction(SolveAxis::NONE), verticalSubAxis(SolveVerticalSubAxis::X_AXIS) {}

SolveMode::~SolveMode() {}

bool SolveMode::isActive() const {
    return active;
}

void SolveMode::toggle() {
    active = !active;
    if (!active) {
        clearSelection();
        direction = SolveAxis::NONE;
    } else {
        // Default to HORIZONTAL when entering Solve Mode
        direction = SolveAxis::HORIZONTAL;
    }
}

void SolveMode::setActive(bool val) {
    active = val;
    if (!active) {
        clearSelection();
        direction = SolveAxis::NONE;
    } else if (direction == SolveAxis::NONE) {
        direction = SolveAxis::HORIZONTAL;
    }
}

void SolveMode::setDirection(SolveAxis axis) {
    direction = axis;
    clearSelection(); // Clear selection when changing axis to avoid weird state
}

SolveAxis SolveMode::getDirection() const {
    return direction;
}

void SolveMode::toggleLayer(int layerIndex) {
    if (selectedLayers.count(layerIndex)) {
        selectedLayers.erase(layerIndex);
    } else {
        selectedLayers.insert(layerIndex);
    }
}

void SolveMode::toggleVerticalLayer(int layerIndex, SolveVerticalSubAxis subAxis) {
    if (verticalSubAxis != subAxis) {
        clearSelection();
        verticalSubAxis = subAxis;
    }
    toggleLayer(layerIndex);
}

void SolveMode::clearSelection() {
    selectedLayers.clear();
}

bool SolveMode::isLayerSelected(int layer) const {
    return selectedLayers.count(layer) > 0;
}

const std::set<int>& SolveMode::getSelectedLayers() const {
    return selectedLayers;
}

bool SolveMode::hasSelection() const {
    return !selectedLayers.empty();
}

SolveVerticalSubAxis SolveMode::getVerticalSubAxis() const {
    return verticalSubAxis;
}

void SolveMode::executeRotation(Direction dir, int cubeSize,
                                 std::function<void(const Move&)> queueFn) {
    if (selectedLayers.empty() || direction == SolveAxis::NONE) return;

    if (direction == SolveAxis::HORIZONTAL) {
        for (int layerIndex : selectedLayers) {
            queueFn(Move(Face::UP, dir, cubeSize - 1 - layerIndex));
        }
    } else if (direction == SolveAxis::VERTICAL) {
        for (int layerIndex : selectedLayers) {
            if (verticalSubAxis == SolveVerticalSubAxis::X_AXIS) {
                queueFn(Move(Face::RIGHT, dir, cubeSize - 1 - layerIndex));
            } else {
                Direction adjustedDir = (dir == Direction::CW) ? Direction::CCW : Direction::CW;
                queueFn(Move(Face::FRONT, adjustedDir, cubeSize - 1 - layerIndex));
            }
        }
    }

    clearSelection();
}

#include "aniflip/LayerStack.h"

#include <stdexcept>

namespace aniflip {

Layer& LayerStack::addLayer(int width, int height, const std::string& name) {
    layers_.push_back(std::make_unique<Layer>(width, height, name));
    return *layers_.back();
}

void LayerStack::removeAt(size_t index) {
    if (index >= layers_.size()) throw std::out_of_range("LayerStack::removeAt");
    layers_.erase(layers_.begin() + static_cast<long>(index));
}

void LayerStack::moveLayer(size_t fromIndex, size_t toIndex) {
    if (fromIndex >= layers_.size() || toIndex >= layers_.size()) {
        throw std::out_of_range("LayerStack::moveLayer");
    }
    if (fromIndex == toIndex) return;
    auto layer = std::move(layers_[fromIndex]);
    layers_.erase(layers_.begin() + static_cast<long>(fromIndex));
    layers_.insert(layers_.begin() + static_cast<long>(toIndex), std::move(layer));
}

} // namespace aniflip

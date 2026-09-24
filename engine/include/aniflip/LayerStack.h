#pragma once

#include <memory>
#include <vector>

#include "aniflip/Layer.h"

namespace aniflip {

/// Pilha ordenada de camadas de um único frame. Índice 0 = fundo, último = topo.
class LayerStack {
public:
    Layer& addLayer(int width, int height, const std::string& name);

    size_t count() const { return layers_.size(); }
    Layer& at(size_t index) { return *layers_.at(index); }
    const Layer& at(size_t index) const { return *layers_.at(index); }

    void removeAt(size_t index);
    void moveLayer(size_t fromIndex, size_t toIndex);

private:
    std::vector<std::unique_ptr<Layer>> layers_;
};

} // namespace aniflip

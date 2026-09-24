#pragma once

#include "aniflip/LayerStack.h"

namespace aniflip {

/// Um frame de animação = uma pilha de camadas.
class Frame {
public:
    LayerStack& layers() { return layers_; }
    const LayerStack& layers() const { return layers_; }

private:
    LayerStack layers_;
};

} // namespace aniflip

#pragma once

#include <cstdint>
#include <vector>

#include "aniflip/LayerStack.h"

namespace aniflip {

/// Combina camadas visíveis de uma LayerStack em um buffer ARGB8888,
/// respeitando ordem (índice 0 = fundo), visibilidade e opacidade de cada camada.
class LayerCompositor {
public:
    /// Compõe o canvas inteiro (width x height).
    static std::vector<uint32_t> composite(const LayerStack& stack, int width, int height);

    /// Compõe apenas a sub-região [x0, x1] x [y0, y1] (inclusive) do canvas.
    /// Usado para atualizar só a parte da tela que mudou a cada traço, em vez
    /// de recompor o canvas inteiro a cada movimento do dedo.
    static std::vector<uint32_t> compositeRegion(
        const LayerStack& stack, int canvasWidth, int canvasHeight,
        int x0, int y0, int x1, int y1);
};

} // namespace aniflip

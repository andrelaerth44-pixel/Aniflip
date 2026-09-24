#pragma once

#include <cstdint>
#include <vector>

#include "aniflip/LayerStack.h"

namespace aniflip {

/// Combina todas as camadas visíveis de uma LayerStack em um único buffer ARGB8888,
/// respeitando ordem (índice 0 = fundo), visibilidade e opacidade de cada camada.
class LayerCompositor {
public:
    static std::vector<uint32_t> composite(const LayerStack& stack, int width, int height);
};

} // namespace aniflip

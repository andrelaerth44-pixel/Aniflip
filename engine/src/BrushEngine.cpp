#include "aniflip/BrushEngine.h"

#include <algorithm>
#include <cmath>

namespace aniflip {

void BrushEngine::beginStroke() {
    hasLastPoint_ = false;
}

DirtyRect BrushEngine::strokeTo(Layer& layer, int x, int y, int radius, uint32_t argbColor) {
    int fromX;
    int fromY;

    if (!hasLastPoint_) {
        layer.stampCircle(x, y, radius, argbColor);
        fromX = x;
        fromY = y;
        lastX_ = x;
        lastY_ = y;
        hasLastPoint_ = true;
    } else {
        fromX = lastX_;
        fromY = lastY_;

        const float dx = static_cast<float>(x - lastX_);
        const float dy = static_cast<float>(y - lastY_);
        const float distance = std::sqrt(dx * dx + dy * dy);

        // Espaçamento entre stamps: uma fração do raio do pincel, com mínimo de 1px,
        // para o traço ficar contínuo (sem "pontilhado") em qualquer velocidade.
        const float spacing = std::max(1.0f, static_cast<float>(radius) * 0.35f);
        const int steps = std::max(1, static_cast<int>(distance / spacing));

        for (int i = 1; i <= steps; ++i) {
            const float t = static_cast<float>(i) / static_cast<float>(steps);
            const int sx = lastX_ + static_cast<int>(std::round(dx * t));
            const int sy = lastY_ + static_cast<int>(std::round(dy * t));
            layer.stampCircle(sx, sy, radius, argbColor);
        }

        lastX_ = x;
        lastY_ = y;
    }

    // Retângulo alterado: bounding box do segmento desenhado, expandido pelo raio
    // do pincel (+ margem para o anti-aliasing), e recortado aos limites da camada.
    const int pad = radius + 2;
    DirtyRect rect;
    rect.x0 = std::max(0, std::min(fromX, x) - pad);
    rect.y0 = std::max(0, std::min(fromY, y) - pad);
    rect.x1 = std::min(layer.width() - 1, std::max(fromX, x) + pad);
    rect.y1 = std::min(layer.height() - 1, std::max(fromY, y) + pad);
    return rect;
}

} // namespace aniflip

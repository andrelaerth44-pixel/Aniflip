#include "aniflip/Layer.h"

#include <algorithm>
#include <cmath>

namespace aniflip {

namespace {

inline uint8_t channel(uint32_t argb, int shift) {
    return static_cast<uint8_t>((argb >> shift) & 0xFF);
}

inline uint32_t packARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return (static_cast<uint32_t>(a) << 24) |
           (static_cast<uint32_t>(r) << 16) |
           (static_cast<uint32_t>(g) << 8) |
           static_cast<uint32_t>(b);
}

/// Blend "source over" de um pixel de origem sobre um pixel de destino, ambos ARGB8888.
uint32_t blendSrcOver(uint32_t src, uint32_t dst) {
    const uint8_t srcA = channel(src, 24);
    if (srcA == 0) return dst;
    if (srcA == 255) return src;

    const float sa = srcA / 255.0f;
    const float da = channel(dst, 24) / 255.0f;
    const float outA = sa + da * (1.0f - sa);
    if (outA <= 0.0f) return 0;

    auto lerp = [&](int shift) {
        const float sc = channel(src, shift) / 255.0f;
        const float dc = channel(dst, shift) / 255.0f;
        const float outC = (sc * sa + dc * da * (1.0f - sa)) / outA;
        return static_cast<uint8_t>(std::clamp(outC * 255.0f, 0.0f, 255.0f));
    };

    return packARGB(
        static_cast<uint8_t>(std::clamp(outA * 255.0f, 0.0f, 255.0f)),
        lerp(16), lerp(8), lerp(0));
}

} // namespace

Layer::Layer(int width, int height, std::string name)
    : width_(width), height_(height), name_(std::move(name)),
      pixels_(static_cast<size_t>(width) * height, 0) {}

void Layer::setOpacity(float opacity) {
    opacity_ = std::clamp(opacity, 0.0f, 1.0f);
}

void Layer::clear() {
    std::fill(pixels_.begin(), pixels_.end(), 0u);
}

void Layer::stampCircle(int cx, int cy, int radius, uint32_t argbColor) {
    if (radius <= 0) return;

    const int minX = std::max(0, cx - radius);
    const int maxX = std::min(width_ - 1, cx + radius);
    const int minY = std::max(0, cy - radius);
    const int maxY = std::min(height_ - 1, cy + radius);

    const float r2 = static_cast<float>(radius) * static_cast<float>(radius);
    const uint8_t baseAlpha = channel(argbColor, 24);

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            const float dx = static_cast<float>(x - cx);
            const float dy = static_cast<float>(y - cy);
            const float dist2 = dx * dx + dy * dy;
            if (dist2 > r2) continue;

            // Anti-aliasing simples: suaviza a borda externa do círculo (~1.5px).
            const float dist = std::sqrt(dist2);
            const float edge = static_cast<float>(radius) - dist;
            const float aa = std::clamp(edge / 1.5f, 0.0f, 1.0f);
            const uint8_t a = static_cast<uint8_t>(baseAlpha * aa);
            if (a == 0) continue;

            const uint32_t src = packARGB(a, channel(argbColor, 16), channel(argbColor, 8), channel(argbColor, 0));
            const size_t idx = static_cast<size_t>(y) * width_ + x;
            pixels_[idx] = blendSrcOver(src, pixels_[idx]);
        }
    }
}

} // namespace aniflip

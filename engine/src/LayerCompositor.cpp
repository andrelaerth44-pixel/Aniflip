#include "aniflip/LayerCompositor.h"

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

uint32_t applyOpacity(uint32_t argb, float opacity) {
    if (opacity >= 1.0f) return argb;
    const uint8_t a = static_cast<uint8_t>(std::clamp(channel(argb, 24) * opacity, 0.0f, 255.0f));
    return packARGB(a, channel(argb, 16), channel(argb, 8), channel(argb, 0));
}

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

std::vector<uint32_t> LayerCompositor::composite(const LayerStack& stack, int width, int height) {
    std::vector<uint32_t> out(static_cast<size_t>(width) * height, 0);

    for (size_t i = 0; i < stack.count(); ++i) {
        const Layer& layer = stack.at(i);
        if (!layer.visible() || layer.opacity() <= 0.0f) continue;

        const auto& src = layer.pixels();
        for (size_t p = 0; p < out.size(); ++p) {
            const uint32_t srcPixel = applyOpacity(src[p], layer.opacity());
            out[p] = blendSrcOver(srcPixel, out[p]);
        }
    }

    return out;
}

} // namespace aniflip

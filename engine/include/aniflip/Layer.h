#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace aniflip {

/// Uma camada é um buffer de pixels ARGB8888 (0xAARRGGBB), do tamanho do canvas,
/// mais metadados de composição (nome, opacidade, visibilidade).
///
/// Nesta fase o motor não depende de nenhuma biblioteca externa de rasterização:
/// o objetivo é ter um pipeline de camadas + composição correto e 100% testável
/// antes de acoplar um motor de rasterização mais avançado (ex. Skia) no futuro.
class Layer {
public:
    Layer(int width, int height, std::string name);

    int width() const { return width_; }
    int height() const { return height_; }
    const std::string& name() const { return name_; }

    bool visible() const { return visible_; }
    void setVisible(bool visible) { visible_ = visible; }

    float opacity() const { return opacity_; }
    void setOpacity(float opacity);

    /// Acesso direto ao buffer de pixels (ARGB8888), tamanho width*height.
    std::vector<uint32_t>& pixels() { return pixels_; }
    const std::vector<uint32_t>& pixels() const { return pixels_; }

    /// Desenha um "stamp" circular sólido com anti-aliasing simples na posição (x, y).
    /// Usado pelo BrushEngine para compor um traço a partir de vários stamps.
    void stampCircle(int cx, int cy, int radius, uint32_t argbColor);

    /// Limpa a camada inteira (transparente).
    void clear();

private:
    int width_;
    int height_;
    std::string name_;
    bool visible_ = true;
    float opacity_ = 1.0f;
    std::vector<uint32_t> pixels_;
};

} // namespace aniflip

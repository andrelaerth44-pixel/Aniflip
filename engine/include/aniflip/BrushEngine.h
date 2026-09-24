#pragma once

#include <cstdint>

#include "aniflip/Layer.h"

namespace aniflip {

/// Retângulo (em coordenadas de canvas) que foi efetivamente alterado por uma
/// operação de desenho. Usado para atualizar só a região suja da tela, em vez
/// de recompor o canvas inteiro a cada movimento do dedo (o que causava
/// travamento perceptível).
struct DirtyRect {
    int x0 = 0, y0 = 0, x1 = -1, y1 = -1; // x1 < x0 (ou y1 < y0) => retângulo vazio
    bool empty() const { return x1 < x0 || y1 < y0; }
};

/// Motor de pincel: transforma uma sequência de pontos (um traço) em uma série
/// de "stamps" desenhados na camada, com espaçamento entre stamps para o traço
/// ficar contínuo mesmo em movimentos rápidos (sem "buracos").
///
/// Fase 1: pincel circular sólido de tamanho fixo por traço. Pressão, textura
/// e formatos de ponta vêm em fases seguintes.
class BrushEngine {
public:
    /// Desenha um segmento de traço entre o último ponto conhecido e (x, y).
    /// Na primeira chamada de um traço (sem ponto anterior, ou após beginStroke()),
    /// apenas estampa em (x, y). Retorna o retângulo que foi alterado.
    DirtyRect strokeTo(Layer& layer, int x, int y, int radius, uint32_t argbColor);

    /// Deve ser chamado ao iniciar um novo traço (ex.: dedo/caneta tocou a tela),
    /// para não interpolar/conectar a partir do traço anterior.
    void beginStroke();

private:
    bool hasLastPoint_ = false;
    int lastX_ = 0;
    int lastY_ = 0;
};

} // namespace aniflip

#pragma once

#include <cstdint>

#include "aniflip/Layer.h"

namespace aniflip {

/// Motor de pincel: transforma uma sequência de pontos (um traço) em uma série
/// de "stamps" desenhados na camada, com espaçamento entre stamps para o traço
/// ficar contínuo mesmo em movimentos rápidos (sem "buracos").
///
/// Fase 1: pincel circular sólido de tamanho fixo por traço. Pressão, textura
/// e formatos de ponta vêm em fases seguintes.
class BrushEngine {
public:
    /// Desenha um segmento de traço entre o último ponto conhecido e (x, y).
    /// Na primeira chamada de um traço (sem ponto anterior), apenas estampa em (x, y).
    void strokeTo(Layer& layer, int x, int y, int radius, uint32_t argbColor);

    /// Deve ser chamado ao iniciar um novo traço (ex.: dedo/caneta tocou a tela),
    /// para não interpolar a partir do traço anterior.
    void beginStroke();

private:
    bool hasLastPoint_ = false;
    int lastX_ = 0;
    int lastY_ = 0;
};

} // namespace aniflip

#pragma once

#include <memory>
#include <vector>

#include "aniflip/Frame.h"

namespace aniflip {

/// Sequência de frames de uma cena, com FPS associado.
/// Controla qual é o frame "corrente" sendo editado.
class FrameSequence {
public:
    explicit FrameSequence(int fps = 12) : fps_(fps) {}

    int fps() const { return fps_; }
    void setFps(int fps) { fps_ = fps; }

    Frame& addFrame();
    size_t count() const { return frames_.size(); }

    Frame& at(size_t index) { return *frames_.at(index); }
    const Frame& at(size_t index) const { return *frames_.at(index); }

    size_t currentIndex() const { return currentIndex_; }
    void setCurrentIndex(size_t index);
    Frame& current();

private:
    int fps_;
    std::vector<std::unique_ptr<Frame>> frames_;
    size_t currentIndex_ = 0;
};

} // namespace aniflip

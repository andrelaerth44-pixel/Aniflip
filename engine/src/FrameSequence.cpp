#include "aniflip/FrameSequence.h"

#include <stdexcept>

namespace aniflip {

Frame& FrameSequence::addFrame() {
    frames_.push_back(std::make_unique<Frame>());
    currentIndex_ = frames_.size() - 1;
    return *frames_.back();
}

void FrameSequence::setCurrentIndex(size_t index) {
    if (index >= frames_.size()) throw std::out_of_range("FrameSequence::setCurrentIndex");
    currentIndex_ = index;
}

Frame& FrameSequence::current() {
    if (frames_.empty()) throw std::runtime_error("FrameSequence vazio: chame addFrame() primeiro");
    return *frames_.at(currentIndex_);
}

} // namespace aniflip

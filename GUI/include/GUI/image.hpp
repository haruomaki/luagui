#pragma once

#include "Resource.hpp"
#include <Lunchbox/core/image.hpp>

class Image : public Resource, public lunchbox::core::ImageData {
  public:
    Image(lunchbox::core::ImageData &&image)
        : lunchbox::core::ImageData(std::move(image)) {}
};

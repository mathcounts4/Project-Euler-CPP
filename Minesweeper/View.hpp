#pragma once

#include "../TypeUtils.hpp"
#include "../VirtualDestructor.hpp"

#include <memory>
#include <vector>

struct Pixel {
    UC r;
    UC g;
    UC b;
    friend bool operator==(Pixel const& x, Pixel const& y);
    friend bool operator!=(Pixel const& x, Pixel const& y);
};

class Screen {
  public:
    static Screen capture();

  public:
    Pixel at(SZ col, SZ row) const;
    std::vector<std::vector<Pixel>> toLocal() const;
    
  private:
    Screen(std::unique_ptr<VirtualDestructor> dataOwner, UC const* rawData, SZ width, SZ height, SZ bytesPerPixel);

  private:
    std::unique_ptr<VirtualDestructor> fDataOwner;
    UC const* fRawData;
    SZ fWidth;
    SZ fHeight;
    SZ fBytesPerPixel;
};

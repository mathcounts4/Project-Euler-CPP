#include "OSX_ApplicationServices.hpp"
#include "View.hpp"

class MainImageRef : public VirtualDestructor {
  public:
    MainImageRef()
	: fImageRef(CGDisplayCreateImage(CGMainDisplayID())) {
    }
    ~MainImageRef() override;
    CGImageRef& operator*() {
	return fImageRef;
    }
  private:
    CGImageRef fImageRef;
};

MainImageRef::~MainImageRef() {
    CGImageRelease(fImageRef);
}

class DataRef {
  public:
    DataRef(CGImageRef image)
	: fDataRef(CGDataProviderCopyData(CGImageGetDataProvider(image))) {
    }
    ~DataRef() {
	CFRelease(fDataRef);
    }
    UC const* rawData() {
	return CFDataGetBytePtr(fDataRef);
    }
  private:
    CFDataRef fDataRef;
};

bool operator==(Pixel const& x, Pixel const& y) {
    return x.r == y.r && x.g == y.g && x.b == y.b;
}

bool operator!=(Pixel const& x, Pixel const& y) {
    return !(x == y);
}

Screen Screen::capture() {
    auto mainImage = std::make_unique<MainImageRef>();
    CGImageRef& ref = **mainImage;
    DataRef data(ref);
    SZ width = CGImageGetWidth(ref);
    SZ height = CGImageGetHeight(ref);
    SZ bytesPerPixel = CGImageGetBitsPerPixel(ref) / 8;
    return {std::move(mainImage), data.rawData(), width, height, bytesPerPixel};
}

Pixel Screen::at(SZ col, SZ row) const {
    UC const* pixelData = fRawData + (fWidth * row + col) * fBytesPerPixel;
    return {pixelData[2], pixelData[1], pixelData[0]};
}

std::vector<std::vector<Pixel>> Screen::toLocal() const {
    std::vector<std::vector<Pixel>> local(fHeight, std::vector<Pixel>(fWidth, {0, 0, 0}));
    for (decltype(fHeight) y = 0; y < fHeight; ++y) {
	for (decltype(fWidth) x = 0; x < fWidth; ++x) {
	    local[y][x] = at(x, y);
	}
    }
    return local;
}

Screen::Screen(std::unique_ptr<VirtualDestructor> dataOwner, UC const* rawData, SZ width, SZ height, SZ bytesPerPixel)
    : fDataOwner(std::move(dataOwner))
    , fRawData(rawData)
    , fWidth(width)
    , fHeight(height)
    , fBytesPerPixel(bytesPerPixel) {
}


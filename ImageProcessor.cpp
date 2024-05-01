#include "ImageProcessor.hpp"

namespace cvt {

QImage Color(const QImage &src, Format cvt_format) {
  QImage result = QImage(src.size(), QImage::Format_Grayscale8);
  for (size_t y = 0; y < src.height(); y++) {
    uchar *line = result.scanLine(y);
    for (size_t x = 0; x < src.width(); x++) {
      QRgb pixelVal = src.pixel(x, y);
      uint grayPixVal = qRed(pixelVal) * 0.299 + qGreen(pixelVal) * 0.587 +
                        qBlue(pixelVal) * 0.114;
      line[x] = grayPixVal;
    }
  }
  return result;
}

} // namespace cvt

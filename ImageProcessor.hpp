#pragma once

#include <QImage>

namespace cvt {

enum class Format { kRGB2Gray, kBGR2Gray };

QImage Color(const QImage &src, Format cvt_format);

} // namespace cvt

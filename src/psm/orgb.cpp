#include "psm/orgb.hpp"

namespace psm {

double convertToRGBangle(double theta) {
  if (theta < M_PI / 3) {
    return (3.0f / 2.0f) * theta;
  } else if (theta <= M_PI && theta >= M_PI / 3) {
    return M_PI / 2 + (3.0f / 4.0f) * (theta - M_PI / 3);
  }
  return -1337;
}

double convertToORGBangle(double theta) {
  if (theta < M_PI / 2) {
    return (2.0f / 3.0f) * theta;
  } else if (theta <= M_PI && theta >= M_PI / 2) {
    return M_PI / 3 + (4.0f / 3.0f) * (theta - M_PI / 2);
  }
  return -1337;
}

Mat3f rgb2lcc(Mat3f src) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
    transform_mat << 0.2990f, 0.5870f, 0.1140f,
                     0.5000f, 0.5000f, -1.0000f,
                     0.8660f, -0.8660f, 0.0000f;
  // clang-format on
  return src * transform_mat;
}

Mat3f lcc2rgb(Mat3f lcc) {
  Eigen::Matrix3f transform_mat;
  // clang-format off
    transform_mat << 1.0000f, 0.1140f, 0.7436f,
                     1.0000f, 0.1140f, -0.4111f,
                     1.0000f, -0.8660f, 0.1663f;
  // clang-format on
  return lcc * transform_mat;
}

Mat3f lcc2orgb(Mat3f lcc) {
  Mat3f orgb(lcc.rows(), 3);

  for (int i = 0; i < lcc.rows(); ++i) {
    double L = lcc(i, 0);
    double C1 = lcc(i, 1);
    double C2 = lcc(i, 2);

    double theta = std::atan2(C2, C1);
    double orgb_theta =
        (theta > 0.0f) ? convertToRGBangle(theta) : -convertToRGBangle(-theta);
    double angle = orgb_theta - theta;

    Eigen::Matrix2f rotation_matrix;
    // clang-format off
    rotation_matrix << std::cos(angle), -std::sin(angle),
                       std::sin(angle), std::cos(angle);
    // clang-format on
    Eigen::Vector2f C1C2(C1, C2);
    Eigen::Vector2f CybCrg = rotation_matrix * C1C2;
    orgb(i, 0) = L;
    orgb(i, 1) = CybCrg(0);
    orgb(i, 2) = CybCrg(1);
  }
  return orgb;
}

Mat3f orgb2lcc(const Mat3f& orgb) {
  Mat3f lcc(orgb.rows(), 3);

  for (int i = 0; i < orgb.rows(); ++i) {
    double L = orgb(i, 0);
    double Cyb = orgb(i, 1);
    double Crg = orgb(i, 2);

    double theta = std::atan2(Crg, Cyb);
    double rgb_theta = (theta > 0.0f) ? convertToORGBangle(theta)
                                      : -convertToORGBangle(-theta);
    double angle = rgb_theta - theta;

    Eigen::Matrix2f rotation_matrix;
    // clang-format off
    rotation_matrix << std::cos(angle), -std::sin(angle),
                       std::sin(angle), std::cos(angle);
    // clang-format on
    Eigen::Vector2f CybCrg(Cyb, Crg);
    Eigen::Vector2f C1C2 = rotation_matrix * CybCrg;

    lcc(i, 0) = L;
    lcc(i, 1) = C1C2(0);
    lcc(i, 2) = C1C2(1);
  }
  return lcc;
}

}  // namespace psm

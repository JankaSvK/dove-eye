#include "dove_eye/camera_calibration.h"

#include <cassert>
#include <vector>

#include "dove_eye/logging.h"

using cv::calibrateCamera;
using cv::Point3f;
using cv::stereoCalibrate;
using std::vector;

namespace dove_eye {

CameraCalibration::CameraCalibration(const Parameters &parameters,
                                     const CameraIndex arity,
                                     CalibrationPattern const *pattern)
    : parameters_(parameters),
      arity_(arity),
      pattern_(pattern),
      data_(arity),
      pairs_(CameraPair::GenerateArray(arity)) {
  Reset();
}

bool CameraCalibration::MeasureFrameset(const Frameset &frameset) {
  assert(frameset.Arity() == arity_);

  if (frame_no_++ % (frames_skip_ + 1) != 0) {
    return false;
  }

  bool result = true;
  /*
   * First we search for pattern in each single camera,
   * when both camera from a pair are calibrated, we estimate pair parameters.
   */
  for (CameraIndex cam = 0; cam < arity_; ++cam) {
    if (!frameset.IsValid(cam)) {
      result = result && (camera_states_[cam] == kReady);
      continue;
    }

    Point2Vector image_points;

    switch (camera_states_[cam]) {
      case kUnitialized:
      case kCollecting:
        if (pattern_->Match(frameset[cam].data, &image_points)) {
          image_points_[cam].push_back(image_points);
          camera_states_[cam] = kCollecting;
        }

        if (image_points_[cam].size() >= frames_to_collect_) {
          vector<Point3Vector> object_points(image_points_[cam].size(),
              pattern_->ObjectPoints());

          auto error = calibrateCamera(object_points, image_points_[cam],
              frameset[cam].data.size(),
              data_.camera_parameters_[cam].camera_matrix,
              data_.camera_parameters_[cam].distortion_coefficients,
              cv::noArray(), cv::noArray());
          DEBUG("Camera %i calibrated, reprojection error %f", cam, error);

          camera_states_[cam] = kReady;
          image_points_[cam].clear(); /* Not needed anymore */
        }

        result = false;
        break;
      case kReady:
        /* nothing to do here */
        break;
      default:
        assert(false);
    }
  }

  for (auto pair : pairs_) {
    auto cam1 = pair.cam1;
    auto cam2 = pair.cam2;

    if (camera_states_[cam1] != kReady || camera_states_[cam2] != kReady) {
      result = result && (pair_states_[pair.index] == kReady);
      continue;
    }
    if (!frameset.IsValid(cam1) || !frameset.IsValid(cam2)) {
      result = result && (pair_states_[pair.index] == kReady);
      continue;
    }

    Point2Vector image_points1, image_points2;
    cv::Mat dummy_R, dummy_T;
    size_t collected;

    switch (pair_states_[pair.index]) {
      case kUnitialized:
      case kCollecting:
        if (pattern_->Match(frameset[cam1].data, &image_points1) &&
            pattern_->Match(frameset[cam2].data, &image_points2)) {
          image_points_pair_[pair.index].first.push_back(image_points1);
          image_points_pair_[pair.index].second.push_back(image_points2);

          pair_states_[pair.index] = kCollecting;
        }

        /* We add points in lockstep, this checking only first of pair */
        collected = image_points_pair_[pair.index].first.size();
        if (collected >= frames_to_collect_) {
          DEBUG("Calibrating pair %i, %i", cam1, cam2);
          vector<Point3Vector> object_points(
              collected,
              pattern_->ObjectPoints());

          // FIXME Getting size more centrally probably.
          auto error = stereoCalibrate(object_points,
              image_points_pair_[pair.index].first,
              image_points_pair_[pair.index].second,
              data_.camera_parameters_[cam1].camera_matrix,
              data_.camera_parameters_[cam1].distortion_coefficients,
              data_.camera_parameters_[cam2].camera_matrix,
              data_.camera_parameters_[cam2].distortion_coefficients,
              cv::Size(1, 1), /* not actually used as we already know camera matrix */
              data_.pair_parameters_[pair.index].rotation,
              data_.pair_parameters_[pair.index].translation,
              cv::noArray(), /* E */
              data_.pair_parameters_[pair.index].fundamental_matrix); /* F */

          DEBUG("Pair %i, %i calibrated, reprojection error %f", cam1, cam2,
                error);

          pair_states_[pair.index] = kReady;
          image_points_pair_[pair.index].first.clear();
          image_points_pair_[pair.index].second.clear();
        }

        result = false;
        break;
      case kReady:
        /* nothing to do here */
        break;
      default:
        assert(false);
    }
  }

  return result;
}

void CameraCalibration::Reset() {
  frames_to_collect_ = parameters_.Get(Parameters::CALIBRATION_FRAMES);
  frames_skip_ = parameters_.Get(Parameters::CALIBRATION_SKIP);
  frame_no_ = 0;

  image_points_ = decltype(image_points_)(arity_);
  image_points_pair_ = decltype(image_points_pair_)(CameraPair::Pairity(arity_));
  camera_states_ = decltype(camera_states_)(arity_, kUnitialized);
  pair_states_ = decltype(pair_states_)(CameraPair::Pairity(arity_), kUnitialized);
}

double CameraCalibration::CameraProgress(const CameraIndex cam) const {
  assert(cam < Arity());

  switch (camera_states_[cam]) {
    case kUnitialized:
      return 0;
    case kCollecting:
      return
          image_points_[cam].size() / static_cast<double>(frames_to_collect_);
    case kReady:
      return 1;
  }

  assert(false); return 0;
}

double CameraCalibration::PairProgress(const CameraIndex index) const {
  assert(index < CameraPair::Pairity(Arity()));

  switch (pair_states_[index]) {
    case kUnitialized:
      return 0;
    case kCollecting: {
      auto collected = image_points_pair_[index].first.size();
      return
          collected / static_cast<double>(frames_to_collect_);
    }
    case kReady:
      return 1;
  }

  assert(false); return 0;
}

} // namespace dove_eye

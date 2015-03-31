#include "dove_eye/tracker.h"

#include <utility>

#include <opencv2/opencv.hpp>

namespace dove_eye {

Tracker::Tracker(const CameraIndex arity, const InnerTracker &inner_tracker)
    : arity(arity),
      trackpoints_(arity),
      trackstates_(arity, kUninitialized),
      trackers_(arity) {
  for (CameraIndex cam = 0; cam < arity; ++cam) {
    trackers_[cam] = std::move(InnerTrackerPtr(inner_tracker.Clone()));
  }
}


void Tracker::SetMark(const CameraIndex cam, const InnerTracker::Mark mark,
                      bool project_other) {
  assert(cam < arity);
  // TODO implement projection
  assert(project_other == false);

  trackpoints_[cam] = mark;
  trackpoints_.SetValid(cam, false);

  trackstates_[cam] = kMarkSet;
}

Positset Tracker::Track(const Frameset &frameset) {
  assert(frameset.Arity() == arity);

  for (CameraIndex cam = 0; cam < arity; ++cam) {
    TrackSingle(cam, frameset[cam]);
  }

  return trackpoints_;
}

void Tracker::TrackSingle(const CameraIndex cam, const Frame &frame) {
  switch (trackstates_[cam]) {
    case kUninitialized:
      return;

    case kMarkSet:
      if (trackers_[cam]->InitializeTracking(frame, trackpoints_[cam],
                                            &trackpoints_[cam])) {
        trackstates_[cam] = kTracking;
        trackpoints_.SetValid(cam, true);
      } else {
        /*
         * If initialization failed, do not change state
         * and try it next time.
         */
        trackpoints_.SetValid(cam, false);
      }
      break;

    case kTracking:
      if (!trackers_[cam]->Track(frame, &trackpoints_[cam])) {
        trackstates_[cam] = kLost;
        trackpoints_.SetValid(cam, false);
      }
      break;
    case kLost:
      // TODO re-initialize tracking
      //    e.g. with knowledge from other cameras
      //         or projection...
      break;
  }
}

} // namespace dove_eye
#ifndef DOVE_EYE_TRACKER_H_
#define DOVE_EYE_TRACKER_H_

#include <memory>
#include <vector>

#include "dove_eye/frame.h"
#include "dove_eye/frameset.h"
#include "dove_eye/inner_tracker.h"
#include "dove_eye/positset.h"

namespace dove_eye {

/**
 * @note This class is not (intentionaly) thread safe.
 */
class Tracker {
 public:
  explicit Tracker(const CameraIndex width, const InnerTracker &inner_tracker);

  void SetMark(const CameraIndex cam, const InnerTracker::Mark mark,
               bool project_other = false);

  Positset Track(const Frameset &frameset);

 private:
  enum TrackState {
    UNINITIALIZED,
    MARK_SET,
    TRACKING,
    LOST
  };

  typedef std::vector<TrackState> StateVector;
  typedef std::unique_ptr<InnerTracker> InnerTrackerPtr;
  typedef std::vector<InnerTrackerPtr> TrackerVector;

  const CameraIndex width_;
  
  Positset trackpoints_;

  StateVector trackstates_;

  TrackerVector trackers_;

  void TrackSingle(const CameraIndex cam, const Frame &frame);
};

} // namespace dove_eye

#endif // DOVE_EYE_TRACKER_H_


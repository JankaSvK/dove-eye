#include "dove_eye/camera_pair.h"

#include <vector>

#include <dove_eye/calibration_pattern.h>
#include <dove_eye/frameset.h>

namespace dove_eye {

CameraPair::PairArray CameraPair::GenerateArray(const CameraIndex camera_count) {
  const CameraIndex pair_count = camera_count * (camera_count - 1) / 2;
  PairArray result(pair_count);

  int index = 0;
  for (int cam1 = 0; cam1 < camera_count; ++cam1) {
    for (int cam2 = cam1 + 1; cam2 < camera_count; ++cam2) {
      result[index].index = index;
      result[index].cam1 = cam1;
      result[index].cam2 = cam2;
    }
  }
  
  return result;
}

} // namespace dove_eye


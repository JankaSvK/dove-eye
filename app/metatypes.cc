#include "metatypes.h"

#include <QMetaObject>

#include "controller.h"
#include "dove_eye/calibration_data.h"
#include "dove_eye/frameset.h"
#include "dove_eye/localization.h"
#include "dove_eye/types.h"
#include "frameset_converter.h"
#include "gui/gui_mark.h"

void RegisterMetaTypes() {
  qRegisterMetaType<dove_eye::CalibrationData>();

  qRegisterMetaType<dove_eye::CameraIndex>();
  /* Metatypes probably don't work with typedefs so here's a workaround */
  qRegisterMetaType<dove_eye::CameraIndex>("dove_eye::CameraIndex");

  qRegisterMetaType<FramesetConverter::ImageList>();
  qRegisterMetaType<FramesetConverter::ImageList>("ImageList");

  qRegisterMetaType<dove_eye::Frameset>();
  qRegisterMetaType<gui::GuiMark>();
  qRegisterMetaType<gui::GuiMark>("GuiMark");

  qRegisterMetaType<dove_eye::Location>();
  qRegisterMetaType<Controller::Mode>();
}


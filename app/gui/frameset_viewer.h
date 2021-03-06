#ifndef GUI_FRAMESET_VIEWER_H_
#define GUI_FRAMESET_VIEWER_H_

#include <QLayout>
#include <QResizeEvent>
#include <QVector>
#include <QWidget>

#include "dove_eye/types.h"
#include "frame_viewer.h"
#include "frameset_converter.h"

class QLayout;

namespace gui {

/*
 * \see http://stackoverflow.com/a/21253353/1351874
 */

class FramesetViewer : public QWidget {
  Q_OBJECT

 public:
  explicit FramesetViewer(QWidget *parent = nullptr)
      : QWidget(parent),
        arity_(0),
        viewers_(0),
        converter_(nullptr) {
  }

  void SetArity(const dove_eye::CameraIndex arity);

  void SetConverter(FramesetConverter *converter);

 public slots:
  void SetImageset(const FramesetConverter::ImageList &image_list);

  void SetPositset(const dove_eye::Positset positset);

 private:
  dove_eye::CameraIndex arity_;
  QVector<FrameViewer *> viewers_;
  FramesetConverter *converter_;
};

} // namespace gui

#endif // GUI_FRAMESET_VIEWER_H_

#include "io/localization_data_storage.h"


#include <QtDebug>

#include <opencv2/opencv.hpp>
#include "widgets/scene_viewer.h"
#include "dove_eye/location.h"

using cv::FileStorage;
using widgets::SceneViewer;

namespace io {

void LocalizationDataStorage::SaveToFile(const QString &filename) {
    FileStorage fs(filename.toStdString(), FileStorage::WRITE);

    assert(fs.isOpened());

    for(dove_eye::Location location : scene_viewer->getLocalizationData()){
        fs << "point";
        fs << "{" << "x" << location.x;
        fs <<        "y" << location.y;
        fs <<        "z" << location.z << "}";
    }
}

} // end namespace io


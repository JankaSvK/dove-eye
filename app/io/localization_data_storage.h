#ifndef IO_LOCALIZATION_DATA_STORAGE_H_
#define IO_LOCALIZATION_DATA_STORAGE_H_

#include <QObject>
#include <QString>

//#include "dove_eye/parameters.h"
#include "widgets/scene_viewer.h"

namespace io {

class LocalizationDataStorage : public QObject {
    Q_OBJECT;

public:
    /*
      explicit ParametersStorage(dove_eye::Parameters & parameters,
                             QObject *parent = nullptr)
      : QObject(parent),
        parameters_(parameters) {
      }
    */

    LocalizationDataStorage(widgets::SceneViewer* sciene_viewer){
        this->scene_viewer = sciene_viewer;
    }

public slots:
    void SaveToFile(const QString &filename);

private:
    widgets::SceneViewer* scene_viewer;
}; // end namespace io

}
#endif // IO_LOCALIZATION_DATA_STORAGE_H_

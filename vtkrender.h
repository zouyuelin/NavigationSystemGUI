#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkDICOMImageReader.h>
#include <vtkBMPReader.h>
#include <vtkMarchingCubes.h>
#include <vtkDecimatePro.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkJPEGReader.h>
#include <vtkCamera.h>
#include <vtkStripper.h>
#include <vtkProperty.h>
#include <vtkStdString.h>
#include <vtkStringArray.h>
#include <vtkParticleReader.h>
#include <vtkAxesActor.h>
#include <QDebug>
#include <QString>
#include <chrono>
#include "QVTKWidget.h"
#include "mainwindow.h"

class vtkRender
{
public:
    void vtkplot(QVTKWidget *vtkwidget);
};

#endif // COMMON_H

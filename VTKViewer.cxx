// VTK Viewer
// Written 2012-2013 Hal Canary <http://cs.unc.edu/~hal>
// Copyright 2012-2013 University of North Carolina at Chapel Hill.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you
// may not use this file except in compliance with the License.  You
// may obtain a copy of the License at
//
//   LICENSE.md in this repository or
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied.  See the License for the specific language governing
// permissions and limitations under the License.

#include "VTKViewer.h"
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPLYReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkColorTransferFunction.h>
#include <vtkPolyDataNormals.h>
#include <vtkPointData.h>
#include <vtkVersion.h>
#if VTK_MAJOR_VERSION <= 5
  #define setInputData(x,y) ((x)->SetInput(y))
#else
  #define setInputData(x,y) ((x)->SetInputData(y))
#endif
#include <vtkCamera.h>

#include <iostream>
using std::cout;

VTKViewer::VTKViewer() :
  renderer(vtkSmartPointer < vtkRenderer >::New())
{
  vtkSmartPointer < vtkRenderWindow > renderWindow =
    vtkSmartPointer < vtkRenderWindow >::New();
  renderWindow->StereoCapableWindowOn();
  renderWindow->SetStereoTypeToRedBlue();
  renderWindow->AddRenderer(renderer);

  renderer->ResetCamera();
  this->resize(480, 480);
  this->setMinimumSize(480, 360);
  this->SetRenderWindow(renderWindow);
  connect(&(this->timer), SIGNAL(timeout()), this, SLOT(rotate()));
  this->timer.start(66);
}
void VTKViewer::add(vtkPolyData * polyData)
{
  double range[2];
  polyData->GetScalarRange(range);
  vtkSmartPointer< vtkColorTransferFunction > colorMap
    = vtkSmartPointer< vtkColorTransferFunction >::New();
  colorMap->SetColorSpaceToLab();
  colorMap->AddRGBPoint(range[0], 0.865, 0.865, 0.865);
  colorMap->AddRGBPoint(range[1], 0.706, 0.016, 0.150);
  colorMap->Build();

  vtkSmartPointer < vtkPolyDataMapper > mapper =
    vtkSmartPointer < vtkPolyDataMapper >::New();
  mapper->SetLookupTable(colorMap);
  if (polyData->GetPointData()->GetNormals() == NULL)
    {
    vtkSmartPointer< vtkPolyDataNormals > polyDataNormals
      = vtkSmartPointer< vtkPolyDataNormals >::New();
    setInputData(polyDataNormals, polyData);
    polyDataNormals->SetFeatureAngle(90.0);
    mapper->SetInputConnection(polyDataNormals->GetOutputPort());
    }
  else
    {
    setInputData(mapper, polyData);
    }
  vtkSmartPointer < vtkActor > actor =
    vtkSmartPointer < vtkActor >::New();
  actor->GetProperty()->SetPointSize(3);
  actor->SetMapper(mapper);
  this->renderer->AddActor(actor);
}
void VTKViewer::add(const char * file_name)
{
  // TODO:  add logic for other file formats.
  vtkSmartPointer < vtkPolyData > polyData =
    vtkSmartPointer< vtkPolyData >::New();
  QString filename = QString::fromUtf8(file_name);
  if (filename.endsWith(".vtp") || filename.endsWith(".VTP"))
    {
    vtkSmartPointer< vtkXMLPolyDataReader > reader =
      vtkSmartPointer< vtkXMLPolyDataReader >::New();
    reader->SetFileName(file_name);
    reader->Update();
    polyData->ShallowCopy(reader->GetOutput());
    }
  else if (filename.endsWith(".vtk") || filename.endsWith(".VTK"))
    {
    vtkSmartPointer< vtkPolyDataReader > reader =
      vtkSmartPointer< vtkPolyDataReader >::New();
    reader->SetFileName(file_name);
    reader->Update();
    polyData->ShallowCopy(reader->GetOutput());
    }
  else if (filename.endsWith(".ply") || filename.endsWith(".PLY"))
    {
    vtkSmartPointer< vtkPLYReader > reader =
      vtkSmartPointer< vtkPLYReader >::New();
    reader->SetFileName(file_name);
    reader->Update();
    polyData->ShallowCopy(reader->GetOutput());
    }
  else
    {
    assert("BAD FILE NAME.  Should end in VTK, VTP, or PLY." && 0);
    return;
    }
  this->add(polyData);
}

void VTKViewer::toggle()
{
  if (this->timer.isActive())
    this->timer.stop();
  else
    this->timer.start(33);
}

void VTKViewer::rotate()
{
  vtkCamera * camera = this->renderer->GetActiveCamera();
  assert(camera != NULL);
  camera->Azimuth(1);
  this->renderer->GetRenderWindow()->Render();
}

void VTKViewer::stereo()
{
  vtkRenderWindow * rw = this->renderer->GetRenderWindow();
  assert(rw != NULL);
  rw->SetStereoRender(! rw->GetStereoRender());
  rw->Render();
}

void VTKViewer::changeStereoType()
{
  vtkRenderWindow * rw = this->renderer->GetRenderWindow();
  assert(rw != NULL);
  int type = rw->GetStereoType();
  type = (type % 9) + 1;
  rw->SetStereoType(type);
  switch(type)
    {
    case 1: cout << "VTK_STEREO_CRYSTAL_EYES\n"; break;
    case 2: cout << "VTK_STEREO_RED_BLUE\n"; break;
    case 3: cout << "VTK_STEREO_INTERLACED\n"; break;
    case 4: cout << "VTK_STEREO_LEFT\n"; break;
    case 5: cout << "VTK_STEREO_RIGHT\n"; break;
    case 6: cout << "VTK_STEREO_DRESDEN\n"; break;
    case 7: cout << "VTK_STEREO_ANAGLYPH\n"; break;
    case 8: cout << "VTK_STEREO_CHECKERBOARD\n"; break;
    case 9: cout << "VTK_STEREO_SPLITVIEWPORT_HORIZONTAL\n"; break;
    }
  rw->Render();
}

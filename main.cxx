// VTK Viewer
// Written 2012 Hal Canary <http://cs.unc.edu/~hal>
// Copyright 2012 University of North Carolina at Chapel Hill.
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

#include <QObject>
#include <QString>
#include <QApplication>
#include <QMainWindow>
#include <QAction>
#include <QShortcut>

#include "VTKViewer.h"

int main(int argc, char ** argv) {
  if (argc == 1)
    {
    std::cerr
      << "\nUseage:  \n  " << argv[0]
      << " FILE [MORE FILES...]\n"
      "Supported File Formats:\n"
      "  *.vtk - VTK Legacy File\n"
      "  *.vtp - VTK Polygonal Data File\n"
      "  *.ply - Stanford Polygon File\n"
      "  *.obj - Wavefront Object file\n"
      "  *.stl - Stereolithography File\n"
      "  *.pdb - Protein Data Bank File\n"
      "  *.vtu - VTK Unstructured Grid Data File\n"
      "Controls:\n"
      "  's' - surface\n"
      "  'w' - wireframe\n"
      "  'r' - reset and center camera\n"
      "  'ctrl-q' - quit\n"
      "  'ctrl-r' - toggle rotation\n"
      "  'ctrl-s' - toggle stereo mode\n"
      "  'ctrl-t' - change stereo type\n\n";
    return 1;
    }
  
  QApplication app(argc, argv);
  QMainWindow mw;
	mw.setWindowTitle("VTK Viewer");
  VTKViewer v;

  QShortcut q_shortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), &mw);
  QObject::connect(&q_shortcut, SIGNAL(activated()), &mw, SLOT(close()));

  QShortcut r_shortcut(QKeySequence(Qt::CTRL + Qt::Key_R), &mw);
  QObject::connect(&r_shortcut, SIGNAL(activated()), &v, SLOT(toggle()));

  QShortcut s_shortcut(QKeySequence(Qt::CTRL + Qt::Key_S), &mw);
  QObject::connect(&s_shortcut, SIGNAL(activated()), &v, SLOT(stereo()));

  QShortcut t_shortcut(QKeySequence(Qt::CTRL + Qt::Key_T), &mw);
  QObject::connect(&t_shortcut, SIGNAL(activated()), &v, SLOT(changeStereoType()));

  mw.setCentralWidget(&v);
  for (int i = 1; i < argc; ++i)
    {
    v.add(argv[i]);
    }
  mw.showMaximized();
  return app.exec();
}

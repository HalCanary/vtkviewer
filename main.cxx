// VTK Viewer
// Written 2012 Hal Canary <http://cs.unc.edu/~hal>
// Copyright 2012 University of North Carolina at Chapel Hill.
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you
// may not use this file except in compliance with the License.  You
// may obtain a copy of the License at
// 
//   LICENSE.txt in this repository or
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
  QApplication app(argc, argv);
  QMainWindow mw;
	mw.setWindowTitle("VTK Viewer");
  VTKViewer v;

  QShortcut q_shortcut(QKeySequence(Qt::Key_Q), &mw);
  QObject::connect(&q_shortcut, SIGNAL(activated()), &mw, SLOT(close()));

  QShortcut r_shortcut(QKeySequence(Qt::Key_R), &mw);
  QObject::connect(&r_shortcut, SIGNAL(activated()), &v, SLOT(toggle()));

  QShortcut s_shortcut(QKeySequence(Qt::Key_S), &mw);
  QObject::connect(&s_shortcut, SIGNAL(activated()), &v, SLOT(stereo()));

  QShortcut t_shortcut(QKeySequence(Qt::Key_T), &mw);
  QObject::connect(&t_shortcut, SIGNAL(activated()), &v, SLOT(changeStereoType()));

  mw.setCentralWidget(&v);
  for (int i = 1; i < argc; ++i)
    {
    v.add(argv[i]);
    }
  mw.showMaximized();
  return app.exec();
}

VTK Viewer
==========

![VTK Viewer screenshot](http://cs.unc.edu/~hal/images/vtkviewer_bunny.png)

Written 2012-2013 Hal Canary <[http://cs.unc.edu/~hal](http://cs.unc.edu/~hal/)>. Copyright 2012-2013 University of North Carolina at Chapel Hill.

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.  You may obtain a copy of the License at

*   [LICENSE.md in this repository](LICENSE.md) or
*   [http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the License for the specific language governing permissions and limitations under the License.


Supported File Formats
----------------------

*	VTK — VTK Legacy File
*	VTP — VTK New-Style File
*	PLY — Stanford Polygon File
*	OBJ — Wavefront Object file
*	STL — Stereolithography File
*	PDB — Protein Data Bank File

Required Packages:
------------------

*	libvtk5.8-qt4
*	libvtk5.8
*	libqtgui4
*	cmake

Example installation instructions:
----------------------------------

Install packages:

    $ sudo apt-get install -y libvtk5.8-qt4 libvtk5.8 libqtgui4 cmake git

or

    $ sudo yum install -y vtk-devel vtk-qt qt-devel cmake git

Download:

    $ git clone git://github.com/HalCanary/vtkviewer.git

Build:

    $ vtkviewer/posix_build.sh

Get some data to view:

    $ wget http://bit.ly/15vy2Rf -O bun_zipper.ply

Run program:

    $ vtkviewer/build/vtkviewer bun_zipper.ply

Controls:
---------

*	's' — surface
*	'w' — wireframe
*	'r' — reset and center camera
*	'ctrl-q' — quit
*	'ctrl-r' — toggle rotation
*	'ctrl-s' — toggle stereo mode
*	'ctrl-t' — change stereo type

VTK Viewer
==========

Written 2012-2013 Hal Canary <http://cs.unc.edu/~hal>  
Copyright 2012-2013 University of North Carolina at Chapel Hill.

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.  You may obtain a copy of the License at

*   [LICENSE.md in this repository](LICENSE.md) or
*   [http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the License for the specific language governing permissions and limitations under the License.


Required Packages:
------------------

*	libvtk5.8-qt4
*	libvtk5.8
*	libqtgui4
*	cmake

Example installation instructions:
----------------------------------

    $ sudo apt-get install -y libvtk5.8-qt4 libvtk5.8 libqtgui4 cmake
    $ git clone git://github.com/HalCanary/vtkviewer.git
    $ vtkviewer/posix_build.sh
    $ wget http://www.itk.org/Wiki/images/7/72/Bunny.vtp
    $ vtkviewer/build/vtkviewer Bunny.vtp

Controls:
---------

*	'q' — quit
*	'r' — toggle rotation
*	's' — toggle stereo mode
*	't' — change stereo type


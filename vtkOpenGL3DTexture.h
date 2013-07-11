/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkOpenGL3DTexture.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkOpenGLTexture - OpenGL texture map
// .SECTION Description
// vtkOpenGLTexture is a concrete implementation of the abstract class 
// vtkTexture. vtkOpenGLTexture interfaces to the OpenGL rendering library.

#ifndef __vtkOpenGL3DTexture_h
#define __vtkOpenGL3DTexture_h

#include <math.h>

#include "vtkSmartPointer.h"
#include "vtkObject.h"
#include "vtkTexture.h"
#include "vtkgl.h"


class vtkFloatArray;
class vtkImageData;
class vtkOpenGLRenderer;
class vtkImageConstantPad;
class vtkRenderWindow;
class vtkWindow;


class vtkOpenGL3DTexture : public vtkTexture
{
public:
  static vtkOpenGL3DTexture *New();
  vtkTypeMacro(vtkOpenGL3DTexture,vtkTexture);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Implement base class method.
  void Load(vtkRenderer *ren);

  // Description:
  // Restores OpenGL state after rendering
  void PostRender(vtkRenderer *ren);
  
  // Description:
  // Release any graphics resources that are being consumed by this texture.
  // The parameter window could be used to determine which graphic
  // resources to release. Using the same texture object in multiple
  // render windows is NOT currently supported. 
  void ReleaseGraphicsResources(vtkWindow *);
  
  // Description:
  // Get the openGL texture name to which this texture is bound.
  // This is available only if GL version >= 1.1
  vtkGetMacro(Index, long);

  // Description:
  // Get the maximum normalized texture coordinates used by the
  // texture. This is important for proper generation of texture
  // coordinates for 3D textures that have non-power-of-two dimensions
  // and which must therefore be padded to power-of-two dimensions.
  vtkGetVector3Macro(MaxTextureCoordinates, double);

protected:
  vtkOpenGL3DTexture();
  ~vtkOpenGL3DTexture();

  // Description:
  // Gets 1D array of floats from the vtkImageData passed to it.
  // Used for handling vtkImages with arbitrary vtkDataArray data.
  vtkFloatArray* GetFloatArray(vtkImageData *image, int component);

  vtkTimeStamp LoadTime;

  long Index;
  
  vtkRenderWindow *RenderWindow;   // RenderWindow used for previous render

  double MaxTextureCoordinates[3];

  // Description:
  // Loads extensions required by this class.
  void LoadExtensions(vtkRenderWindow *renWin);

private:
  vtkOpenGL3DTexture(const vtkOpenGL3DTexture&);  // Not implemented.
  void operator=(const vtkOpenGL3DTexture&);  // Not implemented.
  
  // Description:
  // Tracks whether necessary extensions have been loaded.
  int ExtensionsLoaded;

  // Description:
  // Image padder
  vtkSmartPointer<vtkImageConstantPad> ImagePadder;

};

#endif

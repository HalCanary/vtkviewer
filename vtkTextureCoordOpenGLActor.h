/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkTextureCoordOpenGLActor.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkTextureCoordOpenGLActor - generates texture coords with a shader
// .SECTION Description
// OpenGL actor that generates texture coordinates from a
// transformation on the point positions in the dataset

#ifndef __vtkTextureCoordOpenGLActor_h
#define __vtkTextureCoordOpenGLActor_h

#include "vtkOpenGLActor.h"

#include "vtkSmartPointer.h"

class vtkOpenGLRenderer;
class vtkOpenGLRenderWindow;
class vtkRenderWindow;
class vtkShader2;
class vtkShaderProgram2;

class vtkTextureCoordOpenGLActor : public vtkOpenGLActor
{
protected:

public:
  static vtkTextureCoordOpenGLActor *New();
  vtkTypeMacro(vtkTextureCoordOpenGLActor,vtkOpenGLActor);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Uniform scale factor applied to texture coordinates
  vtkSetMacro(TextureScale, float);
  vtkGetMacro(TextureScale, float);

  // Description:
  // Actual actor render method.
  void Render(vtkRenderer *ren, vtkMapper *mapper);

protected:
  vtkTextureCoordOpenGLActor();
  ~vtkTextureCoordOpenGLActor();

private:
  vtkTextureCoordOpenGLActor(const vtkTextureCoordOpenGLActor&);  // Not implemented.
  void operator=(const vtkTextureCoordOpenGLActor&);  // Not implemented.

  float TextureScale;

  int Initialize();

  int Cleanup();

  int IsInitialized;

  vtkOpenGLRenderWindow * RenderWindow;

  vtkSmartPointer< vtkShader2 > VertexShader;

  vtkSmartPointer< vtkShader2 > FragmentShader;

  vtkSmartPointer< vtkShaderProgram2 > Program;
};

#endif

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkOpenGL3DTexture.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkOpenGL3DTexture.h"

#include "vtkFloatArray.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkImageConstantPad.h"
#include "vtkLookupTable.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLExtensionManager.h"
#include "vtkOpenGLRenderer.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkOpenGL.h"
#include "vtkPointData.h"
#include "vtkRenderWindow.h"
#include "vtkUnsignedShortArray.h"


#ifndef VTK_IMPLEMENT_MESA_CXX
vtkStandardNewMacro(vtkOpenGL3DTexture);
#endif

#include <vtkVersion.h>
#if VTK_MAJOR_VERSION <= 5
  #define setInputData(x,y) ((x)->SetInput(y))
  #define addInputData(x,y) ((x)->AddInput(y))
#else
  #define setInputData(x,y) ((x)->SetInputData(y))
  #define addInputData(x,y) ((x)->AddInputData(y))
#endif

// Initializes an instance, generates a unique index.
vtkOpenGL3DTexture::vtkOpenGL3DTexture() {
  this->Index = 0;
  this->RenderWindow = 0;
  this->ExtensionsLoaded = 0;
  this->ImagePadder = vtkSmartPointer<vtkImageConstantPad>::New();

  for (int i = 0; i < 3; i++)
    this->MaxTextureCoordinates[i] = 1.0;
}

vtkOpenGL3DTexture::~vtkOpenGL3DTexture() {
  this->RenderWindow = NULL;
}

// Release the graphics resources used by this texture.
void vtkOpenGL3DTexture::ReleaseGraphicsResources(vtkWindow *renWin) {
  if (this->Index && renWin) {
    ((vtkRenderWindow *) renWin)->MakeCurrent();
    // free any textures
    if (glIsTexture(this->Index)) {
      GLuint tempIndex;
      tempIndex = this->Index;
      // NOTE: Sun's OpenGL seems to require disabling of texture before delete
      glDisable(vtkgl::TEXTURE_3D);
      glDeleteTextures(1, &tempIndex);
    }
  }
  this->Index = 0;
  this->RenderWindow = NULL;
  this->Modified();
}


void vtkOpenGL3DTexture::LoadExtensions(vtkRenderWindow *renWin) {
  if (this->ExtensionsLoaded)
    return;

  vtkOpenGLExtensionManager *manager = vtkOpenGLExtensionManager::New();
  manager->SetRenderWindow(renWin);

  int versionOK = 0;
  versionOK += manager->ExtensionSupported("GL_VERSION_1_2");
  manager->LoadExtension("GL_VERSION_1_2");
  versionOK += manager->ExtensionSupported("GL_VERSION_1_3");
  manager->LoadExtension("GL_VERSION_1_3");
  versionOK += manager->ExtensionSupported("GL_VERSION_1_4");
  manager->LoadExtension("GL_VERSION_1_4");
  versionOK += manager->ExtensionSupported("GL_VERSION_1_5");
  manager->LoadExtension("GL_VERSION_1_5");
  versionOK += manager->ExtensionSupported("GL_VERSION_2_0");
  manager->LoadExtension("GL_VERSION_2_0");
  versionOK += manager->ExtensionSupported("GL_VERSION_3_3");
  manager->LoadExtension("GL_VERSION_3_3");
  manager->LoadSupportedExtension("GL_ARB_texture_float");
  manager->Delete();

  if (versionOK == 0) {
    vtkErrorMacro(<< "No valid OpenGL version available.");
    return;
  }
  this->ExtensionsLoaded = 1;
}

// Implement base class method.
void vtkOpenGL3DTexture::Load(vtkRenderer *ren) {
  vtkImageData *input = this->GetInput();
  if (!input)
    return;

  // need to reload the texture
  if (this->GetMTime() > this->LoadTime.GetMTime() ||
      input->GetMTime() > this->LoadTime.GetMTime() ||
      (this->GetLookupTable() && this->GetLookupTable()->GetMTime () >
       this->LoadTime.GetMTime()) ||
       ren->GetRenderWindow() != this->RenderWindow) {

    // free any old display lists (from the old context)
    if (this->RenderWindow) {
      this->ReleaseGraphicsResources(this->RenderWindow);
    }
    this->RenderWindow = ren->GetRenderWindow();

    // make the new context current before we mess with opengl
    this->RenderWindow->MakeCurrent();

    this->LoadExtensions(this->RenderWindow);

    // get some info
    int *size = input->GetDimensions();

    // It's safest to stick with power-of-two image dimensions in 3D textures,
    // so we check here and pad the PSF if necessary.
    int newSize[3];
    bool needsResize = false;
    for (int i = 0; i < 3; i++) {
      for (newSize[i]  = 1; newSize[i]  < size[i]; newSize[i]  *= 2);
      if (newSize[i] != size[i])
        needsResize = true;
    }

    if (needsResize) {
      this->ImagePadder->
        SetOutputWholeExtent(0, newSize[0]-1, 0, newSize[1]-1, 0, newSize[2]-1);
      setInputData(this->ImagePadder,this->GetInput());
      this->ImagePadder->UpdateWholeExtent();
      input = this->ImagePadder->GetOutput();

      for (int i = 0; i < 3; i++) {
        this->MaxTextureCoordinates[i] = (double) size[i] / (double) newSize[i];
        size[i] = newSize[i];
      }
    } else {
      for (int i = 0; i < 3; i++) {
        this->MaxTextureCoordinates[i] = 1.0;
      }
    }

    vtkDataArray *scalars = input->GetPointData()->GetScalars();

    // make sure scalars are non null
    if (!scalars) {
      vtkErrorMacro(<< "No scalar values found for texture input!");
      return;
    }

    GLint maxDimGL;
    glGetIntegerv(vtkgl::MAX_3D_TEXTURE_SIZE, &maxDimGL);

    // if larger than permitted by the graphics library then must resample
    if ( size[0] > maxDimGL || size[1] > maxDimGL || size[2] > maxDimGL ) {
      vtkDebugMacro( "Texture too big for gl, maximum is " << maxDimGL);
    }

    // get a unique display list id
    GLuint tempIndex=0;
    glGenTextures(1, &tempIndex);
    this->Index = (long) tempIndex;
    glBindTexture(vtkgl::TEXTURE_3D, this->Index);
    if (glGetError() != GL_NO_ERROR) {
      vtkDebugMacro("glGetError reports error right after binding the 3D texture");
    }

    if(this->BlendingMode != VTK_TEXTURE_BLENDING_MODE_NONE
       && vtkgl::ActiveTexture)
      {
      glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, vtkgl::COMBINE);

      switch(this->BlendingMode)
        {
        case VTK_TEXTURE_BLENDING_MODE_REPLACE:
          {
          glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_RGB, GL_REPLACE);
          glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_ALPHA, GL_REPLACE);
          break;
          }
        case VTK_TEXTURE_BLENDING_MODE_MODULATE:
          {
          glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_RGB, GL_MODULATE);
          glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_ALPHA, GL_MODULATE);
          break;
          }
        case VTK_TEXTURE_BLENDING_MODE_ADD:
          {
          glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_RGB, GL_ADD);
          glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_ALPHA, GL_ADD);
          break;
          }
        case VTK_TEXTURE_BLENDING_MODE_ADD_SIGNED:
          {
          glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_RGB, vtkgl::ADD_SIGNED);
          glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_ALPHA, vtkgl::ADD_SIGNED);
          break;
          }
        case VTK_TEXTURE_BLENDING_MODE_INTERPOLATE:
          {
          glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_RGB, vtkgl::INTERPOLATE);
          glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_ALPHA, vtkgl::INTERPOLATE);
          break;
          }
        case VTK_TEXTURE_BLENDING_MODE_SUBTRACT:
          {
          glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_RGB, vtkgl::SUBTRACT);
          glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_ALPHA, vtkgl::SUBTRACT);
          break;
          }
        default:
          {
          glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_RGB, GL_ADD);
          glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_ALPHA, GL_ADD);
          }
        }
      }


    if (glGetError() != GL_NO_ERROR) {
      vtkDebugMacro("glGetError reports error right calling glTexEnvf");
    }

    ((vtkOpenGLRenderWindow *)(ren->GetRenderWindow()))->RegisterTextureResource( this->Index );

    // Set interpolation mode
    if (this->Interpolate) {
      glTexParameterf( vtkgl::TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameterf( vtkgl::TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
      glTexParameterf( vtkgl::TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glTexParameterf( vtkgl::TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    }

    // Set repeat mode
    if (this->Repeat) {
      glTexParameterf( vtkgl::TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT );
      glTexParameterf( vtkgl::TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT );
      glTexParameterf( vtkgl::TEXTURE_3D, vtkgl::TEXTURE_WRAP_R, GL_REPEAT );
    } else {
      glTexParameterf( vtkgl::TEXTURE_3D, GL_TEXTURE_WRAP_S, vtkgl::CLAMP_TO_EDGE );
      glTexParameterf( vtkgl::TEXTURE_3D, GL_TEXTURE_WRAP_T, vtkgl::CLAMP_TO_EDGE );
      glTexParameterf( vtkgl::TEXTURE_3D, vtkgl::TEXTURE_WRAP_R, vtkgl::CLAMP_TO_EDGE );
    }

    if (glGetError() != GL_NO_ERROR) {
      vtkDebugMacro("glGetError reports error right after setting texture parameters");
    }

    // Convert image data to floats.
    vtkFloatArray *floatArray = this->GetFloatArray(input, 0);
    float *floatPtr = floatArray->GetPointer(0);

    // Store the texture as 16-bit floating point luminance texture.
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    int components = floatArray->GetNumberOfComponents();

    GLint textureFormat;
    GLint textureInternalFormat;
    if (components == 1) {
      textureFormat = GL_LUMINANCE;
    } else if (components == 3) {
      textureFormat = GL_RGB;
    } else if (components == 4) {
      textureFormat = GL_RGBA;
    }

    if (this->Quality == VTK_TEXTURE_QUALITY_16BIT) {
      if (components == 1) {
        textureInternalFormat = vtkgl::LUMINANCE16F_ARB;
      } else if (components == 3) {
        textureInternalFormat = vtkgl::RGB32F_ARB;
      } else if (components == 4) {
        textureInternalFormat = vtkgl::RGBA32F_ARB;
      } else {
        vtkErrorMacro(<< "Unknown TextureInternalFormat");
      }
    } else { // VTK_TEXTURE_QUALITY_32BIT
      if (components == 1) {
        textureInternalFormat = vtkgl::LUMINANCE32F_ARB;
      } else if (components == 3) {
        textureInternalFormat = vtkgl::RGB32F_ARB;
      } else if (components == 4) {
        textureInternalFormat = vtkgl::RGBA32F_ARB;
      }
    }

    vtkgl::TexImage3D(vtkgl::TEXTURE_3D, 0, textureInternalFormat,
                      size[0], size[1], size[2], 0, textureFormat, GL_FLOAT,
                      (const GLvoid *) floatPtr);

    glPopClientAttrib();

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
      vtkDebugMacro(<< "glGetError reports an error after calling glTexImage3D");
    }

    // Clean up the float array.
    floatArray->Delete();
    floatArray = NULL;

    // modify the load time to the current time
    this->LoadTime.Modified();
  }

  // now bind it
  glBindTexture(vtkgl::TEXTURE_3D, this->Index);
  glEnable(vtkgl::TEXTURE_3D);
}


void vtkOpenGL3DTexture::PostRender(vtkRenderer *ren) {
  glBindTexture(vtkgl::TEXTURE_3D, 0);
  glDisable(vtkgl::TEXTURE_3D);
}


vtkFloatArray* vtkOpenGL3DTexture::GetFloatArray(vtkImageData *image, int component) {
  vtkImageCast *cast = vtkImageCast::New();
  cast->SetOutputScalarTypeToFloat();
  setInputData(cast,image);
  vtkImageData *floatData = cast->GetOutput();
  cast->Update();
  vtkFloatArray *array = vtkFloatArray::New();
  array->DeepCopy(floatData->GetPointData()->GetScalars());
  cast->Delete();
  return array;
}

void vtkOpenGL3DTexture::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Index: " << this->Index << endl;
}

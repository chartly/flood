/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#ifdef ENABLE_RENDERER_OPENGL

#include <glew/glew.h>
#ifdef PLATFORM_WINDOWS
    #include <glew/wglew.h>
#endif

#include "Graphics/RenderBatch.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Texture.h"

NAMESPACE_GRAPHICS_BEGIN

//-----------------------------------//

// Handles OpenGL error states.
bool CheckLastErrorGL( const char* msg );

// Gets a string given an OpenGL error code.
//const char* glErrorString(GLenum errorCode);

class VertexBuffer;
void BindFixedVertexBufferDeclarations(VertexBuffer*);
void UnbindFixedVertexBufferDeclarations(VertexBuffer*);
void BindGenericVertexBufferDeclarations(VertexBuffer*);
void UnbindGenericVertexBufferDeclarations(VertexBuffer*);

int ConvertBufferGL(BufferUsage, BufferAccess);
GLenum ConvertPrimitiveGL(PrimitiveType);
GLenum ConvertPrimitiveRasterGL(PrimitiveRasterMode);

int ConvertTextureFilterFormatGL( TextureFilterMode );
int ConvertTextureWrapFormatGL( TextureWrapMode );
GLint ConvertTextureInternalFormatGL( PixelFormat );
GLint ConvertTextureSourceFormatGL( PixelFormat );
GLint ConvertTextureTargetGL( TextureTarget );

GLenum ConvertBlendSourceGL(BlendSource);
GLenum ConvertBlendDestinationGL(BlendDestination);
GLenum ConvertDepthModeGL(DepthCompare);

GLenum ConvertShaderTypeGL( ShaderType );

class RenderBackend;
RenderBackend* RenderCreateBackendGLES2();

NAMESPACE_GRAPHICS_END

//-----------------------------------//

#define glewGetContext() (GetRenderDevice()->getActiveContext()->glewContext)

#endif
/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "../Build/Build.h"
#include "Core/Platform.h"

#ifdef PLATFORM_NACL
#undef ENABLE_NETWORK_ENET
#undef ENABLE_NETWORKING_CURL
#undef ENABLE_HTTP_SERVER
#endif

//---------------------------------------------------------------------//
// Namespaces
//---------------------------------------------------------------------//

#define FLD_NAMESPACE fld

#ifdef FLD_NAMESPACE
#define NAMESPACE_CORE_BEGIN namespace FLD_NAMESPACE {
#define NAMESPACE_CORE_END }
#else
#define NAMESPACE_CORE_BEGIN
#define NAMESPACE_CORE_END
#endif

//---------------------------------------------------------------------//
// API Exports
//---------------------------------------------------------------------//

#if defined(API_CORE_DLL) && defined(API_CORE_DLL_EXPORT)
    #define API_CORE API_EXPORT
#elif defined(API_CORE_DLL)
    #define API_CORE API_IMPORT
#else
    #define API_CORE
#endif

#ifndef SWIG

#ifdef COMPILER_MSVC
// nonstandard extension used: 'identifier' before template explicit instantiation
#pragma warning(disable : 4231)

// nonstandard extension used: enum 'identifier' used in qualified name
#pragma warning(disable : 4482)
#endif

#define FL_INSTANTIATE_TEMPLATES() \
    template class std::basic_string<char>;

//extern template class std::basic_string<char>;

#endif

//---------------------------------------------------------------------//
// Global includes
//---------------------------------------------------------------------//

#include <array>
#include <functional>
#include <miniformat/miniformat.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Core.h"
#include "Core/Memory.h"
#include "Core/HashMap.h"

//---------------------------------------------------------------------//
// Namespace adapters ---> uggghh
//---------------------------------------------------------------------//
namespace fld {
    using glm::vec2;
    using glm::vec3;
    using glm::vec4;
    using glm::mat4;
    using glm::one;
    using glm::zero;
    using glm::quarter_pi;
    using glm::normalize;
    using glm::cross;
    using glm::rotate;
    using glm::length;
    using glm::inverse;
    using glm::rotate;
    using glm::perspective;
    using glm::infinitePerspective;
    using glm::scale;
    using glm::smoothstep;
}
/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

//---------------------------------------------------------------------//
// Base API
//---------------------------------------------------------------------//
#include "Core/API.h"
#include "Core/RefPtr.h"

//---------------------------------------------------------------------//
// Framework includes
//---------------------------------------------------------------------//
#include <glew/glew.h>
#include <glew/wglew.h>
#include <glfw/glfw3.h>
#include <AntTweakBar/AntTweakBar.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <miniformat/miniformat.h>

#include <array>
#include <functional>

//---------------------------------------------------------------------//
// Namespace adapters ---> uggghh
//---------------------------------------------------------------------//
namespace dit {
    using namespace fld;

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
}
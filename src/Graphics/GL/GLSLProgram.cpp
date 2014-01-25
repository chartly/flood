/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "Graphics/API.h"
#include "Graphics/GL/GLSLProgram.h"
#include "Graphics/GL/GLApp.h"
#include "Engine/Resources/Text.h"
#include "Core/Log.h"
#include "Core/Math/Hash.h"

#include <fstream>

namespace dit
{
    //-----------------------------------//

    template<>
    void GLSLUniform::set(const mat4& m)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, &m[0][0]);
    }

    template<>
    void GLSLUniform::set(const Array<mat4>& ms)
    {
        glUniformMatrix4fv(location, ms.size(), GL_FALSE, &(ms.front())[0][0]);
    }

    template<>
    void GLSLUniform::set(const vec4& v)
    {
        glUniform4fv(location, 1, &v[0]);
    }

    template<>
    void GLSLUniform::set(const Array<vec4>& vs)
    {
        glUniform4fv(location, vs.size(), &(vs.front())[0]);
    }

    template<>
    void GLSLUniform::set(const vec3& v)
    {
        glUniform3fv(location, 1, &v[0]);
    }

    template<>
    void GLSLUniform::set(const Array<vec3>& vs)
    {
        glUniform3fv(location, vs.size(), &(vs.front())[0]);
    }

    //-----------------------------------//

    bool GLSLShader::create(GLenum shaderType)
    {
        id = glCreateShader(shaderType);
        if(id == 0)
        {
            LogError("Failed to created a glsl shader.");
            return false;
        }

        return true;
    }

    bool GLSLShader::compile(const String& code)
    {
        String msg;

        const GLchar* codeArray[] = { code.c_str() };
        glShaderSource(id, 1, codeArray, NULL);
        glCompileShader(id);

        GLint res;
        glGetShaderiv(id, GL_COMPILE_STATUS, &res);
        if(res == GL_FALSE)
        {
            Array<char> buffer;
            GLint len, written;

            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
            buffer.resize(len);
            glGetShaderInfoLog(id, len, &written, buffer.data());

            assert(len == written + 1);

            mini::format(msg, "%0", buffer.data());
            LogError(msg.c_str());

            return false;
        }

        mini::format(msg, "glsl compiled...");
        LogInfo(msg.c_str());

        return true;
    }

    //-----------------------------------//

    void GLSLProgramMetadata::attach(const ResourceHandle& resource, GLenum shaderType, const String& code)
    {
        // store fragment metadata
        GLSLShaderMetadata shaderMetadata;
        shaderMetadata.resource = resource;
        shaderMetadata.type = shaderType;
        shaderMetadata.code = code;

        shaders.pushBack(std::move(shaderMetadata));
    }

    //-----------------------------------//

    bool GLSLProgram::create(GLSLProgramMetadata _metadata)
    {
        metadata = std::move(_metadata);

        // create the program handle
        id = glCreateProgram();
        if(id == 0)
        {
            LogError("Failed to create a glsl program.");
            return false;
        }

        // compile the fragments
        for(auto smd : metadata.shaders)
        {
            GLSLShader shader;
            shader.create(smd.type);
            
            if(shader.compile(smd.code))
            {
                shaders.pushBack(shader);
                glAttachShader(id, shader.id);
            }
            else
            {
                String msg;
                mini::format(msg, "\t^ %0", smd.resource->getPath().c_str());
                LogError(msg.c_str());
            }
        }

        return true;
    }

    void GLSLProgram::destroy()
    {
        for(auto& shader : shaders)
        {
            glDetachShader(id, shader.id);
            glDeleteShader(shader.id);
            shader.id = 0;
        }
        
        shaders.clear();
        
        glDeleteProgram(id);
        id = 0;
    }

    bool GLSLProgram::link()
    {
        String msg;
        
        // link the program
        glLinkProgram(id);
        
        // report any error messages
        GLint status;
        glGetProgramiv(id, GL_LINK_STATUS, &status);
        if(status == GL_FALSE)
        {
            Array<char> buffer;
            GLint len, written;

            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
            buffer.resize(len);

            glGetProgramInfoLog(id, len, &written, buffer.data());
            assert(written + 1 == len);

            mini::format(msg, "%0", buffer.data());
            LogError(msg.c_str());

            return false;
        }

        // extract active uniforms
        int32 count, len, written;
        glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &len);
        glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);

        Array<char> buffer;
        buffer.resize(len);

        for(int32 i = 0; i < count; ++i)
        {
            GLSLUniformMetadata umd;
            GLSLUniform u;

            glGetActiveUniform(id, i, len, &written, &umd.size, &umd.type, buffer.data());
            u.location = glGetUniformLocation(id, buffer.data());

            uniforms.pushBack(u);
            metadata.uniforms.pushBack(umd);
        }

        mini::format(msg, "glsl program linked...");
        LogInfo(msg.c_str());

        return true;
    }

    //-----------------------------------//

    GLSLProgramManager::GLSLProgramManager()
    {}

    GLSLProgramManager::~GLSLProgramManager()
    {
        auto res = fldCore()->resourceManager;
        res->onResourceReloaded.Disconnect(this, &GLSLProgramManager::onResourceReload);
    }
    
    void GLSLProgramManager::init()
    {
        auto res = fldCore()->resourceManager;
        res->onResourceReloaded.Connect(this, &GLSLProgramManager::onResourceReload);
    }

    GLSLProgramPtr GLSLProgramManager::load(const String & vsFilename, const String & fsFilename)
    {
        String progName;
        mini::format(progName, "%0 %1", vsFilename.c_str(), fsFilename.c_str());
        auto progKey = MurmurHash64(progName.c_str(), progName.size(), 0);
        auto prog = programs.get(progKey, nullptr);

        if(prog != nullptr)
            return prog;

        auto res = fldCore()->resourceManager;
        auto vsText = res->loadResource<Text>(vsFilename);
        auto fsText = res->loadResource<Text>(fsFilename);

        auto vsDeps = dependencies.get((uint64)vsText.getId(), nullptr);
        auto fsDeps = dependencies.get((uint64)fsText.getId(), nullptr);
        if(!vsDeps)
        {
            vsDeps = Allocate(AllocatorGetHeap(), Array<uint64>);
            dependencies.set((uint64)vsText.getId(), vsDeps);
        }
        if(!fsDeps)
        {
            fsDeps = Allocate(AllocatorGetHeap(), Array<uint64>);
            dependencies.set((uint64)fsText.getId(), fsDeps);
        }

        GLSLProgramMetadata progData;
        progData.attach(HandleCast<Resource>(vsText), GL_VERTEX_SHADER, vsText->data);
        progData.attach(HandleCast<Resource>(fsText), GL_FRAGMENT_SHADER, fsText->data);

        prog = Allocate(AllocatorGetHeap(), GLSLProgram);
        prog->create(progData);
        prog->link();

        vsDeps->pushBack(progKey);
        fsDeps->pushBack(progKey);
        programs.set(progKey, prog);

        return prog;
    }

    void GLSLProgramManager::onResourceReload(const ResourceEvent& e)
    {
        auto deps = dependencies.get((uint64)e.handle.id, nullptr);
        if(deps == nullptr)
            return;

        auto res = (HandleCast<Text>(e.handle));
        res->addReference();

        for(auto key : *deps)
        {
            auto prog = programs.get(key, nullptr);
            assert(prog);

            for(auto& shader : prog->metadata.shaders)
            {
                if(shader.resource.id == e.handle.id)
                {
                    shader.code = res->data;
                    break;
                }
            }

            prog->destroy();

            prog->create(std::move(prog->metadata));
            prog->link();
        }
    }
}
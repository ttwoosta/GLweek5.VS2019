#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.h"

#include <string>
#include <memory>
using namespace std;

using glm::vec4; using glm::mat4;

namespace {
    template <typename GET, typename LOG>
    string show_log_info(GLuint object, GET glGet__iv, LOG glGet__InfoLog)
    {
        GLint log_length;
        
        glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
        unique_ptr<char[]> buffer{new char[log_length]};
        glGet__InfoLog(object, log_length, NULL, buffer.get());
        return string{buffer.get()};
    }
}

namespace gl {
    template<>
    Name<Shader>::~Name() {
        if (_name) { glDeleteShader(_name); }
    }
    
    Shader::Shader(Shader::Kind type, string const& source)
    :   Name {glCreateShader(static_cast<GLenum>(type))}
    {
        GLint length = static_cast<GLint>(source.length());
        char const* text = source.c_str();
        glShaderSource(_name, 1, &text, &length);
        glCompileShader(_name);
        GLint shaderStatus = false;
        glGetShaderiv(_name, GL_COMPILE_STATUS, &shaderStatus);
        if (!shaderStatus) { throw invalid_argument{"Failed to compile shader: " + show_log_info(_name, glGetShaderiv, glGetShaderInfoLog)};}
    }
    
    template<>
    Name<Program>::Name()
    :   Name(glCreateProgram())
    {}
    
    template<>
    Name<Program>::~Name()
    {
        if (_name) { glDeleteProgram(_name); }
    }

    Program::AttributeBinding& Program::AttributeBinding::operator=(Program::AttributeBinding::value_type v)
    {
        glBindAttribLocation(_host, _value = v, c_str()); 
        return *this; 
    }
    Program::AttributeBinding::operator value_type() const
    {
        return _value != -1 ? _value : (_value = glGetAttribLocation(_host, c_str()));
    }
    
    Program::Program(const Shader& vertex, const Shader& fragment)
    {
        Link(list<const Shader*> {&vertex, &fragment});
    }
    
    void Program::Activate() const
    {
        glUseProgram(_name);
    }
    
    void Program::Deactivate()
    {
        glUseProgram(0);
    }

    Program::UniformBinding& Program::UniformBinding::operator=(UniformBuffer::BindingPoint value)
    {
        glUniformBlockBinding(program._name, index, value);
        TRAPGL("uniform error: ");
        return *this;
    }

    Program::UniformBinding::operator UniformBuffer::BindingPoint() const
    {
        Int result = 0;
        glGetActiveUniformBlockiv(program._name, index, static_cast<GLenum>(UniformBuffer::ParameterName::Binding), &result);
        return result;
    }

    Program::UniformBinding Program::operator[](const std::string& block_name)
    {
        auto index = glGetUniformBlockIndex(_name, block_name.c_str());

        return UniformBinding{ index, *this };
    }

    const Program::UniformBinding Program::operator[](const std::string& block_name) const
    {
        auto index = glGetUniformBlockIndex(_name, block_name.c_str());

        return UniformBinding{ index, *this };
    }

    template<>
    Program::UniformAccessor<Int>::operator Int() const
    {
        Int value;
        glGetUniformiv(program._name, index, &value);
        return value;
    }
    
    template<>
    Program::UniformAccessor<Uint>::operator Uint() const
    {
        Uint value;
        glGetUniformuiv(program._name, index, &value);
        return value;
    }

	template<>
	Program::UniformAccessor<Float>::operator Float() const
	{
		Float value;
		glGetUniformfv(program._name, index, &value);
		return value;
	}

    template<>
    Program::UniformAccessor<Vector3>::operator Vector3() const
    {
        Vector3 value;
        glGetUniformfv(program._name, index, glm::value_ptr(value));
        return value;
    }
    
    template<>
    Program::UniformAccessor<Vector4>::operator Vector4() const
    {
        Vector4 value;
        glGetUniformfv(program._name, index, glm::value_ptr(value));
        return value;
    }
    
    template<>
    Program::UniformAccessor<Matrix4>::operator Matrix4() const
    {
        Matrix4 value;
        glGetUniformfv(program._name, index, glm::value_ptr(value));
        return value;
    }
    
    template<>
    Program::UniformAccessor<Int>& Program::UniformAccessor<Int>::operator=(const Int& value)
    {
        glProgramUniform1i(program._name, index, value);
        return *this;
    }
    
    template<>
    Program::UniformAccessor<Uint>& Program::UniformAccessor<Uint>::operator=(const Uint& value)
    {
        glProgramUniform1ui(program._name, index, value);
        return *this;
    }
	
	template<>
	Program::UniformAccessor<Float>& Program::UniformAccessor<Float>::operator=(const Float& value)
	{
		glProgramUniform1f(program._name, index, value);
		return *this;
	}

    template<>
    Program::UniformAccessor<Vector3>& Program::UniformAccessor<Vector3>::operator=(const Vector3& value)
    {
        glProgramUniform3fv(program._name, index, 1, glm::value_ptr(value));
        return *this;
    }
    
    template<>
    Program::UniformAccessor<Vector4>& Program::UniformAccessor<Vector4>::operator=(const Vector4& value)
    {
        glProgramUniform4fv(program._name, index, 1, glm::value_ptr(value));
        return *this;
    }
    
    template<>
    Program::UniformAccessor<Matrix4>& Program::UniformAccessor<Matrix4>::operator= (const Matrix4& value)
    {
        glProgramUniformMatrix4fv(program._name, index, 1, GL_FALSE, glm::value_ptr(value));
        return *this;
    }
    
    void Program::Link(std::list<const Shader*> sh)
    {
        for (const Shader* shader: sh) { glAttachShader(_name, shader->_name); }
        glLinkProgram(_name);
        for (const Shader* shader: sh) { glDetachShader(_name, shader->_name); }
        GLint linkStatus = false;
        glGetProgramiv(_name, GL_LINK_STATUS, &linkStatus);
        if (!linkStatus) { throw invalid_argument{"Failed to link program: " + show_log_info(_name, glGetProgramiv, glGetProgramInfoLog)}; }
    }
}

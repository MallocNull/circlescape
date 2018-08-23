#include "_shader.hpp"

sosc::shdr::Shader::Shader() : loaded(false) {}

bool sosc::shdr::Shader::Load() {
    if(this->loaded)
        return true;

    program = glCreateProgram();
    PrepareLoad();

    this->loaded = true;
    return true;
}

void sosc::shdr::Shader::Start() const {
    if(!this->loaded)
        return;

    glUseProgram(this->program);
}

void sosc::shdr::Shader::Stop() const {
    if(!this->loaded)
        return;

    glUseProgram(0);
}

void sosc::shdr::Shader::Unload() {
    if(!this->loaded)
        return;

    PrepareUnload();
    glDeleteProgram(this->program);
    this->loaded = false;
}

void sosc::shdr::Shader::AttachSource
    (const std::string& fileName, GLuint shaderType)
{
    if(this->loaded)
        return;

    GLuint shader = glCreateShader(shaderType);

    std::ifstream file(fileName);
    std::stringstream ss;
    ss << file.rdbuf();

    const char* src = ss.str().c_str();
    glShaderSource(shader, 1, (const GLchar**)&src, nullptr);
    glCompileShader(shader);

    GLint tmpBuf;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &tmpBuf);
    if(tmpBuf == GL_FALSE) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &tmpBuf);

        auto msg = new char[tmpBuf];
        glGetShaderInfoLog(shader, tmpBuf, nullptr, msg);
        throw ShaderCompilationException(fileName, msg);
    }

    glAttachShader(this->program, shader);
    this->shaders.push_back(shader);
}

void sosc::shdr::Shader::LinkProgram() {
    glLinkProgram(this->program);
    for(const auto& shader : this->shaders)
        glDeleteShader(shader);
}

void sosc::shdr::Shader::LoadUniforms(std::vector<std::string> names) {
    if(this->loaded)
        return;

    GLint id;
    this->locations.clear();
    for(const auto& name : names) {
        if((id = glGetUniformLocation(this->program, name.c_str())) == -1)
            throw ShaderUniformException(name);

        this->locations.push_back(id);
    }
}
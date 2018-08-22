#include "_shader.hpp"

sosc::Shader::Shader() : loaded(false) {}

bool sosc::Shader::Load() {
    if(this->loaded)
        return true;

    program = glCreateProgram();
    if(!PrepareLoad())
        return false;

    this->loaded = true;
    return true;
}

void sosc::Shader::Start() const {
    if(!this->loaded)
        return;

    glUseProgram(this->program);
}

void sosc::Shader::Stop() const {
    if(!this->loaded)
        return;

    glUseProgram(0);
}

void sosc::Shader::Unload() {
    if(!this->loaded)
        return;

    PrepareUnload();
    glDeleteProgram(this->program);
    this->loaded = false;
}

void sosc::Shader::AttachSource
    (const std::string& fileName, GLuint shaderType)
{
    if(this->loaded)
        return;


}

void sosc::Shader::LinkProgram() {
    glLinkProgram(this->program);
    for(const auto& shader : this->shaders)
        glDeleteShader(shader);
}

void sosc::Shader::LoadUniforms(std::vector<std::string> names) {
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
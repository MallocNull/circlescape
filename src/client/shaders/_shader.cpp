#include "_shader.hpp"

sosc::shdr::Shader::Shader() : loaded(false) {}

bool sosc::shdr::Shader::Load() {
    if(this->loaded)
        return true;

    this->program = glCreateProgram();
    this->PrepareLoad();

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

bool sosc::shdr::Shader::AttachSource
    (const std::string& fileName, GLuint shaderType)
{
    if(this->loaded)
        return false;

    GLuint shader = glCreateShader(shaderType);

    std::ifstream file(fileName);
    std::stringstream ss;
    ss << file.rdbuf();
    std::string source = ss.str();

    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint err_chk;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &err_chk);
    if(err_chk == GL_FALSE) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &err_chk);
        auto msg = new char[err_chk];

        glGetShaderInfoLog(shader, err_chk, nullptr, msg);
        std::cerr << "Error in '"<< fileName << "':" << msg << std::endl;
        delete[] msg;

        return false;
    }

    glAttachShader(this->program, shader);
    this->shaders.push_back(shader);
    return true;
}

bool sosc::shdr::Shader::LinkProgram() {
    GLint err_chk;

    glLinkProgram(this->program);
    glGetProgramiv(this->program, GL_LINK_STATUS, &err_chk);
    if(err_chk == GL_FALSE) {
        glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &err_chk);
        auto msg = new char[err_chk];

        glGetProgramInfoLog(this->program, err_chk, nullptr, msg);
        std::cerr << "Error linking shader: " << msg << std::endl;
        delete[] msg;

        return false;
    }

    for(const auto& shader : this->shaders)
        glDeleteShader(shader);
    return true;
}

bool sosc::shdr::Shader::LoadUniforms(std::vector<std::string> names) {
    if(this->loaded)
        return false;

    GLint id;
    this->locations.clear();
    for(const auto& name : names) {
        if((id = glGetUniformLocation(this->program, name.c_str())) == -1)
            0; //return false;

        this->locations.push_back(id);
    }

    return true;
}
#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
class Shader{
    public:
    Shader(const char* vertexPath, const char* fragmentPath);
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void Use();
    
    void setMat4(const std::string& string, const glm::mat4& matrix);

    private:
    unsigned int programID;
    std::unordered_map<std::string, GLint> uniformslocationCache;
    GLint GetUniformLocation(const std::string& key);
    void CheckCompileErrors(unsigned int ID, const std::string& description, const std::string& type);

};


#endif


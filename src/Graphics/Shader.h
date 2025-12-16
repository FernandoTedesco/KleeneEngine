
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
class Shader{
    public:
    Shader(const char* vertexPath, const char* fragmentPath);
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void Use();
    void SetVec3(const std::string &name, const glm::vec3 &value);
    void SetMat4(const std::string& string, const glm::mat4& matrix);
    void SetInt(const std::string &name, int value);

    private:
    unsigned int programID;
    std::unordered_map<std::string, GLint> uniformslocationCache;
    GLint GetUniformLocation(const std::string& key);
    std::string LoadShaderSource(const std::string& path);
    void CheckCompileErrors(unsigned int ID, const std::string& description, const std::string& type);

};




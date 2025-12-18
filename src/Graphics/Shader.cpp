#include "Shader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <exception>
#include "Mesh.h"



Shader::Shader(const char* vertexPath,const char* fragmentPath){

    //Path Reading & Shader code build
    std::string vertexStringCode = LoadShaderSource(vertexPath);
    std::string fragmentStringCode = LoadShaderSource(fragmentPath);

    const char* vertexCode = vertexStringCode.c_str();
    const char* fragmentCode = fragmentStringCode.c_str();

    //Shader compiling
    unsigned int vertexID = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexID, 1,&vertexCode, NULL);
    glShaderSource(fragmentID, 1, &fragmentCode, NULL);

    glCompileShader(vertexID);
    glCompileShader(fragmentID);

    CheckCompileErrors(vertexID,"Vertex Shader", "SHADER" );
    CheckCompileErrors(fragmentID,"Fragment Shader", "SHADER" );

    //Create the shader program
    this->programID = glCreateProgram();
    glAttachShader(this->programID, vertexID);
    glAttachShader(this->programID, fragmentID);

    glLinkProgram(this->programID);

    CheckCompileErrors(this->programID,"Shader Program", "PROGRAM" );

    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);

}


Shader::Shader(const std::string& vertexPath, const std::string& fragPath):Shader(vertexPath.c_str(),fragPath.c_str())
{
}



void Shader::Use(){

glUseProgram(this->programID);

}

void Shader::SetMat4(const std::string& string, const glm::mat4& matrix){

    GLint location = GetUniformLocation(string);

    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));

}

void Shader::CheckCompileErrors(unsigned int ID, const std::string& description, const std::string& type){

    int success;
    char infoLog[512];

    if(type == "SHADER")
    {
        glGetShaderiv(ID,GL_COMPILE_STATUS, &success);
        if(!success)
        {
         glGetShaderInfoLog(ID, 512, NULL, infoLog);
         std::cout<<"ERROR::SHADER::COMPILATION_FAILED"<<std::endl;
         std::cout<<"System log:"<< infoLog <<std::endl;
         std::cout<<"Attempted to compile the shader:"<<description<<" but failed"<<std::endl;
         throw std::runtime_error("FATAL: Shader Compilation has failed");
        }
    }
    else if(type == "PROGRAM")
    {
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success)
        {
         glGetProgramInfoLog(ID, 512, NULL, infoLog);
         std::cout<<"ERROR::SHADER::LINKING_FAILED"<<std::endl;
         std::cout<<"System Log:"<< infoLog <<std::endl;
         std::cout<<"Attempted to link shaders into a program object:"<<description<<" but failed"<<std::endl; 
         throw std::runtime_error("FATAL: Shader program has failed to be linked");
        }
    }
    
    
}
std::string Shader::LoadShaderSource(const std::string& path){

    
   

    std::string rawShaderCode;
    

    std::ifstream shaderFile;
    std::stringstream shaderStream;
    

    shaderFile.exceptions(std::ifstream::badbit|std::ifstream::failbit);
    
    
    try{
        shaderFile.open(path);
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();

    }
    catch(std::ifstream::failure& e){
        std::cout<<"ERROR::SHADER::FILE_OPENING_FAIL"<<std::endl;
        std::cout<<"System log:"<< e.what()<<std::endl;
        std::cout<<"Tried to open"<< path << " but failed" <<std::endl;
        throw std::runtime_error("FATAL: Shader Source File could not be found");
    }
    
    rawShaderCode = shaderStream.str();
    return(rawShaderCode);


}

void Shader::SetVec3(const std::string &name, const glm::vec3 &value){
    GLint location = GetUniformLocation(name);
    glUniform3f(location, value.x, value.y, value.z);
}

void Shader::SetInt(const std::string &name, int value){

    GLint location = GetUniformLocation(name);
    glUniform1i(location, value);

}

GLint Shader::GetUniformLocation(const std::string& key){

    std::unordered_map<std::string, GLint>::iterator iterator;
    iterator = uniformslocationCache.find(key);

    if(iterator != uniformslocationCache.end()) //Cache HIT
    {
        return iterator->second;
    }
    else //Cache MISS
    {

        GLint location = glGetUniformLocation(this->programID, key.c_str());
        if(location == -1)
        {
         std::cout<<"ERROR::SHADER::UNIFORM_NOT_FOUND"<<std::endl;
         std::cout<<"Could not find Uniform '"<<key<<"' in the shader program:"<<this->programID<<std::endl; 
        }
        uniformslocationCache[key] = location;
        return location;

    }
}

Shader::~Shader()
{
    glDeleteProgram(programID);
}

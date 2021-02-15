#include "Shader.h"
#include <string>
#include <fstream>

static std::string LoadShader(const char* fileName)
{
	std::ifstream file;
	file.open(fileName);
	std::string output;
	std::string line;
	if (file.is_open())
	{
		while (file.good())
		{
			std::getline(file, line);
			output.append(line + "\n");
		}
	}
	else
	{
		printf("Failed to load Shader: %s",fileName);
	}
	return output.c_str();
}

static void CheckShaderErrors(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };
	if (isProgram)
	{
		glGetProgramiv(shader, flag, &success);
	}
	else
	{
		glGetShaderiv(shader, flag, &success);
	}

	if (success == GL_FALSE)
	{
		if (isProgram)
		{
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		}
		else
		{
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);
		}
		printf("%s : %s", errorMessage.c_str(), error);
	}
}

static GLuint CreateShader(std::string code, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);
	if (shader != 0)
	{
		const GLchar* shaderString[1];
		GLint shaderStringLength[1];
		shaderString[0] = code.c_str();
		shaderStringLength[0] = code.size();

		glShaderSource(shader, 1, shaderString, shaderStringLength);
		glCompileShader(shader);
		CheckShaderErrors(shader, GL_COMPILE_STATUS, false, "SHADER COMPILATION ERROR:");
		return shader;
	}
	else
	{
		printf("Shader creation failed!");
		return 0;
	}
}



Shader::Shader(const char* vshaderName, const char* fshaderName)
{
	CreateShaderProgram(programID, shaders, vshaderName, fshaderName);
}

Shader::~Shader()
{
	RemoveShaderProgram();
}

void Shader::Update(const Transform& transform, const Camera& camera)
{
	glm::mat4 modelTransform = transform.GetTransformMatrix();
	glUniformMatrix4fv(uniform[0], 1, GL_FALSE, &modelTransform[0][0]);
	
	glm::mat4 view = camera.GetViewMatrix();
	glUniformMatrix4fv(uniform[1], 1, GL_FALSE, &view[0][0]);
}

void Shader::Bind()
{
	glUseProgram(programID);
}

void Shader::CreateShaderProgram(GLuint& newProgramID, GLuint* newShaders,const char* vshaderName, const char* fshaderName)
{
	newProgramID = glCreateProgram();
	std::string vshaderPath(ASSET_DIR);
	vshaderPath.append("Shaders\\");
	std::string fshaderPath(vshaderPath);
	vshaderPath.append("Vertex\\");
	fshaderPath.append("Fragment\\");
	vshaderPath.append(vshaderName);
	fshaderPath.append(fshaderName);
	newShaders[0] = CreateShader(LoadShader(vshaderPath.c_str()), GL_VERTEX_SHADER);
	newShaders[1] = CreateShader(LoadShader(fshaderPath.c_str()), GL_FRAGMENT_SHADER);

	glAttachShader(newProgramID, newShaders[0]);
	glAttachShader(newProgramID, newShaders[1]);

	glBindAttribLocation(newProgramID, 0, "position");

	glLinkProgram(newProgramID);
	CheckShaderErrors(newProgramID, GL_LINK_STATUS, true, "SHADER PROGRAM LINKER ERROR");

	glValidateProgram(newProgramID);
	CheckShaderErrors(newProgramID, GL_VALIDATE_STATUS, true, "SHADER PROGRAM INVALID");

	uniform[0] = glGetUniformLocation(programID, "transform");
	uniform[1] = glGetUniformLocation(programID, "view");
}

void Shader::RemoveShaderProgram()
{
	glDetachShader(programID, shaders[0]);
	glDetachShader(programID, shaders[1]);
	glDeleteShader(shaders[0]);
	glDeleteShader(shaders[1]);
	glDeleteProgram(programID);
}

void Shader::Hot_Reload(const char* vshaderName, const char* fshaderName)
{
	GLuint newProgram;
	GLuint newShaders[2];
	CreateShaderProgram(newProgram, newShaders, vshaderName, fshaderName);
	RemoveShaderProgram();
	programID = newProgram;
	shaders[0] = newShaders[0];
	shaders[1] = newShaders[1];
}



#include "Shader.h"

Shader::Shader()
{
	shaderID = 0; // init default values 
	u_Model = 0;
	u_Projection = 0;
}

Shader::~Shader()
{

}

void Shader::CreateFromString(const char* vertexShader, const char* fragmentShader)
{
	CompileShader(vertexShader, fragmentShader); //create shader
}

void Shader::CreateFromFile(const char* vertexPath, const char* geometryPath, const char* fragmentPath)
{
	std::string vertexStr = ReadFile(vertexPath); // load shader code into strings
	std::string geometryStr = ReadFile(geometryPath);
	std::string fragmentStr = ReadFile(fragmentPath);

	CompileShader(vertexStr.c_str(), geometryStr.c_str(), fragmentStr.c_str()); // compile strings into shaders
}

void Shader::CreateFromFile(const char* vertexPath, const char* fragmentPath)
{
	std::string vertexStr = ReadFile(vertexPath); // load shader code into strings
	std::string fragmentStr = ReadFile(fragmentPath);

	CompileShader(vertexStr.c_str(), fragmentStr.c_str()); // compile strings into shaders

}

void Shader::CreateFromFile(const char* computePath)
{
	std::string computeStr = ReadFile(computePath); // load shader code into strings
	CompileShader(computeStr.c_str());  // compile strings into shaders
}

std::string Shader::ReadFile(const char* path)
{
	std::string content; // used to store text from file
	std::ifstream fileStream(path, std::ios::in); // data stream for reading file

	if (!fileStream.is_open()) { // if file doesnt open

		printf("[ERROR READING FILE]: Failed to open file {%s}\n", path); // print error
		return ""; //return no data
	}

	std::string line = ""; //used to read each line

	while (!fileStream.eof()) // while not at end of file
	{
		std::getline(fileStream, line); // read line
		content.append(line + "\n"); // add new line and append to total
	}

	fileStream.close(); //close file
	return content; //return file text

}

GLuint Shader::GetProjectionLocation()
{
	return u_Projection;
}

GLuint Shader::GetModelLocation()
{
	return u_Model;
}

void Shader::Bind()
{
	if (shaderID != 0) // if shader id i not default 
		glUseProgram(shaderID); // use 
	else
		std::cout << "[SHADER ERROR]: Shader did not bind..\n"; // else print error
}

void Shader::Unbind()
{
	glUseProgram(0); //bind 0
}

void Shader::ClearShader()
{
	if (shaderID != 0) { // delete program
		glDeleteProgram(shaderID);
		shaderID = 0;
	}

	u_Projection = 0;
	u_Model = 0;
}

void Shader::Set1f(GLfloat value, const GLchar* name)
{
	glUniform1f(glGetUniformLocation(shaderID, name), value); 
}

void Shader::Set1i(GLint value, const GLchar* name)
{
	glUniform1i(glGetUniformLocation(this->shaderID, name), value);
}

void Shader::Set2f(glm::vec2 value, const GLchar* name)
{
   glUniform2fv(glGetUniformLocation(shaderID, name), 1, glm::value_ptr(value));
}

void Shader::SetVec2f(glm::vec2 value, const GLchar* name)
{
	glUniform2fv(glGetUniformLocation(shaderID, name), 1, glm::value_ptr(value));
}

void Shader::SetVec3f(glm::fvec3 value, const GLchar* name)
{
	glUniform3fv(glGetUniformLocation(shaderID, name), 1, glm::value_ptr(value));
}

void Shader::SetVec4f(glm::vec4 value, const GLchar* name)
{
	glUniform4fv(glGetUniformLocation(shaderID, name), 1, glm::value_ptr(value));
}

void Shader::SetMat3f(glm::mat3 value, const char* name, bool transpose)
{
	glUniformMatrix3fv(glGetUniformLocation(shaderID, name), 1, transpose, glm::value_ptr(value));
}

void Shader::SetMat4f(glm::mat4 value, const char* uniformName, bool transpose)
{
	glUniformMatrix4fv(glGetUniformLocation(shaderID, uniformName), 1, transpose, glm::value_ptr(value));
}

void Shader::Validate()
{
	GLint result = 0; //check for errors
	GLchar eLog[1024] = { 0 }; 

	glValidateProgram(shaderID); 
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
	if (!result) // if error
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog); // get information about error
		printf("Error validating program: '%s'\n", eLog); //print error
		return;
	}
}

void Shader::QueryWorkgroups() {

	int work_grp_size[3], work_grp_inv;
	// maximum global work group (total work in a dispatch)
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_size[2]);
	//printf("max global (total) work group size x:%i y:%i z:%i\n", work_grp_size[0], work_grp_size[1], work_grp_size[2]);
	// maximum local work group (one shader's slice)
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
	//printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n", work_grp_size[0], work_grp_size[1], work_grp_size[2]);
	// maximum compute shader invocations (x * y * z)
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	//printf("max computer shader invocations %i\n", work_grp_inv);
}

void Shader::CompileShader(const char* vertexShader, const char* fragmentShader)
{
	shaderID = glCreateProgram(); // create shader program

	AddShader(shaderID, vertexShader, GL_VERTEX_SHADER); //add shader to program
	AddShader(shaderID, fragmentShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar errorLog[1024] = { 0 }; 

	glLinkProgram(shaderID); // link program
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); //check result
	if (!result) { // if error
		glGetProgramInfoLog(shaderID, sizeof(errorLog), NULL, errorLog);
		printf("[ERROR]: Linking program failed: '%s'\n", errorLog); // print error
		return;
	}

	glValidateProgram(shaderID);
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result); // check program is valid
	if (!result) { // if error
		glGetProgramInfoLog(shaderID, sizeof(errorLog), NULL, errorLog);
		printf("[ERROR]: Validating program failed: '%s'\n", errorLog); // print error
		return;
	}

	if (!shaderID) { // if shader id is null
		printf("[ERROR]: shader wasn't complied..\n"); // print error
		return;
	}

	u_Model = glGetUniformLocation(shaderID, "u_Model"); // get model uniform location
	u_Projection = glGetUniformLocation(shaderID, "u_Projection"); // get projection uniform location

}

void Shader::CompileShader(const char* vertexShader, const char* geometryShader, const char* fragmentShader)
{
	shaderID = glCreateProgram(); //create shader program

	AddShader(shaderID, vertexShader, GL_VERTEX_SHADER); //add shdaer code
	AddShader(shaderID, geometryShader, GL_GEOMETRY_SHADER);
	AddShader(shaderID, fragmentShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar errorLog[1024] = { 0 };

	glLinkProgram(shaderID); // link program and error check
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, sizeof(errorLog), NULL, errorLog);
		printf("[ERROR]: Linking program failed: '%s'\n", errorLog);
		return;
	}

	glValidateProgram(shaderID);
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result); // validate program and error check
	if (!result) {
		glGetProgramInfoLog(shaderID, sizeof(errorLog), NULL, errorLog);
		printf("[ERROR]: Validating program failed: '%s'\n", errorLog);
		return;
	}

	if (!shaderID) {
		printf("[ERROR]: shader wasn't complied..\n");
		return;
	}

	u_Model = glGetUniformLocation(shaderID, "u_Model"); //get uniform locations
	u_Projection = glGetUniformLocation(shaderID, "u_Projection");
}

void Shader::CompileShader(const char* computeShader)
{
	
	shaderID = glCreateProgram(); //create program

	AddShader(shaderID, computeShader, GL_COMPUTE_SHADER); // add shader code to program

	GLint result = 0;
	GLchar errorLog[1024] = { 0 };

	glLinkProgram(shaderID);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);  // link program and error check
	if (!result) {
		glGetProgramInfoLog(shaderID, sizeof(errorLog), NULL, errorLog);
		printf("[ERROR]: Linking program failed: '%s'\n", errorLog);
		return;
	}

	glValidateProgram(shaderID);
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);  // validate program and error check
	if (!result) {
		glGetProgramInfoLog(shaderID, sizeof(errorLog), NULL, errorLog);
		printf("[ERROR]: Validating program failed: '%s'\n", errorLog);
		return;
	}

	if (!shaderID) {
		printf("[ERROR]: shader wasn't complied..\n");
		return;
	}

}

void Shader::AddShader(GLuint program, const char* shaderCode, GLenum type)
{
	GLuint shader = glCreateShader(type); //create shader
	const char* programCode[1]; 
	programCode[0] = shaderCode; //store code as char array

	GLint codeLength[1]; 
	codeLength[0] = strlen(shaderCode); //get string length

	glShaderSource(shader, 1, (const GLchar* const*)programCode, codeLength); // pass data to shader
	glCompileShader(shader); // compile shader

	GLint result = 0;
	GLchar errorLog[1024] = { 0 };
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result); // check for errors
	if (!result) {
		glGetShaderInfoLog(shader, sizeof(errorLog), NULL, errorLog);
		printf("[ERROR]: Compiliing %d shader: '%s'\n", type, errorLog);
		return;
	}
	glAttachShader(program, shader);
}

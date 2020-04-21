#include <glad/glad.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Light {
public:
	glm::vec3 pos;


	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 dist;

	float constant;
	float linear;
	float quadratic;

	Light() {
		ambient = diffuse = specular = pos = glm::vec3(1.0f);
		constant = linear = quadratic = 0.0f;
	}

};


class Material {
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
	int diffBlock;
	int specBlock;
	Material() {
		ambient = diffuse = specular = glm::vec3(1.0f);
		shininess = 0.0f;
	}
};

class Shader {


public:
	unsigned int id;
	Shader() {

	}
	Shader(const std::string filepath) {
		std::ifstream file(filepath);

		if (!file) {
			std::cout << "File isnt opened" << std::endl;
		}
		int mode = -1;
		std::string line;
		std::stringstream ss[2];
		while (!file.eof()) {
			getline(file, line);
			if (line.find("shader") != std::string::npos) {
				if (line.find("vertex") != std::string::npos) {
					mode = 0;
				}
				if (line.find("fragment") != std::string::npos) {
					mode = 1;
				}
			}
			else {
				ss[mode] << line << '\n';
			}
			//std::cout << line << std::endl;
		}
		std::string strCode[2];
		strCode[0] = ss[0].str();
		strCode[1] = ss[1].str();
		const GLchar * shaderCode[2];
		shaderCode[0] = strCode[0].c_str();
		shaderCode[1] = strCode[1].c_str();;
		//std::cout << ss[1].str() << std::endl;
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(vertexShader, 1, &shaderCode[0], NULL);
		glCompileShader(vertexShader);
		glShaderSource(fragmentShader, 1, &shaderCode[1], NULL);
		glCompileShader(fragmentShader);
		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		};
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		};
		id = glCreateProgram();
		glAttachShader(id, vertexShader);
		glAttachShader(id, fragmentShader);
		glLinkProgram(id);
		glGetProgramiv(this->id, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(this->id, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	void setUniformVec3(const GLchar * name, glm::vec3 &color) {
		bind();
		int loc = glGetUniformLocation(id, name);
		if (loc == -1) {
			std::cout <<"uniform didnt find" << std::endl;
		}
		glUniform3fv(loc, 1, &color[0]);
		unBind();
	}
	void setUniform4m(const GLchar * name, glm::mat4 &mat) {
		bind();
		int loc = glGetUniformLocation(id, name);
		if (loc == -1) {
			std::cout << "uniform didnt find" << std::endl;;
		}
		glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
		unBind();
	}
	void setUniform1f(const GLchar * name, GLfloat var) {
		bind();
		//std::cout << "setting " << name << " uniform" << std::endl;
		int loc = glGetUniformLocation(id, name);
		if (loc == -1) {
			std::cout << "uniform didnt find" << std::endl;
		}
		glUniform1f(loc, var);
		unBind();
	}

	void setUniform1i(const GLchar * name, GLint var) {
		bind();
		unsigned int loc = glGetUniformLocation(id, name);
		if (loc == -1) {
			std::cout << "uniform didnt find" << std::endl;
		}
		glUniform1i(loc, var);
		unBind();
	}


	void setMaterial(std::string name, Material material) {
		unsigned int loc = glGetUniformLocation(id, name.c_str());
		std::string diffPlace = name + "." + "diffuse";
		std::string ambientPlace = name + "." + "ambient";
		std::string specPlace = name + "." + "specular";
		std::string shinePlace = name + "." + "shininess";

		setUniform1i(diffPlace.c_str(), material.diffBlock);
		setUniform1i(specPlace.c_str(), material.specBlock);
		//setUniformVec3(specPlace.c_str(), material.specular);
		setUniform1f(shinePlace.c_str(), material.shininess);
	}

	/*	void setMaterial(std::string name, Material material) {
		bind();
		unsigned int loc = glGetUniformLocation(id, name.c_str());
		std::string diffPlace = name + "." + "diffuse";
		std::string ambientPlace = name + "." + "ambient";
		std::string specPlace = name+ "." + "specular";
		std::string shinePlace = name + "." + "shininess";

		setUniformVec3(diffPlace.c_str(), material.diffuse);
		setUniformVec3(ambientPlace.c_str(), material.ambient);
		setUniformVec3(specPlace.c_str(), material.specular);
		setUniform1f(shinePlace.c_str(), material.shininess);
		unBind();
	}
	 */

	void setLight(std::string name, Light light) {
		bind();
		unsigned int loc = glGetUniformLocation(id, name.c_str());
		std::string diffPlace = name + "." + "diffuse";
		std::string ambientPlace = name + "." + "ambient";
		std::string specPlace = name + "." + "specular";
		std::string posPlace = name + "." + "position";
		std::string constPlace = name + "." + "distanses";


		

		setUniformVec3(diffPlace.c_str(), light.diffuse);
		setUniformVec3(ambientPlace.c_str(), light.ambient);
		setUniformVec3(specPlace.c_str(), light.specular);
		setUniformVec3(posPlace.c_str(), light.pos);
		//setUniformVec3(constPlace.c_str(), light.dist);

		unBind();
	}

	void bind() const {
		glUseProgram(id);
	}
	void unBind() const {
		glUseProgram(0);
	}
};

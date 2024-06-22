#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Common/include/stb_image.h"
#include "../Common/include/Shader.h"
#include "SceneObject.cpp"

// Dimens�es da janela
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Vari�veis de controle de rota��o
bool rotateX = false, rotateY = false, rotateZ = false;

// Vari�veis de controle de transla��o
bool translateX = false, translateY = false, translateZ = false;
int translateDirection = 0;

// Vari�vel de controle de escala
float scale = 1.0;

// Ajusta a escala com base na tecla pressionada.
void adjustScale(int key)
{
	float scaleFactor = 0.05;

	if (key == GLFW_KEY_KP_ADD)
		scale += scale * scaleFactor;
	else if (key == GLFW_KEY_KP_SUBTRACT)
		scale -= scale * scaleFactor;
}

// Ajusta a rota��o com base na tecla pressionada.
void adjustRotation(int key)
{
	switch (key)
	{
	case(GLFW_KEY_X):
		rotateX = true;
		rotateY = false;
		rotateZ = false;
		break;
	case(GLFW_KEY_Y):
		rotateX = false;
		rotateY = true;
		rotateZ = false;
		break;
	case(GLFW_KEY_Z):
		rotateX = false;
		rotateY = false;
		rotateZ = true;
		break;
	default:
		break;
	}
	
}

// Ajusta a transla��o com base na tecla pressionada.
void adjustTranslation(int key)
{
	switch (key)
	{
	case(GLFW_KEY_RIGHT):
		translateX = true;
		translateY = false;
		translateZ = false;
		translateDirection = 1;
		break;
	case(GLFW_KEY_LEFT):
		translateX = true;
		translateY = false;
		translateZ = false;
		translateDirection = -1;
		break;
	case(GLFW_KEY_UP):
		translateX = false;
		translateY = true;
		translateZ = false;
		translateDirection = 1;
		break;
	case(GLFW_KEY_DOWN):
		translateX = false;
		translateY = true;
		translateZ = false;
		translateDirection = -1;
		break;
	case(GLFW_KEY_W):
		translateX = false;
		translateY = false;
		translateZ = true;
		translateDirection = 1;
		break;
	case(GLFW_KEY_S):
		translateX = false;
		translateY = false;
		translateZ = true;
		translateDirection = -1;
		break;
	default:
		break;
	}
}

// Fun��o callback acionada quando h� intera��o com o teclado
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	adjustScale(key);
	adjustRotation(key);
	adjustTranslation(key);
}

// Reseta vari�veis de controle de transla��o
void resetTranslationVariables() {
	translateX = false;
	translateY = false;
	translateZ = false;
	translateDirection = 0;
}

// Cria e retorna um vetor de objetos da cena, representando cubos, distribu�dos horizontalmente, com base no n�mero fornecido (numObjects)
std::vector<SceneObject> generateSceneObjects(int numObjects, GLuint vertexArrayObject, int numVertices, Shader* shader, GLuint textureId) {
	std::vector<SceneObject> objects;

	const float horizontalSpacing = 2.75f;

	for (int i = 0; i < numObjects; ++i)
	{
		float xPosition = 0.0f;

		if (i % 2 == 0)
			xPosition = (-horizontalSpacing) * (i / 2);
		else
			xPosition = (horizontalSpacing) * ((i / 2) + 1);

		objects.push_back(SceneObject(vertexArrayObject, numVertices, shader, textureId, glm::vec3(xPosition, 0.0, 0.0)));
	}

	return objects;
}

// Fun��o para ler o arquivo OBJ e extrair os dados de v�rtices e �ndices
bool readOBJFile(const std::string& filepath, std::vector<GLuint>& indices, std::vector<GLfloat>& vbuffer,
	string& materialFileName, string& materialName) {

	glm::vec3 color = glm::vec3(1.0, 0.0, 1.0);

	vector <glm::vec2> textureCoordinates;
	vector <glm::vec3> vertices;
	vector <glm::vec3> normals;

	// Abrindo o arquivo OBJ
	std::ifstream inputFile(filepath);
	if (!inputFile.is_open()) {
		std::cerr << "Erro ao abrir o arquivo OBJ: " << filepath << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(inputFile, line)) {
		std::istringstream ssline(line);
		std::string word;
		ssline >> word;

		if (word == "mtllib") {
			ssline >> materialFileName;
		}
		else if (word == "usemtl") { 
			ssline >> materialName;
		}
		else if (word == "v") {
			glm::vec3 v;
			ssline >> v.x >> v.y >> v.z;
			vertices.push_back(v);
		}
		else if (word == "vt")
		{
			glm::vec2 vt;
			ssline >> vt.s >> vt.t;
			textureCoordinates.push_back(vt);
		}
		else if (word == "vn")
		{
			glm::vec3 vn;
			ssline >> vn.x >> vn.y >> vn.z;
			normals.push_back(vn);
		}
		else if (word == "f") {
			std::string tokens[3];
			ssline >> tokens[0] >> tokens[1] >> tokens[2];

			for (int i = 0; i < 3; ++i) {
				int posLastValue = tokens[i].find_last_of('/');
				std::string lastValue = tokens[i].substr(posLastValue + 1);
				int normal = std::stoi(lastValue);

				int pos = tokens[i].find("/");
				std::string token = tokens[i].substr(0, pos);
				int index = std::atoi(token.c_str()) - 1;
				indices.push_back(index);

				vbuffer.push_back(vertices[index].x);
				vbuffer.push_back(vertices[index].y);
				vbuffer.push_back(vertices[index].z);

				vbuffer.push_back(color.r);
				vbuffer.push_back(color.g);
				vbuffer.push_back(color.b);

				// Movendo para a pr�xima parte da string para obter o �ndice da textura
				tokens[i] = tokens[i].substr(pos + 1);
				pos = tokens[i].find("/");
				token = tokens[i].substr(0, pos);
				index = atoi(token.c_str()) - 1; // Convertendo o �ndice para inteiro e ajustando para come�ar de 0

				// Adicionando as coordenadas da textura ao buffer de v�rtices
				vbuffer.push_back(textureCoordinates[index].s);
				vbuffer.push_back(textureCoordinates[index].t);

				//Recuperando os indices de vns
				tokens[i] = tokens[i].substr(pos + 1);
				index = atoi(tokens[i].c_str()) - 1;

				vbuffer.push_back(normals[index].x);
				vbuffer.push_back(normals[index].y);
				vbuffer.push_back(normals[index].z);
			}
		}
	}

	inputFile.close();
	return true;
}

// Fun��o para inicializar os buffers de v�rtices e arrays de v�rtices (VAO e VBO)
bool initializeBuffers(GLuint& VBO, GLuint& VAO, const std::vector<GLfloat>& vbuffer, int stride) {
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Especificando os atributos do v�rtice
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

// Fun��o principal para carregar um arquivo OBJ e inicializar os buffers de v�rtices e arrays de v�rtices (VAO e VBO)
int loadSimpleOBJ(const std::string& filepath, int& numVertices, string& materialFileName, string& materialName) {
	std::vector<GLuint> indices;
	std::vector<GLfloat> vbuffer;
	int stride = 11;

	if (!readOBJFile(filepath, indices, vbuffer, materialFileName, materialName)) {
		std::cerr << "Erro ao ler o arquivo OBJ: " << filepath << std::endl;
		return -1;
	}

	numVertices = vbuffer.size() / stride;

	GLuint VBO, VAO;
	if (!initializeBuffers(VBO, VAO, vbuffer, stride)) {
		std::cerr << "Erro ao inicializar os buffers de v�rtices e arrays de v�rtices." << std::endl;
		return -1;
	}

	return VAO;
}

// Carrega uma textura a partir de um arquivo, configura seus par�metros e retorna o ID da textura
int loadTexture(string filepath)
{
	GLuint texID;

	// Gera o identificador da textura na mem�ria 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	//Ajusta os par�metros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Carregamento da imagem
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		GLenum format;
		switch (nrChannels) {
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			std::cerr << "N�mero de canais n�o suportado: " << nrChannels << std::endl;
			stbi_image_free(data);
			return 0;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Falha ao carregar a textura" << std::endl;
		return 0;
	}

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

// Carrega um arquivo MTL simples, extrai o nome do arquivo de textura e o armazena na vari�vel textureFileName
string loadSimpleMTL(const std::string& filepath, string materialName)
{
	string textureFileName;
	std::ifstream inputFile(filepath);

	if (!inputFile.is_open()) {
		std::cerr << "Erro ao abrir o arquivo MTL: " << filepath << std::endl;
		return "";
	}

	string line;
	bool materialFound = false;

	while (getline(inputFile, line))
	{
		istringstream ssline(line);
		string word;
		ssline >> word; 
		
		if (word == "newmtl")
		{
			string currentMaterialName;
			ssline >> currentMaterialName;
			materialFound = (currentMaterialName == materialName);
		} else if (word == "map_Kd" && materialFound) { 
			ssline >> textureFileName;
			break;
		}
	}

	inputFile.close();
	return textureFileName;
}

int main()
{
	// Inicializa��o da GLFW
	glfwInit();

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Modulo 4", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, keyCallback);

	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Falha ao inicializar o GLAD" << std::endl;
	}

	// Obtendo as informa��es de vers�o
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderizador: " << renderer << endl;
	cout << "Vers�o OpenGL suportada" << version << endl;

	// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	Shader shader("VShader.vs", "FShader.fs");
	glUseProgram(shader.ID);

	//Matriz de view
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	shader.setMat4("view", value_ptr(view));

	//Matriz de proje��o perspectiva - definindo o volume de visualiza��o (frustum)
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	shader.setMat4("projection", glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);

	int numVertices;
	string materialFileName;
	string materialName;
	//GLuint VAO = loadSimpleOBJ("../3D_models/Suzanne/SuzanneTriTextured.obj", numVertices, materialFileName, materialName);
	GLuint VAO = loadSimpleOBJ("../3D_models/Cube/cube.obj", numVertices, materialFileName, materialName);

	// Carregamento do arquivo MTL para obter as informa��es do material
	string textureFileName = loadSimpleMTL(materialFileName, materialName);

	//Carregando uma textura e armazenando o identificador na mem�ria
	GLuint textureId = loadTexture(textureFileName);

	int numObjetcts = 7;
	std::vector<SceneObject> sceneObjects = generateSceneObjects(numObjetcts, VAO, numVertices, &shader, textureId);

	//Atualizando o shader com a posi��o da c�mera
	shader.setVec3("camera_pos", 0.0, 0.0, 3.0);
	
	//Definindo as propriedades do material da superficie
	shader.setFloat("ka", 0.2);
	shader.setFloat("kd", 0.5);
	shader.setFloat("ks", 0.5);
	shader.setFloat("q", 10.0);

	//Definindo a fonte de luz pontual
	shader.setVec3("light_pos", 0.0, 10.0, 0.0);
	shader.setVec3("light_color", 1.0, 1.0, 0.8);

	// Loop da aplica��o
	while (!glfwWindowShouldClose(window))
	{
		resetTranslationVariables();

		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		for (int i = 0; i < sceneObjects.size(); ++i)
		{
			if (rotateX)
				sceneObjects[i].rotateX();
			else if (rotateY)
				sceneObjects[i].rotateY();
			else if (rotateZ)
				sceneObjects[i].rotateZ();

			if (translateX)
				sceneObjects[i].translateX(translateDirection);
			else if (translateY)
				sceneObjects[i].translateY(translateDirection);
			else if (translateZ)
				sceneObjects[i].translateZ(translateDirection);

			sceneObjects[i].setScale(glm::vec3(scale, scale, scale));
			sceneObjects[i].updateModelMatrix();
			sceneObjects[i].renderObject();
		}

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}
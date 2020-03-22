#include <iostream>
#include <glew.h>
#include <glfw3.h>
#include <Glaux.h>
#include <glut.h>
#include <vec3.hpp> 
#include <vec4.hpp> 
#include <mat4x4.hpp> 
#include <gtc/matrix_transform.hpp>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <string>
#include <map>
#include <algorithm>
#include <glm.hpp>

#define N 6

#define WIDTH 1024
#define HEIGHT 1024

using namespace std;
using namespace glm;

GLFWwindow* window;

vector <const char*> Meshes
{
	"cube1.obj",
	"plane.obj",
	"cube2.obj",
	"Hare.obj",
	"ball.obj",
	"spider.obj"
};

vector <const char*> Textures
{
	"kl.bmp",
	"trava.bmp",
	"kl.bmp",
	"fur.bmp"
};
GLfloat lastX = WIDTH / 2;
GLfloat lastY = HEIGHT / 2;

GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;

vec3 light_pos = vec3(10, 10, 2);
float fog = 0.0;

vec3 cameraPos = vec3(2.0f, 2.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

bool keys[1024];
bool firstMouse = true; 

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	GLfloat cameraSpeed = 0.05f;
	if (key == GLFW_KEY_W)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
	if (key == GLFW_KEY_S)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
	if (key == GLFW_KEY_A)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
	if (key == GLFW_KEY_D)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
	if (key == GLFW_KEY_F)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
	if (key == GLFW_KEY_G)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void do_movement()
{
	// Camera controls
	GLfloat cameraSpeed = 0.04f;
	if (keys[GLFW_KEY_W])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_S])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_A])
		cameraPos -= normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		cameraPos += normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_F])
	{
		fog = 1.0;
	}
	if (keys[GLFW_KEY_G])
	{
		fog = 0.0;
	}

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	GLfloat sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	vec3 front;
	front.x = cos(radians(yaw)) * cos(radians(pitch));
	front.y = sin(radians(pitch));
	front.z = sin(radians(yaw)) * cos(radians(pitch));
	cameraFront = normalize(front);
}
void Tangent_Bitangent(vector<vec3> & vertices, vector<vec2> & uvs, vector<vec3> & normals,
	vector<vec3> & tangents, vector<vec3> & bitangents)
{
	for (unsigned int i = 0; i < vertices.size(); i += 3)
	{ 
		//направление касательной и бикасательной выровнено с направлениями текстурных координат, задающими координаты поверхности
		vec3 v0 = vertices[i];
		vec3 v1 = vertices[i + 1];
		vec3 v2 = vertices[i + 2];

		vec2 uv0 = uvs[i];
		vec2 uv1 = uvs[i + 1];
		vec2 uv2 = uvs[i + 2];

		vec3 deltaPos1 = v1 - v0;
		vec3 deltaPos2 = v2 - v0;

		vec2 deltaUV1 = uv1 - uv0;
		vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
		vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

		tangents.push_back(tangent);
		tangents.push_back(tangent);
		tangents.push_back(tangent);

		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);

	}

}

bool is_near(float v1, float v2)
{
	return fabs(v1 - v2) < 0.01f;
}

bool getSimilarIndex1(vec3& in_vertex, vec2& in_uv, vec3& in_normal,
	vector<vec3>& out_vertices, vector<vec2>& out_uvs, vector<vec3>& out_normals,
	unsigned short& result)
{
	for (unsigned int i = 0; i < out_vertices.size(); i++)
	{
		if (is_near(in_vertex.x, out_vertices[i].x) && is_near(in_vertex.y, out_vertices[i].y) &&
			is_near(in_vertex.z, out_vertices[i].z) && is_near(in_uv.x, out_uvs[i].x) &&
			is_near(in_uv.y, out_uvs[i].y) && is_near(in_normal.x, out_normals[i].x) &&
			is_near(in_normal.y, out_normals[i].y) && is_near(in_normal.z, out_normals[i].z))
		{
			result = i;
			return true;
		}
	}
	return false;
}

struct PackedVertex
{
	vec3 position;
	vec2 uv;
	vec3 normal;
	bool operator<(const PackedVertex ob) const
	{
		return memcmp((void*)this, (void*)&ob, sizeof(PackedVertex)) > 0;
	};
};

bool getSimilarIndex2(PackedVertex& packed, map<PackedVertex, unsigned short>& VertexToOutIndex,
	unsigned short& result)
{
	map<PackedVertex, unsigned short>::iterator it = VertexToOutIndex.find(packed);
	if (it == VertexToOutIndex.end())
	{
		return false;
	}
	else
	{
		result = it->second;
		return true;
	}
}

void Index_vert1(vector<vec3>& in_vertices, vector<vec2>& in_uvs, vector<vec3>& in_normals,
	vector<unsigned short>& out_indices, vector<vec3> & out_vertices, vector<vec2> & out_uvs,
	vector<vec3> & out_normals)
{
	map<PackedVertex, unsigned short> VertexToOutIndex;

	for (unsigned int i = 0; i < in_vertices.size(); i++)
	{
		PackedVertex packed = { in_vertices[i], in_uvs[i], in_normals[i] };
		unsigned short index;
		bool found = getSimilarIndex2(packed, VertexToOutIndex, index);
		if (found)
		{
			out_indices.push_back(index);
		}
		else
		{
			out_vertices.push_back(in_vertices[i]);
			out_uvs.push_back(in_uvs[i]);
			out_normals.push_back(in_normals[i]);
			unsigned short newindex = (unsigned short)out_vertices.size() - 1;
			out_indices.push_back(newindex);
			VertexToOutIndex[packed] = newindex;
		}
	}
}
void Index_vert2(vector<vec3>& in_vertices, vector<vec2>& in_uvs, vector<vec3>& in_normals,
	vector<vec3>& in_tangents, vector<vec3>& in_bitangents, vector<unsigned short> & out_indices,
	vector<vec3>& out_vertices, vector<vec2>& out_uvs, vector<vec3>& out_normals,
	vector<vec3>& out_tangents, vector<vec3>& out_bitangents)
{
	for (unsigned int i = 0; i < in_vertices.size(); i++)
	{
		unsigned short index;
		bool found = getSimilarIndex1(in_vertices[i], in_uvs[i], in_normals[i], out_vertices, out_uvs, out_normals, index);
		if (found)
		{
			out_indices.push_back(index);
			out_tangents[index] += in_tangents[i];
			out_bitangents[index] += in_bitangents[i];
		}
		else
		{
			out_vertices.push_back(in_vertices[i]);
			out_uvs.push_back(in_uvs[i]);
			out_normals.push_back(in_normals[i]);
			out_tangents.push_back(in_tangents[i]);
			out_bitangents.push_back(in_bitangents[i]);
			out_indices.push_back((unsigned short)out_vertices.size() - 1);
		}
	}
}
GLuint GetBMPPicture(const char * imagepath) {

	cout << "Image: " << imagepath << endl;

	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	unsigned char* data;

	FILE* file = fopen(imagepath, "rb");
	if (!file)
	{
		cout << imagepath << "could not be opened" << endl;
		return 0;
	}

	if (fread(header, 1, 54, file) != 54)
	{
		cout << "Not a correct BMP file" << endl;
		fclose(file);
		return 0;
	}
	if (header[0] != 'B' || header[1] != 'M')
	{
		cout << "Not a correct BMP file" << endl;
		fclose(file);
		return 0;
	}

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	if (imageSize == 0)
		imageSize = width * height * 3;
	if (dataPos == 0)
		dataPos = 54;

	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	delete[] data;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	return textureID;
}

bool GetOBJ(const char* path, vector<vec3> & out_vertices, vector<vec2>& out_uvs,
	vector<vec3> & out_normals)
{
	cout << "Loading OBJ file: " << path << endl;

	vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	vector<vec3> temp_vertices;
	vector<vec2> temp_uvs;
	vector<vec3> temp_normals;

	FILE* file = fopen(path, "r");
	if (file == NULL)
	{
		cout << "Impossible to open the file!" << endl;
		return false;
	}

	while (1)
	{
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0)
		{
			vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else
			if (strcmp(lineHeader, "vt") == 0)
			{
				vec2 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				temp_uvs.push_back(uv);
			}
			else
				if (strcmp(lineHeader, "vn") == 0)
				{
					glm::vec3 normal;
					fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
					temp_normals.push_back(normal);
				}
				else
					if (strcmp(lineHeader, "f") == 0)
					{
						std::string vertex1, vertex2, vertex3;
						unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
						int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
						if (matches != 9)
						{
							cout << "File can't be read by parser" << endl;
							fclose(file);
							return false;
						}
						vertexIndices.push_back(vertexIndex[0]);
						vertexIndices.push_back(vertexIndex[1]);
						vertexIndices.push_back(vertexIndex[2]);
						uvIndices.push_back(uvIndex[0]);
						uvIndices.push_back(uvIndex[1]);
						uvIndices.push_back(uvIndex[2]);
						normalIndices.push_back(normalIndex[0]);
						normalIndices.push_back(normalIndex[1]);
						normalIndices.push_back(normalIndex[2]);

					}
	}
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		vec3 vertex = temp_vertices[vertexIndex - 1];
		vec2 uv = temp_uvs[uvIndex - 1];
		vec3 normal = temp_normals[normalIndex - 1];

		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);
	}
	fclose(file);
	return true;
}

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	string Line;

	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, ios::in);
	Line = "";
	while (getline(VertexShaderStream, Line))
		VertexShaderCode += "\n" + Line;
	VertexShaderStream.close();

	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_file_path, ios::in);
	Line = "";
	while (getline(FragmentShaderStream, Line))
		FragmentShaderCode += "\n" + Line;
	FragmentShaderStream.close();

	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
int main(void)
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1024, 768, "Comp_Grapf", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	int windowWidth = HEIGHT;
	int windowHeight = WIDTH;
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwPollEvents();
	glfwSetCursorPos(window, HEIGHT / 2, WIDTH / 2);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glClearColor(0.5f, 0.6f, 0.5f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	GLuint VertexArrayID[N];
	glGenVertexArrays(N, VertexArrayID);

	GLuint depthProgramID = LoadShaders("depth.vertex", "depth.fragment"); //рендер сцены для источника света
	GLuint depthMatrixID = glGetUniformLocation(depthProgramID, "depthMVP");

	GLuint Texture[N - 2];
	vector<vec3> vertices[N];
	vector<vec2> uvs[N];
	vector<vec3> normals[N];

	vector<vec3> sun_v;
	vector<vec2> sun_u;
	vector<vec3> sun_n;

	bool sun = GetOBJ("sun.obj", sun_v, sun_u, sun_n);

	GLuint sun_vertexbuffer;
	glGenBuffers(1, &sun_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, sun_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sun_v.size() * sizeof(vec3), &sun_v[0], GL_STATIC_DRAW);

	bool res[N];
	for (int i = 0; i < N; i++)
	{
		//Texture[i] = GetBMPPicture(Textures[i]);
		res[i] = GetOBJ(Meshes[i], vertices[i], uvs[i], normals[i]);
	}
	

	for (int i = 0; i < N - 2; i++)
	{
		Texture[i] = GetBMPPicture(Textures[i]);
		//res[i] = GetOBJ(Meshes[i], vertices[i], uvs[i], normals[i]);
	}
	vector<unsigned short> indices[N];
	vector<vec3> indexed_vertices[N];
	vector<vec2> indexed_uvs[N];
	vector<vec3> indexed_normals[N];

	GLuint vertexbuffer[N];
	glGenBuffers(N, vertexbuffer);
	GLuint uvbuffer[N];
	glGenBuffers(N, uvbuffer);
	GLuint normalbuffer[N];
	glGenBuffers(N, normalbuffer);
	GLuint elementbuffer[N];
	glGenBuffers(N, elementbuffer);
	vector<vec3> tangents;
	vector<vec3> bitangents;
	vector<vec3> indexed_tangents;
	vector<vec3> indexed_bitangents;
	GLuint tangentbuffer;
	glGenBuffers(1, &tangentbuffer);
	

	GLuint bitangentbuffer;
	glGenBuffers(1, &bitangentbuffer);
	
	Tangent_Bitangent(vertices[N - 1], uvs[N - 1], normals[N - 1], tangents, bitangents);
	for (int i = 0; i < N; i++)
	{
		if (i == N - 1)
		{
			Index_vert2(vertices[i], uvs[i], normals[i], tangents, bitangents, indices[i], indexed_vertices[i], indexed_uvs[i], indexed_normals[i], indexed_tangents, indexed_bitangents);
			glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
			glBufferData(GL_ARRAY_BUFFER, indexed_tangents.size() * sizeof(vec3), &indexed_tangents[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
			glBufferData(GL_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(vec3), &indexed_bitangents[0], GL_STATIC_DRAW);
		}
		else
		{
			Index_vert1(vertices[i], uvs[i], normals[i], indices[i], indexed_vertices[i], indexed_uvs[i], indexed_normals[i]);
		}
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, indexed_vertices[i].size() * sizeof(vec3), &indexed_vertices[i][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, indexed_uvs[i].size() * sizeof(vec2), &indexed_uvs[i][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, indexed_normals[i].size() * sizeof(vec3), &indexed_normals[i][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(unsigned short), &indices[i][0], GL_STATIC_DRAW);

	}
	
	//создаем карту глубины
	GLuint Framebufferdepth = 0; 
	glGenFramebuffers(1, &Framebufferdepth);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebufferdepth);

	//в качестве буфера глубины
	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, HEIGHT, WIDTH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER/*EDGE*/);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glDrawBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	GLuint programID = LoadShaders("main.vertex", "main.fragment");

	GLuint programID1 = LoadShaders("cooktor.vertex", "cooktor.fragment");

	GLuint programID2 = LoadShaders("normalmapping.vertex", "normalmapping.fragment");

	GLuint sun_programID = LoadShaders("sun.vertex", "sun.fragment");
	GLuint sun_MatrixID = glGetUniformLocation(sun_programID, "MVP");

	GLuint TextureID[N];

	for (int i = 0; i < N; i++)
	{
		TextureID[i] = glGetUniformLocation(programID, "my_texture");
	}

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint FogID = glGetUniformLocation(programID, "if_fog");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	GLuint DepthBiasID = glGetUniformLocation(programID, "D_MVP");
	GLuint ShadowMapID = glGetUniformLocation(programID, "shadow_map");

	GLuint LightID = glGetUniformLocation(programID, "light_in_world");
	//GLuint EyeID = glGetUniformLocation(programID, "eye_world");

	GLuint MatrixID1 = glGetUniformLocation(programID1, "MVP");
	GLuint ViewMatrixID1 = glGetUniformLocation(programID1, "V");
	GLuint ModelMatrixID1 = glGetUniformLocation(programID1, "M");
	GLuint DepthBiasID1 = glGetUniformLocation(programID1, "D_MVP");
	GLuint ShadowMapID1 = glGetUniformLocation(programID1, "shadow_map");
	GLuint LightID1 = glGetUniformLocation(programID1, "light_pos");
	GLuint FogID1 = glGetUniformLocation(programID1, "if_fog");

	GLuint MatrixID2 = glGetUniformLocation(programID2, "MVP");
	GLuint ViewMatrixID2 = glGetUniformLocation(programID2, "V");
	GLuint ModelMatrixID2 = glGetUniformLocation(programID2, "M");
	GLuint ModelView3x3MatrixID = glGetUniformLocation(programID2, "MV");
	GLuint LightID2 = glGetUniformLocation(programID2, "light_pos");
	GLuint DepthBiasID2 = glGetUniformLocation(programID2, "D_MVP");
	GLuint ShadowMapID2 = glGetUniformLocation(programID2, "shadow_map");
	GLuint FogID2 = glGetUniformLocation(programID2, "if_fog");

	GLuint DiffuseTexture = GetBMPPicture("color.bmp");
	GLuint NormalTexture =GetBMPPicture("normal.bmp");

	GLuint DiffuseTextureID = glGetUniformLocation(programID2, "DiffuseTexture");
	GLuint NormalTextureID = glGetUniformLocation(programID2, "NormalTexture");

	mat4 Projection = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	mat4 Model = mat4(1.0f);

	mat4 depthProjectionMatrix = ortho<float>(-20, 20, -20, 20, -30, 30); //ortho<float>(-10, 10, -10, 10, -10, 20); задаем область отсечения теней
	mat4 lightViewMatrix = lookAt(light_pos, vec3(0, 0, 0), vec3(0, 1, 0)); //матрица вида, в которой будут видны все объекты с точки зрения источника света
	mat4 depthMVP = depthProjectionMatrix * lightViewMatrix; //задает, то как источник света видит мир
	mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,    //переводит координаты фрагметов из [-1; 1] в [0; 1] чтобы использовать в качестве выборки текстуры
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0);
	mat4 depthBiasMVP = biasMatrix * depthMVP;
	do 
	{
		glfwPollEvents();
		do_movement();
		glBindFramebuffer(GL_FRAMEBUFFER, Framebufferdepth);
		glViewport(0, 0, HEIGHT, WIDTH);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mat4 View = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		mat4 MVP = Projection * View * Model;

		mat4 ModelViewMatrix = View * Model;
		mat3 ModelView3x3Matrix = mat3(ModelViewMatrix);
		//glCullFace(GL_FRONT);
		for (int i = 0; i < N; i++) //рисуем карту глубины
		{

			glUseProgram(depthProgramID);
			glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

			glBindVertexArray(VertexArrayID[i]);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[i]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer[i]);

			glDrawElements(GL_TRIANGLES, indices[i].size(), GL_UNSIGNED_SHORT, (void*)0);
			glDisableVertexAttribArray(0);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, windowWidth, windowHeight);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int i = 0; i < N; i++)
		{
			if (i == 5)
			{
				glUseProgram(programID2);
				glBindVertexArray(VertexArrayID[i]);

				glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
				glUniformMatrix4fv(ModelMatrixID2, 1, GL_FALSE, &Model[0][0]);
				glUniformMatrix4fv(ViewMatrixID2, 1, GL_FALSE, &View[0][0]);
				glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
				glUniformMatrix4fv(DepthBiasID2, 1, GL_FALSE, &depthBiasMVP[0][0]);
				glUniform3f(LightID2, light_pos.x, light_pos.y, light_pos.z);
				glUniform1f(FogID2, fog);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, DiffuseTexture);
				glUniform1i(DiffuseTextureID, 0);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, NormalTexture);
				glUniform1i(NormalTextureID, 1);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, depthTexture);
				glUniform1i(ShadowMapID2, 2);

				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[i]);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[i]);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

				glEnableVertexAttribArray(2);
				glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[i]);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

				glEnableVertexAttribArray(3);
				glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

				glEnableVertexAttribArray(4);
				glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
				glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer[i]);
				glDrawElements(GL_TRIANGLES, indices[i].size(), GL_UNSIGNED_SHORT, (void*)0);

				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(2);
				glDisableVertexAttribArray(3);
				glDisableVertexAttribArray(4);
			}
			else
			if (i == 4)
			{
				glUseProgram(programID1);
				glBindVertexArray(VertexArrayID[i]);

				glUniformMatrix4fv(MatrixID1, 1, GL_FALSE, &MVP[0][0]);
				glUniformMatrix4fv(ModelMatrixID1, 1, GL_FALSE, &Model[0][0]);
				glUniformMatrix4fv(ViewMatrixID1, 1, GL_FALSE, &View[0][0]);
				glUniformMatrix4fv(DepthBiasID1, 1, GL_FALSE, &depthBiasMVP[0][0]);

				glUniform3f(LightID1, light_pos.x, light_pos.y, light_pos.z);
				glUniform1f(FogID1, fog);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, depthTexture);
				glUniform1i(ShadowMapID1, 0);

				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[i]);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[i]);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

				glEnableVertexAttribArray(2);
				glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[i]);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer[i]);
				glDrawElements(GL_TRIANGLES, indices[i].size(), GL_UNSIGNED_SHORT, (void*)0);

				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(2);
			}
			else
			{
				glUseProgram(programID);
				glBindVertexArray(VertexArrayID[i]);

				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
				glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
				glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);

				glUniform3f(LightID, light_pos.x, light_pos.y, light_pos.z);
				glUniform1f(FogID, fog);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, Texture[i]);
				glUniform1i(TextureID[i], 0);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, depthTexture);
				glUniform1i(ShadowMapID, 1);

				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[i]);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[i]);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

				glEnableVertexAttribArray(2);
				glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[i]);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer[i]);
				glDrawElements(GL_TRIANGLES, indices[i].size(), GL_UNSIGNED_SHORT, (void*)0);

				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(2);
			}

		}
		glUseProgram(sun_programID);
		glUniformMatrix4fv(sun_MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, sun_vertexbuffer);
		glVertexAttribPointer(
			0,                  
			3,                  
			GL_FLOAT,           
			GL_FALSE,           
			0,                  
			(void*)0           
		);
		glDrawArrays(GL_TRIANGLES, 0, sun_v.size()); 
		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(N, vertexbuffer);
	glDeleteBuffers(N, uvbuffer);
	glDeleteBuffers(N, normalbuffer);
	glDeleteBuffers(N, elementbuffer);
	glDeleteBuffers(1, &sun_vertexbuffer);

	glDeleteProgram(programID);
	glDeleteProgram(sun_programID);
	glDeleteProgram(programID1);
	glDeleteProgram(programID2);
	glDeleteProgram(depthProgramID);

	glDeleteTextures(N-2, Texture);
	glDeleteTextures(1, &DiffuseTexture);
	glDeleteTextures(1, &NormalTexture);

	glDeleteFramebuffers(1, &Framebufferdepth);
	glDeleteTextures(1, &depthTexture);
	glDeleteVertexArrays(N, VertexArrayID);

	glfwTerminate();
	return 0;
}
#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "ShapeFactory.h"

class VertexBuffer 
{
public:
	VertexBuffer() = default;
	~VertexBuffer();

	void create(unsigned int count);
	void bufferData(const void* data, unsigned int count);
	void bind() const;
	void unbind() const;

private:
	unsigned int m_ID = 0;
};

class IndexBuffer
{
public:
	IndexBuffer() = default;
	~IndexBuffer();

	void create(unsigned int count);
	void bufferData(const void* data, unsigned int count);
	void bind() const;
	void unbind() const;

	//return count
	inline unsigned int GetCount() const { return m_IndicesCount; }

private:
	unsigned int m_ID = 0;
	unsigned int m_IndicesCount = 0;
};

//Buffer layout abstraction layer
struct VertexBufferElement
{
	unsigned int count;
	unsigned int type;
	unsigned int normalized;

	static unsigned int getTypeSize(unsigned int type) {
		switch (type) {
		case GL_FLOAT: return 4;
		case GL_UNSIGNED_INT: return 4; //can extend for more data types
		case GL_UNSIGNED_BYTE: return 1;
		default: return 0;
		}
	}
};

class VertexBufferLayout 
{
public:
	VertexBufferLayout() = default;
	~VertexBufferLayout() = default;

	template<typename T>
	void push(unsigned int count) { //default
		static_assert(false, "Unknown type pushed to VBL");
	}

	template<>
	void push<float>(unsigned int count) { //push float
		m_Elements.push_back({count, GL_FLOAT, GL_FALSE});
		m_Stride += count * VertexBufferElement::getTypeSize(GL_FLOAT);
	}

	template<>
	void push<unsigned int>(unsigned int count) {  //push uint
		m_Elements.push_back({ count, GL_UNSIGNED_INT, GL_FALSE });
		m_Stride += count * VertexBufferElement::getTypeSize(GL_UNSIGNED_INT);
	}

	template<>
	void push<unsigned char>(unsigned int count) {  //push uint
		m_Elements.push_back({ count, GL_UNSIGNED_BYTE, GL_TRUE });
		m_Stride += count * VertexBufferElement::getTypeSize(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferElement> getElements() const& {
		return m_Elements;
	}

	inline unsigned int getStride() const { return m_Stride; }

private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride = 0;
};

class VertexArray
{
public:
	VertexArray() = default;
	~VertexArray();

	void create();
	void bind() const;
	void unbind() const;

	void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);


private:
	unsigned int m_ID;
};

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
public:
	Shader() = default;
	~Shader();

	void create(const std::string &filepath);

	void bind() const;
	void unbind() const;

	//set uniforms - overload for each data type
	void setUniform(const std::string &name, float uniform);
	void setUniform(const std::string& name, const glm::vec3* uniform);
	void setUniform(const std::string& name, const glm::mat4* uniform);
	
private:
	unsigned int m_ID = 0;
	std::unordered_map<std::string, unsigned int> m_UniformLocationCache;
	unsigned int compileShader(const std::string& source, unsigned int type);
	unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
	ShaderProgramSource parseShader(const std::string& filePath);
	unsigned int getUniformLocation(const std::string& name);
};

//ACTUAL RENDERER CLASSES
struct primitiveLocation
{	
	//used to return where a particular primitive is stored - needs to provide tri count
	unsigned int indexVert;
	unsigned int indexSizeVert;
	unsigned int indexInd;
	unsigned int indexSizeInd;
};

class Camera
{
public:
	Camera(float width, float height);
	~Camera() = default;
	void translateCamera(float deltaX, float deltaY);
	void positionCamera(float x, float y);
	void zoomCamera(float deltaZoom);
	void setZoomCamera(float zoom);

	//get data from camera
	void sendCameraUniforms(Shader& shader);

	//get bounds
	glm::mat4 bounds() const { return m_ResultantMatrix; }

private:
	float m_CameraWidth; float m_CameraHeight;
	glm::vec3 m_Scaling = { 1.0f , 1.0f, 1.0f }; //zoom
	glm::mat4 m_Translation = glm::mat4(1.0f);
	glm::mat4 m_Projection;
	Component4f m_CameraBounds = { -1.0f, 1.0f, -1.0f, 1.0f }; //xmin,xmax,ymin,ymax

	//Result to be sent to screen
	glm::mat4 m_ResultantMatrix = glm::mat4(1.0f);
};

class Renderer
{
public:
	Renderer() = default;
	Renderer(float width, float height);

	void init(float width, float height);
	void clearScreen() const;
	static void commitPrimitive(const void* vert, unsigned int vertSize, const void* ind, unsigned short int indSize);
	void drawPrimitives(Shader& shader);
	void draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const;

	
	//is in bounds check - use to enable culling
	bool isInBounds(const void* vert, unsigned int vertSize);
	
	//static indices - some indices are standard and will not change
	static const unsigned int s_TriIndices[];
	static const unsigned int s_QuadIndices[];
	static const unsigned int s_LineIndices[];
	static const unsigned int s_CircleIndices[];

	//static indices sizes
	static const unsigned short int IND_TRI = 3;
	static const unsigned short int IND_QUAD = 6;
	static const unsigned short int IND_LINE = 6;
	static const unsigned short int IND_CIRCLE = 189;

	//Camera
	Camera camera;

private:
	void bufferVideoData();
	void clearPrimitives();
	VertexBuffer m_VB;
	IndexBuffer m_IB;
	VertexArray m_VA;
	VertexBufferLayout m_VBL;

	//data pointers and size
	static std::vector<const void*> s_PrimitiveVertices;
	static std::vector<unsigned int> s_PrimitiveVerticesSize;
	static std::vector<const void*> s_PrimitiveIndices;
	static std::vector<unsigned int> s_PrimitiveIndicesSize;
};



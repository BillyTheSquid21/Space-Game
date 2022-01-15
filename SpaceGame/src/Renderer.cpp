#include "Renderer.h"

//count means number of, size is in bytes

void VertexBuffer::create(unsigned int count) {
	glGenBuffers(1, &m_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
	//tells how much data to set aside - size in bytes
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * count, nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &m_ID);
}

void VertexBuffer::bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

void VertexBuffer::unbind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::bufferData(const void* data, unsigned int count) {
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * count, nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(float), data);
}

void IndexBuffer::create(unsigned int count)
{
	m_IndicesCount = count;
	glGenBuffers(1, &m_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);
}

void IndexBuffer::bufferData(const void* data, unsigned int count) {
	m_IndicesCount = count;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count * sizeof(unsigned int), data);
}

IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1, &m_ID);
}

void IndexBuffer::bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
}

void IndexBuffer::unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//Vertex Array
VertexArray::~VertexArray() {
	glDeleteVertexArrays(1, &m_ID);
}

void VertexArray::create() {
	glGenVertexArrays(1, &m_ID);
	glBindVertexArray(m_ID);
}

void VertexArray::bind() const {
	glBindVertexArray(m_ID);
}

void VertexArray::unbind() const {
	glBindVertexArray(0);
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	bind();
	vb.bind();
	const std::vector<VertexBufferElement> elements = layout.getElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const VertexBufferElement element = elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element.count, element.type, element.normalized,
			layout.getStride(), (const void*)offset);
		offset += element.count * VertexBufferElement::getTypeSize(element.type);
	}
}

//Shader
ShaderProgramSource Shader::parseShader(const std::string& filePath) {
	std::ifstream stream(filePath);
	EngineLog("Reading shader file at: ", filePath);

	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	ShaderType type = ShaderType::NONE;

	std::string line;
	std::stringstream ss[2]; //sets up two string streams
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				type = ShaderType::FRAGMENT;
			}
		}
		else if (type != ShaderType::NONE) {
			ss[(int)type] << line << "\n";
		}

	}

	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader) {
	//returns id of shader program
	unsigned int program = glCreateProgram();
	unsigned int vs = compileShader(vertexShader, GL_VERTEX_SHADER);
	unsigned int fs = compileShader(fragmentShader, GL_FRAGMENT_SHADER);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

unsigned int Shader::compileShader(const std::string& source, unsigned int type) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr); //end specifies length of array which the string is
	glCompileShader(id);                  //(cont) c_str() will alway be null terminating

	//Error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		EngineLog("Shader did not compile correctly");
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		EngineLog(message);

		//Handle
		glDeleteShader(id);
		return 0;
	}

	return id;
}


Shader::~Shader() {
	glDeleteProgram(m_ID);
}

void Shader::create(const std::string& filepath) {
	ShaderProgramSource source = parseShader(filepath);
	m_ID = createShader(source.VertexSource, source.FragmentSource);
}

void Shader::bind() const {
	glUseProgram(m_ID);
}

void Shader::unbind() const {
	glUseProgram(0);
}

//uniforms
void Shader::setUniform(const std::string &name, float uniform) {
	unsigned int location = getUniformLocation(name);
	if (location == -1) {
		return;
	}
	glUniform1f(location, uniform);
}

void Shader::setUniform(const std::string& name, const glm::mat4* uniform) {
	unsigned int location = getUniformLocation(name);
	if (location == -1) {
		return;
	}
	const float* pointer = (float*)(void*)uniform;
	glUniformMatrix4fv(location, 1, GL_FALSE, pointer);
}

void Shader::setUniform(const std::string& name, const glm::vec3* uniform) {
	unsigned int location = getUniformLocation(name);
	if (location == -1) {
		return;
	}
	const float* pointer = (float*)(void*)uniform;
	glUniform3fv(location, 1, pointer);
}

unsigned int Shader::getUniformLocation(const std::string& name) {

	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
		return m_UniformLocationCache[name];
	}

	unsigned int location = glGetUniformLocation(m_ID, name.c_str());
	if (location == -1) {
		EngineLog("Uniform doesn't exist: ", name);
	}
	m_UniformLocationCache[name] = location;

	return location;
}

//RENDERER

//Camera
Camera::Camera(float width, float height)
	:m_CameraWidth{width}, m_CameraHeight{height}, 
	m_Projection{glm::ortho(-width/2, width/2, -height/2, height/2, -1.0f, 1.0f)} //depth buffer layer
{
	m_CameraBounds = { -m_CameraWidth / 2,m_CameraWidth / 2,-m_CameraHeight / 2,m_CameraHeight / 2 };
}

void Camera::positionCamera(float x, float y) {
	m_Translation[3][0] = x;
	m_Translation[3][1] = y;
	m_Translation[3][2] = 1.0f;

	//position bounds
	m_CameraBounds = {x - (m_CameraWidth/2), x + (m_CameraWidth/2), y - (m_CameraHeight/2), y + (m_CameraHeight/2)};
}

void Camera::translateCamera(float deltaX, float deltaY) {
	m_Translation[3][0] += deltaX;
	m_Translation[3][1] += deltaY;
	m_Translation[3][2] = 1.0f;

	//translate bounds
	m_CameraBounds.a += deltaX;
	m_CameraBounds.b += deltaX;
	m_CameraBounds.c += deltaY;
	m_CameraBounds.d += deltaY;
}

void Camera::zoomCamera(float deltaZoom) {
	setZoomCamera(m_Scaling.x + deltaZoom);
}

void Camera::setZoomCamera(float zoom) {
	//correct back to original scale
	m_CameraBounds.a *= m_Scaling.x; m_CameraBounds.b *= m_Scaling.x;
	m_CameraBounds.c *= m_Scaling.y; m_CameraBounds.d *= m_Scaling.y;

	//scale
	m_Scaling.x = zoom;
	m_Scaling.y = zoom;

	//scale accordingly
	m_CameraBounds.a /= m_Scaling.x; m_CameraBounds.b /= m_Scaling.x;
	m_CameraBounds.c /= m_Scaling.y; m_CameraBounds.d /= m_Scaling.y;
}

void Camera::sendCameraUniforms(Shader& shader) {
	//multiply
	m_ResultantMatrix = glm::scale(m_Projection * m_Translation, m_Scaling);
	shader.setUniform("u_Projection", &m_ResultantMatrix);
}

std::vector<const void*> Renderer::s_PrimitiveVertices;
std::vector<unsigned int> Renderer::s_PrimitiveVerticesSize;
std::vector<const void*> Renderer::s_PrimitiveIndices;
std::vector<unsigned int> Renderer::s_PrimitiveIndicesSize;

Renderer::Renderer(float width, float height)
	:camera{Camera::Camera(width, height)}, m_VA{VertexArray::VertexArray()}
{

}

void Renderer::init(float width, float height)
{
	//Init camera
	camera = Camera::Camera(width, height);

	//Create vertex Array
	m_VA.create();

	//Create vertex buffer
	m_VB.create(1);

	//Create indice buffer
	m_IB.create(1);

	//push vertex position floats to layout
	m_VBL.push<float>(3);

	//push color floats to layout
	m_VBL.push<float>(3);

	//add buffer to array
	m_VA.addBuffer(m_VB, m_VBL);

	//Bind
	m_VA.bind();
	m_VB.unbind();
	m_IB.unbind();

	EngineLog("Renderer Initalized");
}

void Renderer::clearScreen() const {
	/* Render here */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::commitPrimitive(const void* vert, unsigned int vertSize, const void* ind, unsigned short int indSize) {	
	s_PrimitiveVertices.push_back(vert);
	s_PrimitiveVerticesSize.push_back(vertSize);
	s_PrimitiveIndices.push_back(ind);
	s_PrimitiveIndicesSize.push_back(indSize);
}

void Renderer::clearPrimitives() {
	s_PrimitiveVertices.clear();
	s_PrimitiveIndices.clear();
	s_PrimitiveIndicesSize.clear();
	s_PrimitiveVerticesSize.clear();
}

void Renderer::bufferVideoData() {
	{
		//First get amount of data among all vertice floats
		unsigned int totalVertFloats = 0;
		for (unsigned int i = 0; i < s_PrimitiveVerticesSize.size(); i++) {
			totalVertFloats += s_PrimitiveVerticesSize[i];
		}

		//Then create buffer with space for that many floats
		std::vector<float> verticeBuffer;

		//Using array instead
		std::vector<float> vertices;
		vertices.resize(totalVertFloats);
		auto verticesIterator = vertices.begin();
		unsigned int index = 0;

		//Add all the vertice data
		for (int i = 0; i < s_PrimitiveVertices.size(); i++) {
			const float* dataPointer = (const float*)s_PrimitiveVertices[i];
			unsigned int dataSize = s_PrimitiveVerticesSize[i];

			//Copy into vector
			std::copy(&dataPointer[0], &dataPointer[dataSize], verticesIterator + index);
			index += dataSize;
		}
		m_VB.bufferData(vertices.data(), vertices.size());
	}

	//next get amount of data among all indice ints
	unsigned int totalIndFloats = 0;
	for (unsigned int i = 0; i < s_PrimitiveIndices.size(); i++) {
		totalIndFloats += s_PrimitiveIndicesSize[i];
	}

	//Then create buffer with space for that many ints - TODO make able to reserve
	std::vector<unsigned int> indiceBuffer;

	std::vector<unsigned int> indices;
	indices.resize(totalIndFloats);
	auto indicesIterator = indices.begin();
	unsigned int index = 0;

	//Store last biggest indice
	unsigned int largestInd = 0;

	//Add all the indice data
	for (int i = 0; i < s_PrimitiveIndices.size(); i++) {
		const unsigned int* dataPointer = (const unsigned int*)s_PrimitiveIndices[i];
		unsigned int dataSize = s_PrimitiveIndicesSize[i];

		//Add to vector one by one
		unsigned int currentLargest = 0;
		for (int j = 0; j < dataSize; j++) {
			unsigned int newValue = dataPointer[j] + largestInd;
			indices[index] = newValue + i;
			index++;
			if (newValue > currentLargest) {
				currentLargest = newValue;
			}
		}
		largestInd = currentLargest;
	}
	m_IB.bufferData(indices.data(), indices.size());

	//clear list
	clearPrimitives();
}

//no longer has functionality
bool Renderer::isInBounds(const void* vert, unsigned int vertSize) {
	return true;
}

void Renderer::drawPrimitives(Shader& shader) {
	//buffer data
	bufferVideoData();

	//Use camera
	camera.sendCameraUniforms(shader);

	shader.bind();
	m_VA.bind();
	m_IB.bind();
	glDrawElements(GL_TRIANGLES, m_IB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
	shader.bind();
	va.bind();
	ib.bind();
	glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
}

//constant indices for a given supported shape
const unsigned int Renderer::s_TriIndices[] =
{
	0, 1, 2
};

const unsigned int Renderer::s_QuadIndices[] =
{
	0, 1, 2,
	2, 3, 0
};

const unsigned int Renderer::s_LineIndices[] =
{
	0, 1, 2,
	2, 3, 0
};

const unsigned int Renderer::s_CircleIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 3, 4,
	0, 4, 5,
	0, 5, 6,
	0, 6, 7,
	0, 7, 8,
	0, 8, 9,
	0, 9, 10,
	0, 10, 11,
	0, 11, 12,
	0, 12, 13,
	0, 13, 14,
	0, 14, 15,
	0, 15, 16,
	0, 16, 17,
	0, 17, 18,
	0, 18, 19, 
	0, 19, 20,
	0, 20, 21,
	0, 21, 22,
	0, 22, 23,
	0, 23, 24,
	0, 24, 25,
	0, 25, 26,
	0, 26, 27,
	0, 27, 28,
	0, 28, 29,
	0, 29, 30,
	0, 30, 31,
	0, 31, 32,
	0, 32, 33,
	0, 33, 34,
	0, 34, 35,
	0, 35, 36,
	0, 36, 37,
	0, 37, 38,
	0, 38, 39,
	0, 39, 40,
	0, 40, 41,
	0, 41, 42,
	0, 42, 43,
	0, 43, 44,
	0, 44, 45,
	0, 45, 46,
	0, 46, 47,
	0, 47, 48,
	0, 48, 49,
	0, 49, 50, 
	0, 50, 51,
	0, 51, 52,
	0, 52, 53,
	0, 53, 54,
	0, 54, 55,
	0, 55, 56,
	0, 56, 57,
	0, 57, 58,
	0, 58, 59,
	0, 59, 60,
	0, 60, 61,
	0, 61, 62,
	0, 62, 63,
	0, 63, 64
};
 


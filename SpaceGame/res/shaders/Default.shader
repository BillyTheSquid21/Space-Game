#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 color;

uniform mat4 u_Projection;

out vec3 vColor;

void main()
{	
	gl_Position = u_Projection * position;
	vColor = color;
}

#shader fragment
#version 330 core

in vec3 vColor;

out vec4 color;

void main()
{
	color = vec4(vColor, 1.0f);
}
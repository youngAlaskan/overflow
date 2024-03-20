#version 460 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in uint aWaterLevel;

void main()
{
	gl_Position = aPos;
}
#version 330 core

in VertexData
{
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} iVertexData;

out vec4 FragColor;

void main()
{
   FragColor = vec4(0.1, 0.7f, 0.2f, 1.0f);
}
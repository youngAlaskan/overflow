#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VertexData
{
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    float WaterLevel;
} inVertexData[];

out vec3 gNormal;
out vec4 gFragPos;
out vec2 gTexCoords;
out float gWaterLevel;

void main()
{
    for (int i = 0; i < 3; i++)
    {
        vec4 fragPos = inVertexData[i].FragPos;
        vec3 normal = inVertexData[i].Normal;
        vec2 texCoords = inVertexData[i].TexCoords;
        float waterLevel = inVertexData[i].WaterLevel;

        gNormal = normal;
        gFragPos = fragPos;
        gTexCoords = texCoords;
        gWaterLevel = waterLevel;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
}

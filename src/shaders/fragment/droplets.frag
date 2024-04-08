#version 330 core

in lowp float WorldHeight;
in vec2 ViewportCoord;
out vec4 FragColor;

const lowp float indexMat[16] = float[](
 0f, 16f,  4f, 20f,
24f,  8f, 28f, 12f,
 6f, 22f,  2f, 18f,
30f, 14f, 26f, 10f);

void main()
{
	ivec2 iFragCoord = ivec2(mod(gl_FragCoord, 4));
    lowp float idxVal = indexMat[(iFragCoord.x + iFragCoord.y * 4)];
	FragColor = vec4(0.1, 0.4, 0.8, clamp(idxVal - WorldHeight, 0.0f, 0.8f));
}
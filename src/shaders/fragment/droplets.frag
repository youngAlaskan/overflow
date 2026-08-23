#version 330 core

in lowp float WorldHeight;
in vec2 ViewportCoord;
out vec4 FragColor;

const lowp float indexMat[16] = float[](
 0.0f, 16.0f,  4.0f, 20.0f,
24.0f,  8.0f, 28.0f, 12.0f,
 6.0f, 22.0f,  2.0f, 18.0f,
30.0f, 14.0f, 26.0f, 10.0f);

void main()
{
	ivec2 iFragCoord = ivec2(mod(gl_FragCoord, 4));
    lowp float idxVal = indexMat[(iFragCoord.x + iFragCoord.y * 4)];
	FragColor = vec4(0.1, 0.4, 0.8, clamp(idxVal - WorldHeight, 0.0f, 0.8f));
}
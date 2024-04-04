#version 330 core

in VertexData
{
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} iVertexData;

vec3 green = vec3(0.3f, 0.9f, 0.1f);
vec3 blue = vec3(0.1f, 0.5f, 0.4f);

void main()
{
  vec3 N = normalize(iVertexData.Normal);
  vec3 L = normalize(-vec3(1.0f, -1.0f, 1.0f));

  // Lambert's cosine law
  float lambertian = max(dot(N, L), 0.0);

  // Get color by elevation
  float v = clamp(iVertexData.FragPos.y + 10.0, -2.0f, 2.0f) * 0.25 + 0.5;

  gl_FragColor = vec4(0.1f * blue + lambertian * mix(blue, green, v), 1.0);
}
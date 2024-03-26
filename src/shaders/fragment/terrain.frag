#version 330 core

in VertexData
{
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} iVertexData;

void main()
{
  vec3 N = normalize(iVertexData.Normal);
  vec3 L = normalize(-vec3(1.0f, -1.0f, 1.0f));

  // Lambert's cosine law
  float lambertian = max(dot(N, L), 0.0);

  gl_FragColor = vec4(0.1f * vec3(0.1f, 1.0f, 0.1f) + lambertian * vec3(0.1f, 1.0f, 0.1f), 1.0);
}
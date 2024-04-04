#version 330 core

in VertexData
{
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    float WaterLevel;
} iVertexData;

vec3 green = vec3(0.3f, 0.9f, 0.1f);
vec3 blue = vec3(0.1f, 0.5f, 0.4f);

uniform vec3 viewPos;

void main()
{
  vec3 N = normalize(iVertexData.Normal);
  vec3 L = normalize(vec3(1.0f, -1.0f, 1.0f));

  // Lambert's cosine law
  float lambertian = max(dot(N, L), 0.0);

  // Get color by elevation
  float v = clamp(iVertexData.FragPos.y + 10.0, -2.0f, 2.0f) * 0.25 + 0.5;

  vec3 terrainColor = vec3(0.1f * blue + lambertian * mix(blue, green, v));

  vec3 waterColor = vec3(0.0, 0.5, 0.8); // Water color
  // Blend terrain color with water color based on height
  float blendFactor = smoothstep(0.0, 0.1, iVertexData.WaterLevel); // Adjust the threshold and width of the blend
  vec3 finalColor = mix(terrainColor, waterColor, blendFactor);
    
  gl_FragColor = vec4(finalColor, 1.0);
}
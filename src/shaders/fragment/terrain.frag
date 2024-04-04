#version 330 core

in VertexData
{
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    float WaterLevel;
} inVertexData;

uniform vec3 viewPos;

void main()
{
    vec3 toSun = vec3(-1.0, 1.0, -1.0);
    vec3 waterColor = vec3(0.0, 0.5, 0.8); // Water color
    float shininess = 32.0; // Adjust shininess as needed
    
    vec3 normal = normalize(inVertexData.Normal);
    vec3 viewDir = normalize(viewPos - inVertexData.FragPos.xyz);
    vec3 lightDir = normalize(toSun);
    vec3 reflected = reflect(-lightDir, normal);
    
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflected), 0.0), shininess);
    
    vec3 ambient = 0.2 * waterColor;
    vec3 diffuse = 0.7 * diff * waterColor;
    vec3 specular = 0.3 * spec * waterColor;
    
    vec3 lighting = ambient + diffuse + specular;
    
    vec3 terrainColor = vec3(0.1, 1.0, 0.1);
    
    // Blend terrain color with water color based on height
    float blendFactor = smoothstep(0.0, 0.1, inVertexData.WaterLevel); // Adjust the threshold and width of the blend
    vec3 finalColor = mix(terrainColor, waterColor, blendFactor);
    
    gl_FragColor = vec4(finalColor * lighting, 1.0);
    // gl_FragColor = vec4(vec3(inVertexData.WaterLevel), 1.0);
}
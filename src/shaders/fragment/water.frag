#version 330 core

in vec3 vNormal;
out vec4 FragColor;

void main()
{
    vec3 n = normalize(vNormal);
    float diffuse = max(dot(n, normalize(vec3(0.4, 1.0, 0.3))), 0.0);
    vec3 base = vec3(0.1, 0.4, 0.8);   // same base color as the droplets
    FragColor = vec4(base * (0.35 + 0.65 * diffuse), 0.8);   // 0.8 = droplet max alpha
}
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D planetTexture;
uniform bool useTexture;
uniform vec3 objectColor;

uniform vec3 lightPos;      // Sun position (0,0,0)
uniform vec3 viewPos;       // Camera position
uniform vec3 lightColor;    // Sun light color

// Phong lighting parameters
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;

void main()
{
    // ── Ambient ────────────────────────────────────────────
    vec3 ambient = ambientStrength * lightColor;

    // ── Diffuse ────────────────────────────────────────────
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // ── Specular (Blinn-Phong) ─────────────────────────────
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // ── Combine with texture/color ───────────────────────────────
    vec3 baseColor = useTexture ? texture(planetTexture, TexCoord).rgb : objectColor;
    
    vec3 result = (ambient + diffuse + specular) * baseColor;

    FragColor = vec4(result, 1.0);
}

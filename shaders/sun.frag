#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D sunTexture;
uniform float time;
uniform float glowIntensity;

void main()
{
    vec3 texColor = texture(sunTexture, TexCoord).rgb;

    // Emissive glow effect - sun emits its own light
    float pulse = 1.0 + 0.05 * sin(time * 2.0);
    vec3 emissive = texColor * glowIntensity * pulse;

    // Slight rim glow
    vec3 viewDir = normalize(-FragPos);
    float rim = 1.0 - max(dot(normalize(Normal), viewDir), 0.0);
    rim = pow(rim, 3.0);
    vec3 rimColor = vec3(1.0, 0.6, 0.2) * rim * 0.5;

    FragColor = vec4(emissive + rimColor, 1.0);
}

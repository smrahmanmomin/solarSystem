#version 330 core
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D ringTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

// Stable 3D hash noise for granularity
float hash(vec3 p) {
    p = fract(p * 0.3183099 + 0.1);
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

void main() {
    // 1. Procedural Ring Density (Bypasses broken texture coordinates/files)
    float r = TexCoord.y; // 0.0 = inner radius, 1.0 = outer radius
    
    // Generate beautiful Saturn-like bands
    // Main A and B rings
    float density = smoothstep(0.0, 0.05, r) * (1.0 - smoothstep(0.4, 0.42, r)); // Inner B ring
    density += smoothstep(0.46, 0.48, r) * (1.0 - smoothstep(0.75, 0.78, r)) * 0.8; // Outer A ring
    // Faint C ring inside
    density += smoothstep(0.82, 0.83, r) * (1.0 - smoothstep(0.9, 0.95, r)) * 0.4; // F ring

    // If we are in the Cassini division or empty space, discard
    if (density < 0.05) discard;

    // 2. Base Color
    vec3 ringTint = vec3(0.85, 0.8, 0.7); // Icy dusty yellow/brown
    vec4 texColor = vec4(ringTint, density);

    // 3. Vectors
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);

    // 4. Stable Procedural Granularity (Ice chunks)
    float noise = hash(floor(FragPos * 60.0));
    float particleDensity = 0.5 + 0.5 * noise;
    float finalAlpha = texColor.a * particleDensity;

    // 5. Lighting - Two-Sided Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    float diffBack = max(dot(-norm, lightDir), 0.0);
    float finalDiff = max(diff, diffBack);

    // 6. Lighting - Forward Scattering (Mie approximation)
    float cosTheta = dot(viewDir, -lightDir);
    float scatter = pow(max(cosTheta, 0.0), 4.0);

    // 7. Color Composition
    vec3 baseColor = texColor.rgb * (0.7 + 0.5 * noise);
    
    vec3 ambient = 0.2 * lightColor; 
    vec3 diffuse = finalDiff * lightColor * 0.8;
    
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfDir), 0.0), 16.0);
    vec3 specular = spec * lightColor * 0.4 * noise;

    vec3 scatterColor = lightColor * scatter * 0.5 * finalAlpha;

    vec3 result = (ambient + diffuse) * baseColor + specular + scatterColor;

    FragColor = vec4(result, finalAlpha);
}

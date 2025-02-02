#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 chUV;

out vec4 FragColor;

uniform vec3 lightPos;   // Light position
uniform vec3 viewPos;    // Camera position
uniform vec3 lightColor; // Light color
uniform vec3 objectColor; // Object base color
uniform sampler2D texture1; // Texture sampler

void main()
{
    // Ambient lighting
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting
    float specularStrength = 0.4;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Apply texture
    vec3 texColor = texture(texture1, chUV).rgb;

    // Combine lighting results with texture color
    vec3 result = (ambient + diffuse + specular) * texColor;
    FragColor = vec4(result, 1.0);
}

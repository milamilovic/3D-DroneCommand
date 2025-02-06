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
uniform sampler2D specularMap; // Specular map
uniform bool specular;

struct Point {
    vec3 pos;
    vec3 col;
    float diffuseStrength;
};

uniform Point uPoints[6];

void main()
{

    //point light debugging - leave just this
    //float d = length(uPoints[5].pos - FragPos) / 10.0; // Normalize distance
    //FragColor = vec4(vec3(d), 1.0);

    // Ambient lighting
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting
    //TODO jer ne vidim da radi mada opet sto ne bi
    float specularStrength = 0.1;
    if (specular) {
        specularStrength = texture(specularMap, chUV).r;
    }
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular1 = specularStrength * spec * lightColor;

    // Apply texture
    vec3 texColor = texture(texture1, chUV).rgb;

    // Combine lighting results with texture color
    vec3 result = (ambient + diffuse + specular1) * texColor;

    //point lights
    //TODO
    for(int i = 0; i < 6; i++) {
        // Diffuse lighting
        float diffuseStrength = 0.5;
        vec3 lightDir = normalize(uPoints[i].pos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * uPoints[i].col * uPoints[i].diffuseStrength;

        // Specular lighting
        float specStrength = 0.3;
        if (specular) {
            specStrength = texture(specularMap, chUV).r;
        }
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
        vec3 specul = specStrength * spec * uPoints[i].col;

        result += 0.3 * (diffuse + specul) * texColor;
    }

    FragColor = vec4(result, 1.0);
}

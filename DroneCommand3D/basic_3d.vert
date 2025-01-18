    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;
    layout (location = 2) in vec2 inUV;

    uniform mat4 uM;  // Model matrix
    uniform mat4 uV;  // View matrix
    uniform mat4 uP;  // Projection matrix

    out vec3 FragPos;
    out vec3 Normal;
    out vec2 chUV;

    void main()
    {
        chUV = inUV;
        FragPos = vec3(uM * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(uM))) * aNormal; // Transform normals
        gl_Position = uP * uV * uM * vec4(FragPos, 1.0);
    }
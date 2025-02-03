// Autor: Mila Milovic

#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h> 
#include <GLFW/glfw3.h>

//GLM biblioteke
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "model.h"
#include "shader.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);

unsigned loadAndConfigureTexture(const char* filePath, unsigned shaderProgram, const char* uniformName, int textureUnit) {
    unsigned texture = loadImageToTexture(filePath);

    glBindTexture(GL_TEXTURE_2D, texture);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Set texture uniform
    glUseProgram(shaderProgram);
    unsigned uTexLoc = glGetUniformLocation(shaderProgram, uniformName);
    glUniform1i(uTexLoc, textureUnit);

    if (filePath == "res/static4.jpg" || filePath == "res/static5.jpg") {
        unsigned uTime = glGetUniformLocation(shaderProgram, "uTime");
        glUniform1f(uTime, float(glfwGetTime()));
    }

    glUseProgram(0);

    return texture;
}

void createVAO(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO,
    const float* vertices, size_t vertexSize,
    const unsigned int* indices, size_t indexSize,
    int vertexAttribCount, int stride, int texCoordOffset) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertices, GL_STATIC_DRAW);

    if (indices) { // if an EBO is needed
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indices, GL_STATIC_DRAW);
    }

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, vertexAttribCount, GL_FLOAT, GL_FALSE, stride, (void*)(texCoordOffset));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void drawDigit(int index, int digit, GLuint texture0, GLuint texture1, GLuint texture2, GLuint texture3, GLuint texture4, GLuint texture5, GLuint texture6, GLuint texture7, GLuint texture8, GLuint texture9, GLuint* vao) {
    GLuint texture;

    switch (digit) {
    case 0: texture = texture0; break;
    case 1: texture = texture1; break;
    case 2: texture = texture2; break;
    case 3: texture = texture3; break;
    case 4: texture = texture4; break;
    case 5: texture = texture5; break;
    case 6: texture = texture6; break;
    case 7: texture = texture7; break;
    case 8: texture = texture8; break;
    case 9: texture = texture9; break;
    default: texture = texture0; // Fallback to 0 if digit is out of bounds
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao[index]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void drawBattery(int battery, GLuint texture0, GLuint texture1, GLuint texture2, GLuint texture3, GLuint texture4, GLuint texture5, GLuint texture6, GLuint texture7, GLuint texture8, GLuint texture9, GLuint* vao) {
    int hundreds = battery / 100;
    int tens = (battery / 10) % 10;
    int ones = battery % 10;

    if (hundreds != 0) {
        drawDigit(0, hundreds, texture0, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, vao);
    }
    drawDigit(1, tens, texture0, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, vao);
    drawDigit(2, ones, texture0, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, vao);
}

struct Drone {
    float x, y;
    float radius;
    float batteryLevel;
    bool active;
    bool destroyed;
    float height;
    bool cameraOn;
};
Drone drone1 = { -0.5f, -0.5f, 0.05f, 100.0f, false, false, 0, false };
Drone drone2 = { 0.5f, -0.5f, 0.05f, 100.0f, false, false, 0, false };

void updateDroneVertices(const Drone& drone, float vertices[], float aspectRatio) {
    const float centerX = drone.x;
    const float centerY = drone.y;
    const float radius = drone.radius + 0.0005 * drone.height;
    const int numPoints = 30;

    vertices[0] = drone.x;      // X
    vertices[1] = drone.y;      // Y
    vertices[2] = 0.102;  // R
    vertices[3] = 0.278;  // G
    vertices[4] = 0.149;  // B
    vertices[5] = 1.0;  // A

    for (int i = 0; i < numPoints; i++) {
        float theta = (2.07f * 3.1415f / numPoints) * i;
        float x = centerX + radius * cos(theta) * aspectRatio;
        float y = centerY + radius * sin(theta);

        int index = (i + 1) * 6;
        vertices[index] = x;
        vertices[index + 1] = y;
        vertices[index + 2] = 0.102; // Red
        vertices[index + 3] = 0.278; // Green
        vertices[index + 4] = 0.149; // Blue
        vertices[index + 5] = 1.0; // Alpha
    }
}

void updateProgressVertices(float x, float y, float width, float height, float percentage, float r, float g, float b, float progressBarVertices[]) {
    
    float rg = 0.184, gg = 0.22, bg = 0.196;

    float borderXOffset = 0.007f;
    float borderYOffset = 0.008f;

    float adjustedWidth = width - 2 * borderXOffset;

    float vertices[] = {
        // Background bar (gray)
        x, y, rg, gg, bg, 1.0f,
        x + width, y, rg, gg, bg, 1.0f,
        x + width, y - height, rg, gg, bg, 1.0f,
        x, y - height, rg, gg, bg, 1.0f,

        // Filled portion (based on percentage)
        x + borderXOffset, y - borderYOffset, r, g, b, 1.0f,
        x + borderXOffset + adjustedWidth * percentage, y - borderYOffset, r, g, b, 1.0f,
        x + borderXOffset + adjustedWidth * percentage, y - height + borderYOffset, r, g, b, 1.0f,
        x + borderXOffset, y - height + borderYOffset, r, g, b, 1.0f,
    };

    memcpy(progressBarVertices, vertices, sizeof(vertices));
}

struct NoFlyZone {
    float x, y, radius;
    bool dragging;
    bool resizing;
};
NoFlyZone noFlyZone = { 0.05f, -0.07f, 0.20f, false, false };

bool areClashing(float x1, float y1, float r1, float x2, float y2, float r2) {
    float dx = (x2 - x1) * 4 / 3;
    float dy = y2 - y1;
    float distance = std::sqrt(dx * dx + dy * dy);
    return distance <= (r1 + r2);
}

void updateNoFlyZoneVertices(float noFlyZoneVertices[], float aspectRatio) {
    const float centerX = noFlyZone.x;
    const float centerY = noFlyZone.y;
    const float radius = noFlyZone.radius;
    const int numPoints = 30;

    float R = noFlyZone.resizing ? 0.431f : 0.8f;
    float G = noFlyZone.resizing ? 0.031f : 0.0f;
    float B = noFlyZone.resizing ? 0.031f : 0.05f;

    // Set center point
    noFlyZoneVertices[0] = centerX;      // X
    noFlyZoneVertices[1] = centerY;      // Y
    noFlyZoneVertices[2] = R;  // R
    noFlyZoneVertices[3] = G;  // G
    noFlyZoneVertices[4] = B;  // B
    noFlyZoneVertices[5] = 0.5;  // A

    // Circle points
    for (int i = 0; i < numPoints; i++) {
        float theta = (2.07f * 3.1415f / numPoints) * i;
        float x = centerX + radius * cos(theta) * aspectRatio;
        float y = centerY + radius * sin(theta);

        int index = (i + 1) * 6;
        noFlyZoneVertices[index] = x;
        noFlyZoneVertices[index + 1] = y;
        noFlyZoneVertices[index + 2] = R; // Red
        noFlyZoneVertices[index + 3] = G; // Green
        noFlyZoneVertices[index + 4] = B; // Blue
        noFlyZoneVertices[index + 5] = 0.5; // Alpha
    }
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    if (noFlyZone.dragging) {
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        // Adjust for the top-right quarter viewport
        int viewportX = windowWidth / 2;
        int viewportY = 0;  // Top-right quarter starts from y = 0
        int viewportWidth = windowWidth / 2;
        int viewportHeight = windowHeight / 2;

        // Make xpos, ypos relative to the viewport
        xpos -= viewportX;
        ypos -= viewportY;

        // Normalize within the viewport (not whole screen)
        float normalizedX = (2.0f * xpos / viewportWidth - 1.0f);
        float normalizedY = -1.0f * (2.0f * ypos / viewportHeight - 1.0f);

        float initialRadius = 0.20f;
        const float minX = -0.87f + (noFlyZone.radius - initialRadius) * 0.75;
        const float maxX = 0.87f - (noFlyZone.radius - initialRadius) * 0.75;
        const float minY = -0.51f + noFlyZone.radius - initialRadius;
        const float maxY = 0.83f - noFlyZone.radius + initialRadius;

        if (normalizedX < minX) normalizedX = minX;
        if (normalizedX > maxX) normalizedX = maxX;
        if (normalizedY < minY) normalizedY = minY;
        if (normalizedY > maxY) normalizedY = maxY;

        noFlyZone.x = normalizedX;
        noFlyZone.y = normalizedY;
    }
}

bool isPointInCircle(float x, float y, float cx, float cy, float radius) {
    float newCx = 0.5 + cx / 2;
    float newCy = 0.5 + cy / 2;
    float dx = x - newCx;
    float dy = y - newCy;
    return (dx * dx + dy * dy) <= (radius * radius);
}


void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            int windowWidth, windowHeight;
            glfwGetWindowSize(window, &windowWidth, &windowHeight);

            float aspectRatio = static_cast<float>(windowWidth) / windowHeight;
            float normalizedX = (2.0f * xpos / windowWidth - 1.0f) * aspectRatio;
            float normalizedY = -1.0f * (2.0f * ypos / windowHeight - 1.0f);

            if (isPointInCircle(normalizedX, normalizedY, noFlyZone.x, noFlyZone.y, noFlyZone.radius)) {
                noFlyZone.dragging = true;
            }
        }
        else if (action == GLFW_RELEASE) {
            noFlyZone.dragging = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            int windowWidth, windowHeight;
            glfwGetWindowSize(window, &windowWidth, &windowHeight);

            float aspectRatio = static_cast<float>(windowWidth) / windowHeight;
            float normalizedX = (2.0f * xpos / windowWidth - 1.0f) * aspectRatio;
            float normalizedY = -1.0f * (2.0f * ypos / windowHeight - 1.0f);

            if (isPointInCircle(normalizedX, normalizedY, noFlyZone.x, noFlyZone.y, noFlyZone.radius)) {
                noFlyZone.resizing = true;
            }
        }
        else if (action == GLFW_RELEASE) {
            noFlyZone.resizing = false;
        }
    }
}

int main(void)
{


    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 1392;
    unsigned int wHeight = 1066;
    const char wTitle[] = "Drone Command 3D";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();


    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ PROMJENLJIVE I BAFERI +++++++++++++++++++++++++++++++++++++++++++++++++

    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");
    unsigned int textureShader = createShader("texture.vert", "texture.frag");
    unsigned int mapShader = createShader("map.vert", "map.frag");
    unsigned int staticShader1 = createShader("static1.vert", "static1.frag");
    unsigned int staticShader2 = createShader("static2.vert", "static2.frag");


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++            MODELI            +++++++++++++++++++++++++++++++++++++++++++++++++

    //2d
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    float noFlyZoneVertices[186];
    updateNoFlyZoneVertices(noFlyZoneVertices, 0.75);
    float droneVertices1[186];
    updateDroneVertices(drone1, droneVertices1, 0.75);
    float droneVertices2[186];
    updateDroneVertices(drone2, droneVertices2, 0.75);
    float progressVertices1[48];
    float progressVertices2[48];
    updateProgressVertices(-0.9f, -0.86f, 0.4f, 0.08f, drone1.batteryLevel / 100.0f, 0.329f, 0.612f, 0.404f, progressVertices1);
    updateProgressVertices(-0.9f, -0.86f, 0.6f, 0.08f, drone2.batteryLevel / 100.0f, 0.329f, 0.612f, 0.404f, progressVertices2);
    
    //3d
    Shader shaderProgram("basic_3d.vert", "basic_3d.frag"); // Adjust paths if needed
    Model droneModel("res/drone.obj");
    Model majevicaModel("res/majevicamala.obj");


    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++            2D INICIJALIZACIJA            +++++++++++++++++++++++++++++++++++++++++++++++++



    unsigned int stride = (2 + 2) * sizeof(float);

    //map
    float map[] = {
        // X    Y      S    T
        1.0f,  1.0f,  1.0f, 1.0f, // Top-right
        1.0f, -0.7f,  1.0f, 0.0f, // Bottom-right
       -1.0f, -0.7f,  0.0f, 0.0f, // Bottom-left
       -1.0f,  1.0f,  0.0f, 1.0f  // Top-left
    };
    unsigned int indices[] = {
        0, 3, 1, // First triangle
        1, 3, 2  // Second triangle
    };
    unsigned int VAO, VBO, EBO;
    createVAO(VAO, VBO, EBO, map, sizeof(map), indices, sizeof(indices), 2, stride, 2 * sizeof(float));


    //whole screen (for static)
    float staticVertexes[] = {
        // X    Y      S    T
        1.0f,  1.0f,  1.0f, 1.0f, // Top-right
        1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right
       -1.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
       -1.0f,  1.0f,  0.0f, 1.0f  // Top-left
    };
    unsigned int staticIndices[] = {
        0, 3, 1, // First triangle
        1, 3, 2  // Second triangle
    };
    unsigned int staticVAO, staticVBO, staticEBO;
    createVAO(staticVAO, staticVBO, staticEBO, staticVertexes, sizeof(staticVertexes), staticIndices, sizeof(staticIndices), 2, stride, 2 * sizeof(float));


    //no fly zone
    unsigned int noFlyZoneVAO, noFlyZoneVBO;
    createVAO(noFlyZoneVAO, noFlyZoneVBO, EBO, noFlyZoneVertices, sizeof(noFlyZoneVertices), nullptr, 0, 4, 6 * sizeof(float), 2 * sizeof(float));

    //drone 1
    unsigned int droneVAO1, droneVBO1;
    createVAO(droneVAO1, droneVBO1, EBO, droneVertices1, sizeof(droneVertices1), nullptr, 0, 4, 6 * sizeof(float), 2 * sizeof(float));


    //drone 2
    unsigned int droneVAO2, droneVBO2;
    createVAO(droneVAO2, droneVBO2, EBO, droneVertices2, sizeof(droneVertices2), nullptr, 0, 4, 6 * sizeof(float), 2 * sizeof(float));


    //progress bars
    unsigned int progressIndices1[] = {
        0, 2, 1, 2, 0, 3, // Background
        4, 6, 5, 6, 4, 7  // Filled portion
    };
    unsigned int progressIndices2[] = {
        0, 2, 1, 2, 0, 3, // Background
        4, 6, 5, 6, 4, 7  // Filled portion
    };
    unsigned int progressVAO1, progressVBO1, progressEBO1;
    unsigned int progressVAO2, progressVBO2, progressEBO2;
    createVAO(progressVAO1, progressVBO1, progressEBO1, progressVertices1, sizeof(progressVertices1), progressIndices1, sizeof(progressIndices1), 4, 6 * sizeof(float), 2 * sizeof(float));
    createVAO(progressVAO2, progressVBO2, progressEBO2, progressVertices2, sizeof(progressVertices2), progressIndices2, sizeof(progressIndices2), 4, 6 * sizeof(float), 2 * sizeof(float));

    //battery level 1
    float rectangleVertices11[] = {
        // Positions        // Texture Coords
        -0.73f, -0.76f,       0.0f, 1.0f,
        -0.73f, -0.80f,       0.0f, 0.0f,
        -0.71f, -0.80f,       1.0f, 0.0f,
        -0.71f, -0.76f,       1.0f, 1.0f
    };
    float rectangleVertices12[] = {
        // Positions        // Texture Coords
        -0.71f, -0.76f,       0.0f, 1.0f,
        -0.71f, -0.80f,       0.0f, 0.0f,
        -0.69f, -0.80f,       1.0f, 0.0f,
        -0.69f, -0.76f,       1.0f, 1.0f
    };
    float rectangleVertices13[] = {
        // Positions        // Texture Coords
        -0.69f, -0.76f,       0.0f, 1.0f,
        -0.69f, -0.80f,       0.0f, 0.0f,
        -0.67f, -0.80f,       1.0f, 0.0f,
        -0.67f, -0.76f,       1.0f, 1.0f
    };
    unsigned int batteryIndices1[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int batteryIndices2[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int batteryIndices3[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int batteryVao1[3], batteryVbo1[3], batteryEbo1[3];

    int texStride = 4 * sizeof(float);  // 2 floats for position and 2 for texture coords
    int texCoordOffset = 2 * sizeof(float);  // Texture coordinates start after the position
    createVAO(batteryVao1[0], batteryVbo1[0], batteryEbo1[0], rectangleVertices11, sizeof(rectangleVertices11), batteryIndices1, sizeof(batteryIndices1), 2, texStride, texCoordOffset);
    createVAO(batteryVao1[1], batteryVbo1[1], batteryEbo1[1], rectangleVertices12, sizeof(rectangleVertices12), batteryIndices2, sizeof(batteryIndices2), 2, texStride, texCoordOffset);
    createVAO(batteryVao1[2], batteryVbo1[2], batteryEbo1[2], rectangleVertices13, sizeof(rectangleVertices13), batteryIndices3, sizeof(batteryIndices3), 2, texStride, texCoordOffset);

    //battery level 2
    float rectangleVertices21[] = {
        // Positions        // Texture Coords
        -0.23f, -0.76f,       0.0f, 1.0f,
        -0.23f, -0.80f,       0.0f, 0.0f,
        -0.21f, -0.80f,       1.0f, 0.0f,
        -0.21f, -0.76f,       1.0f, 1.0f
    };
    float rectangleVertices22[] = {
        // Positions        // Texture Coords
        -0.21f, -0.76f,       0.0f, 1.0f,
        -0.21f, -0.80f,       0.0f, 0.0f,
        -0.19f, -0.80f,       1.0f, 0.0f,
        -0.19f, -0.76f,       1.0f, 1.0f
    };
    float rectangleVertices23[] = {
        // Positions        // Texture Coords
        -0.19f, -0.76f,       0.0f, 1.0f,
        -0.19f, -0.80f,       0.0f, 0.0f,
        -0.17f, -0.80f,       1.0f, 0.0f,
        -0.17f, -0.76f,       1.0f, 1.0f
    };
    unsigned int batteryIndices21[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int batteryIndices22[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int batteryIndices23[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int batteryVao2[3], batteryVbo2[3], batteryEbo2[3];
    createVAO(batteryVao2[0], batteryVbo2[0], batteryEbo2[0], rectangleVertices21, sizeof(rectangleVertices21), batteryIndices21, sizeof(batteryIndices21), 2, texStride, texCoordOffset);
    createVAO(batteryVao2[1], batteryVbo2[1], batteryEbo2[1], rectangleVertices22, sizeof(rectangleVertices22), batteryIndices22, sizeof(batteryIndices22), 2, texStride, texCoordOffset);
    createVAO(batteryVao2[2], batteryVbo2[2], batteryEbo2[2], rectangleVertices23, sizeof(rectangleVertices23), batteryIndices23, sizeof(batteryIndices23), 2, texStride, texCoordOffset);

    //coordinates 1 : X
    float coordinates1x[] = {
        // Positions        // Texture Coords
        -0.90f, -0.86f,       0.0f, 1.0f,
        -0.90f, -0.90f,       0.0f, 0.0f,
        -0.875f, -0.90f,       1.0f, 0.0f,
        -0.875f, -0.86f,       1.0f, 1.0f
    };
    float coordinates1dot1[] = {
        // Positions        // Texture Coords
        -0.87f, -0.87f,       0.0f, 1.0f,
        -0.87f, -0.875f,       0.0f, 0.0f,
        -0.875f, -0.875f,       1.0f, 0.0f,
        -0.875f, -0.87f,       1.0f, 1.0f
    };
    float coordinates1dot2[] = {
        // Positions        // Texture Coords
        -0.87f, -0.88f,       0.0f, 1.0f,
        -0.87f, -0.885f,       0.0f, 0.0f,
        -0.875f, -0.885f,       1.0f, 0.0f,
        -0.875f, -0.88f,       1.0f, 1.0f
    };
    float coordinates1minus[] = {
        // Positions        // Texture Coords
        -0.85f, -0.875f,       0.0f, 1.0f,
        -0.85f, -0.88f,       0.0f, 0.0f,
        -0.84f, -0.88f,       1.0f, 0.0f,
        -0.84f, -0.875f,       1.0f, 1.0f
    };
    float coordinates1num1[] = {
        // Positions        // Texture Coords
        -0.83f, -0.86f,       0.0f, 1.0f,
        -0.83f, -0.90f,       0.0f, 0.0f,
        -0.81f, -0.90f,       1.0f, 0.0f,
        -0.81f, -0.86f,       1.0f, 1.0f
    };
    float coordinates1dot[] = {
        // Positions        // Texture Coords
        -0.80f, -0.89f,       0.0f, 1.0f,
        -0.80f, -0.895f,       0.0f, 0.0f,
        -0.805f, -0.895f,       1.0f, 0.0f,
        -0.805f, -0.89f,       1.0f, 1.0f
    };
    float coordinates1num2[] = {
        // Positions        // Texture Coords
        -0.79f, -0.86f,       0.0f, 1.0f,
        -0.79f, -0.90f,       0.0f, 0.0f,
        -0.77f, -0.90f,       1.0f, 0.0f,
        -0.77f, -0.86f,       1.0f, 1.0f
    };
    float coordinates1num3[] = {
        // Positions        // Texture Coords
        -0.77f, -0.86f,       0.0f, 1.0f,
        -0.77f, -0.90f,       0.0f, 0.0f,
        -0.75f, -0.90f,       1.0f, 0.0f,
        -0.75f, -0.86f,       1.0f, 1.0f
    };
    unsigned int indices1x[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indices1dot1[] = {
        0, 3, 1,
        1, 3, 2
    };
    unsigned int indices1dot2[] = {
        0, 3, 1,
        1, 3, 2
    };
    unsigned int indices1minus[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indices1num1[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indices1dot[] = {
        0, 3, 1,
        1, 3, 2
    };
    unsigned int indices1num2[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indices1num3[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int coordinatesVao1[8], coordinatesVbo1[8], coordinatesEbo1[8];
    createVAO(coordinatesVao1[0], coordinatesVbo1[0], coordinatesEbo1[0], coordinates1x, sizeof(coordinates1x), indices1x, sizeof(indices1x), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao1[1], coordinatesVbo1[1], coordinatesEbo1[1], coordinates1dot1, sizeof(coordinates1dot1), indices1dot1, sizeof(indices1dot1), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao1[2], coordinatesVbo1[2], coordinatesEbo1[2], coordinates1dot2, sizeof(coordinates1dot2), indices1dot2, sizeof(indices1dot2), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao1[3], coordinatesVbo1[3], coordinatesEbo1[3], coordinates1minus, sizeof(coordinates1minus), indices1minus, sizeof(indices1minus), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao1[4], coordinatesVbo1[4], coordinatesEbo1[4], coordinates1num1, sizeof(coordinates1num1), indices1num1, sizeof(indices1num1), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao1[5], coordinatesVbo1[5], coordinatesEbo1[5], coordinates1dot, sizeof(coordinates1dot), indices1dot, sizeof(indices1dot), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao1[6], coordinatesVbo1[6], coordinatesEbo1[6], coordinates1num2, sizeof(coordinates1num2), indices1num2, sizeof(indices1num2), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao1[7], coordinatesVbo1[7], coordinatesEbo1[7], coordinates1num3, sizeof(coordinates1num3), indices1num3, sizeof(indices1num3), 2, texStride, texCoordOffset);
    

    //coordinates 1 : Y
    float coordinates1y[] = {
        // Positions        // Texture Coords
        -0.70f, -0.86f,       0.0f, 1.0f,
        -0.70f, -0.90f,       0.0f, 0.0f,
        -0.675f, -0.90f,       1.0f, 0.0f,
        -0.675f, -0.86f,       1.0f, 1.0f
    };
    float coordinates1dot1y[] = {
        // Positions        // Texture Coords
        -0.67f, -0.87f,       0.0f, 1.0f,
        -0.67f, -0.875f,       0.0f, 0.0f,
        -0.675f, -0.875f,       1.0f, 0.0f,
        -0.675f, -0.87f,       1.0f, 1.0f
    };
    float coordinates1dot2y[] = {
        // Positions        // Texture Coords
        -0.67f, -0.88f,       0.0f, 1.0f,
        -0.67f, -0.885f,       0.0f, 0.0f,
        -0.675f, -0.885f,       1.0f, 0.0f,
        -0.675f, -0.88f,       1.0f, 1.0f
    };
    float coordinates1minusy[] = {
        // Positions        // Texture Coords
        -0.65f, -0.875f,       0.0f, 1.0f,
        -0.65f, -0.88f,       0.0f, 0.0f,
        -0.64f, -0.88f,       1.0f, 0.0f,
        -0.64f, -0.875f,       1.0f, 1.0f
    };
    float coordinates1num1y[] = {
        // Positions        // Texture Coords
        -0.63f, -0.86f,       0.0f, 1.0f,
        -0.63f, -0.90f,       0.0f, 0.0f,
        -0.61f, -0.90f,       1.0f, 0.0f,
        -0.61f, -0.86f,       1.0f, 1.0f
    };
    float coordinates1doty[] = {
        // Positions        // Texture Coords
        -0.60f, -0.89f,       0.0f, 1.0f,
        -0.60f, -0.895f,       0.0f, 0.0f,
        -0.605f, -0.895f,       1.0f, 0.0f,
        -0.605f, -0.89f,       1.0f, 1.0f
    };
    float coordinates1num2y[] = {
        // Positions        // Texture Coords
        -0.59f, -0.86f,       0.0f, 1.0f,
        -0.59f, -0.90f,       0.0f, 0.0f,
        -0.57f, -0.90f,       1.0f, 0.0f,
        -0.57f, -0.86f,       1.0f, 1.0f
    };
    float coordinates1num3y[] = {
        // Positions        // Texture Coords
        -0.57f, -0.86f,       0.0f, 1.0f,
        -0.57f, -0.90f,       0.0f, 0.0f,
        -0.55f, -0.90f,       1.0f, 0.0f,
        -0.55f, -0.86f,       1.0f, 1.0f
    };
    unsigned int indices1y[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indices1dot1y[] = {
        0, 3, 1,
        1, 3, 2
    };
    unsigned int indices1dot2y[] = {
        0, 3, 1,
        1, 3, 2
    };
    unsigned int indices1minusy[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indices1num1y[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indices1doty[] = {
        0, 3, 1,
        1, 3, 2
    };
    unsigned int indices1num2y[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indices1num3y[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int coordinatesVao1y[8], coordinatesVbo1y[8], coordinatesEbo1y[8];
    createVAO(coordinatesVao1y[0], coordinatesVbo1y[0], coordinatesEbo1y[0], coordinates1y, sizeof(coordinates1y), indices1y, sizeof(indices1y), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao1y[1], coordinatesVbo1y[1], coordinatesEbo1y[1], coordinates1dot1y, sizeof(coordinates1dot1y), indices1dot1y, sizeof(indices1dot1y), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao1y[2], coordinatesVbo1y[2], coordinatesEbo1y[2], coordinates1dot2y, sizeof(coordinates1dot2y), indices1dot2y, sizeof(indices1dot2y), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao1y[3], coordinatesVbo1y[3], coordinatesEbo1y[3], coordinates1minusy, sizeof(coordinates1minusy), indices1minusy, sizeof(indices1minusy), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao1y[4], coordinatesVbo1y[4], coordinatesEbo1y[4], coordinates1num1y, sizeof(coordinates1num1y), indices1num1y, sizeof(indices1num1y), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao1y[5], coordinatesVbo1y[5], coordinatesEbo1y[5], coordinates1doty, sizeof(coordinates1doty), indices1doty, sizeof(indices1doty), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao1y[6], coordinatesVbo1y[6], coordinatesEbo1y[6], coordinates1num2y, sizeof(coordinates1num2y), indices1num2y, sizeof(indices1num2y), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao1y[7], coordinatesVbo1y[7], coordinatesEbo1y[7], coordinates1num3y, sizeof(coordinates1num3y), indices1num3y, sizeof(indices1num3y), 2, texStride, texCoordOffset);

    //coordinates 2 : X
    float coordinates2x[] = {
        // Positions        // Texture Coords
        -0.40f, -0.86f,       0.0f, 1.0f,
        -0.40f, -0.90f,       0.0f, 0.0f,
        -0.375f, -0.90f,       1.0f, 0.0f,
        -0.375f, -0.86f,       1.0f, 1.0f
    };
    float coordinates2dot1[] = {
        // Positions        // Texture Coords
        -0.37f, -0.87f,       0.0f, 1.0f,
        -0.37f, -0.875f,       0.0f, 0.0f,
        -0.375f, -0.875f,       1.0f, 0.0f,
        -0.375f, -0.87f,       1.0f, 1.0f
    };
    float coordinates2dot2[] = {
        // Positions        // Texture Coords
        -0.37f, -0.88f,       0.0f, 1.0f,
        -0.37f, -0.885f,       0.0f, 0.0f,
        -0.375f, -0.885f,       1.0f, 0.0f,
        -0.375f, -0.88f,       1.0f, 1.0f
    };
    float coordinates2minus[] = {
        // Positions        // Texture Coords
        -0.35f, -0.875f,       0.0f, 1.0f,
        -0.35f, -0.88f,       0.0f, 0.0f,
        -0.34f, -0.88f,       1.0f, 0.0f,
        -0.34f, -0.875f,       1.0f, 1.0f
    };
    float coordinates2num1[] = {
        // Positions        // Texture Coords
        -0.33f, -0.86f,       0.0f, 1.0f,
        -0.33f, -0.90f,       0.0f, 0.0f,
        -0.31f, -0.90f,       1.0f, 0.0f,
        -0.31f, -0.86f,       1.0f, 1.0f
    };
    float coordinates2dot[] = {
        // Positions        // Texture Coords
        -0.30f, -0.89f,       0.0f, 1.0f,
        -0.30f, -0.895f,       0.0f, 0.0f,
        -0.305f, -0.895f,       1.0f, 0.0f,
        -0.305f, -0.89f,       1.0f, 1.0f
    };
    float coordinates2num2[] = {
        // Positions        // Texture Coords
        -0.29f, -0.86f,       0.0f, 1.0f,
        -0.29f, -0.90f,       0.0f, 0.0f,
        -0.27f, -0.90f,       1.0f, 0.0f,
        -0.27f, -0.86f,       1.0f, 1.0f
    };
    float coordinates2num3[] = {
        // Positions        // Texture Coords
        -0.27f, -0.86f,       0.0f, 1.0f,
        -0.27f, -0.90f,       0.0f, 0.0f,
        -0.25f, -0.90f,       1.0f, 0.0f,
        -0.25f, -0.86f,       1.0f, 1.0f
    };
    unsigned int indices2x[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indices2dot1[] = {
        0, 3, 1,
        1, 3, 2
    };
    unsigned int indices2dot2[] = {
        0, 3, 1,
        1, 3, 2
    };
    unsigned int indices2minus[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indices2num1[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indices2dot[] = {
        0, 3, 1,
        1, 3, 2
    };
    unsigned int indices2num2[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indices2num3[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int coordinatesVao2[8], coordinatesVbo2[8], coordinatesEbo2[8];
    createVAO(coordinatesVao2[0], coordinatesVbo2[0], coordinatesEbo2[0], coordinates2x, sizeof(coordinates2x), indices2x, sizeof(indices2x), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao2[1], coordinatesVbo2[1], coordinatesEbo2[1], coordinates2dot1, sizeof(coordinates2dot1), indices2dot1, sizeof(indices2dot1), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao2[2], coordinatesVbo2[2], coordinatesEbo2[2], coordinates2dot2, sizeof(coordinates2dot2), indices2dot2, sizeof(indices2dot2), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao2[3], coordinatesVbo2[3], coordinatesEbo2[3], coordinates2minus, sizeof(coordinates2minus), indices2minus, sizeof(indices2minus), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao2[4], coordinatesVbo2[4], coordinatesEbo2[4], coordinates2num1, sizeof(coordinates2num1), indices2num1, sizeof(indices2num1), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao2[5], coordinatesVbo2[5], coordinatesEbo2[5], coordinates2dot, sizeof(coordinates2dot), indices2dot, sizeof(indices2dot), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao2[6], coordinatesVbo2[6], coordinatesEbo2[6], coordinates2num2, sizeof(coordinates2num2), indices2num2, sizeof(indices2num2), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao2[7], coordinatesVbo2[7], coordinatesEbo2[7], coordinates2num3, sizeof(coordinates2num3), indices2num3, sizeof(indices2num3), 2, texStride, texCoordOffset);


    //coordinates 2 : Y
    float coordinates2y[] = {
        // Positions        // Texture Coords
        -0.20f, -0.86f,       0.0f, 1.0f,
        -0.20f, -0.90f,       0.0f, 0.0f,
        -0.175f, -0.90f,       1.0f, 0.0f,
        -0.175f, -0.86f,       1.0f, 1.0f
    };
    float coordinates2dot1y[] = {
        // Positions        // Texture Coords
        -0.17f, -0.87f,       0.0f, 1.0f,
        -0.17f, -0.875f,       0.0f, 0.0f,
        -0.175f, -0.875f,       1.0f, 0.0f,
        -0.175f, -0.87f,       1.0f, 1.0f
    };
    float coordinates2dot2y[] = {
        // Positions        // Texture Coords
        -0.17f, -0.88f,       0.0f, 1.0f,
        -0.17f, -0.885f,       0.0f, 0.0f,
        -0.175f, -0.885f,       1.0f, 0.0f,
        -0.175f, -0.88f,       1.0f, 1.0f
    };
    float coordinates2minusy[] = {
        // Positions        // Texture Coords
        -0.15f, -0.875f,       0.0f, 1.0f,
        -0.15f, -0.88f,       0.0f, 0.0f,
        -0.14f, -0.88f,       1.0f, 0.0f,
        -0.14f, -0.875f,       1.0f, 1.0f
    };
    float coordinates2num1y[] = {
        // Positions        // Texture Coords
        -0.13f, -0.86f,       0.0f, 1.0f,
        -0.13f, -0.90f,       0.0f, 0.0f,
        -0.11f, -0.90f,       1.0f, 0.0f,
        -0.11f, -0.86f,       1.0f, 1.0f
    };
    float coordinates2doty[] = {
        // Positions        // Texture Coords
        -0.10f, -0.89f,       0.0f, 1.0f,
        -0.10f, -0.895f,       0.0f, 0.0f,
        -0.105f, -0.895f,       1.0f, 0.0f,
        -0.105f, -0.89f,       1.0f, 1.0f
    };
    float coordinates2num2y[] = {
        // Positions        // Texture Coords
        -0.09f, -0.86f,       0.0f, 1.0f,
        -0.09f, -0.90f,       0.0f, 0.0f,
        -0.07f, -0.90f,       1.0f, 0.0f,
        -0.07f, -0.86f,       1.0f, 1.0f
    };
    float coordinates2num3y[] = {
        // Positions        // Texture Coords
        -0.07f, -0.86f,       0.0f, 1.0f,
        -0.07f, -0.90f,       0.0f, 0.0f,
        -0.05f, -0.90f,       1.0f, 0.0f,
        -0.05f, -0.86f,       1.0f, 1.0f
    };
    unsigned int indices2y[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indices2dot1y[] = {
        0, 3, 1,
        1, 3, 2
    };
    unsigned int indices2dot2y[] = {
        0, 3, 1,
        1, 3, 2
    };
    unsigned int indices2minusy[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indices2num1y[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indices2doty[] = {
        0, 3, 1,
        1, 3, 2
    };
    unsigned int indices2num2y[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indices2num3y[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int coordinatesVao2y[8], coordinatesVbo2y[8], coordinatesEbo2y[8];
    createVAO(coordinatesVao2y[0], coordinatesVbo2y[0], coordinatesEbo2y[0], coordinates2y, sizeof(coordinates2y), indices2y, sizeof(indices2y), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao2y[1], coordinatesVbo2y[1], coordinatesEbo2y[1], coordinates2dot1y, sizeof(coordinates2dot1y), indices2dot1y, sizeof(indices2dot1y), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao2y[2], coordinatesVbo2y[2], coordinatesEbo2y[2], coordinates2dot2y, sizeof(coordinates2dot2y), indices2dot2y, sizeof(indices2dot2y), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao2y[3], coordinatesVbo2y[3], coordinatesEbo2y[3], coordinates2minusy, sizeof(coordinates2minusy), indices2minusy, sizeof(indices2minusy), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao2y[4], coordinatesVbo2y[4], coordinatesEbo2y[4], coordinates2num1y, sizeof(coordinates2num1y), indices2num1y, sizeof(indices2num1y), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao2y[5], coordinatesVbo2y[5], coordinatesEbo2y[5], coordinates2doty, sizeof(coordinates2doty), indices2doty, sizeof(indices2doty), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao2y[6], coordinatesVbo2y[6], coordinatesEbo2y[6], coordinates2num2y, sizeof(coordinates2num2y), indices2num2y, sizeof(indices2num2y), 2, texStride, texCoordOffset);
    createVAO(coordinatesVao2y[7], coordinatesVbo2y[7], coordinatesEbo2y[7], coordinates2num3y, sizeof(coordinates2num3y), indices2num3y, sizeof(indices2num3y), 2, texStride, texCoordOffset);


    // IME PREZIME I INDEKS
    float name1[] = {
        // Positions        // Texture Coords
        0.60f, -0.92f,       0.0f, 1.0f,
        0.60f, -0.96f,       0.0f, 0.0f,
        0.62f, -0.96f,       1.0f, 0.0f,
        0.62f, -0.92f,       1.0f, 1.0f
    };
    float name2[] = {
        // Positions        // Texture Coords
        0.62f, -0.92f,       0.0f, 1.0f,
        0.62f, -0.96f,       0.0f, 0.0f,
        0.63f, -0.96f,       1.0f, 0.0f,
        0.63f, -0.92f,       1.0f, 1.0f
    };
    float name3[] = {
        // Positions        // Texture Coords
        0.63f, -0.92f,       0.0f, 1.0f,
        0.63f, -0.96f,       0.0f, 0.0f,
        0.65f, -0.96f,       1.0f, 0.0f,
        0.65f, -0.92f,       1.0f, 1.0f
    };
    float name4[] = {
        // Positions        // Texture Coords
        0.65f, -0.92f,       0.0f, 1.0f,
        0.65f, -0.96f,       0.0f, 0.0f,
        0.67f, -0.96f,       1.0f, 0.0f,
        0.67f, -0.92f,       1.0f, 1.0f
    };
    float surname1[] = {
        // Positions        // Texture Coords
        0.69f, -0.92f,       0.0f, 1.0f,
        0.69f, -0.96f,       0.0f, 0.0f,
        0.71f, -0.96f,       1.0f, 0.0f,
        0.71f, -0.92f,       1.0f, 1.0f
    };
    float surname2[] = {
        // Positions        // Texture Coords
        0.71f, -0.92f,       0.0f, 1.0f,
        0.71f, -0.96f,       0.0f, 0.0f,
        0.72f, -0.96f,       1.0f, 0.0f,
        0.72f, -0.92f,       1.0f, 1.0f
    };
    float surname3[] = {
        // Positions        // Texture Coords
        0.72f, -0.92f,       0.0f, 1.0f,
        0.72f, -0.96f,       0.0f, 0.0f,
        0.74f, -0.96f,       1.0f, 0.0f,
        0.74f, -0.92f,       1.0f, 1.0f
    };
    float surname4[] = {
        // Positions        // Texture Coords
        0.74f, -0.92f,       0.0f, 1.0f,
        0.74f, -0.96f,       0.0f, 0.0f,
        0.76f, -0.96f,       1.0f, 0.0f,
        0.76f, -0.92f,       1.0f, 1.0f
    };
    float surname5[] = {
        // Positions        // Texture Coords
        0.76f, -0.92f,       0.0f, 1.0f,
        0.76f, -0.96f,       0.0f, 0.0f,
        0.78f, -0.96f,       1.0f, 0.0f,
        0.78f, -0.92f,       1.0f, 1.0f
    };
    float surname6[] = {
        // Positions        // Texture Coords
        0.78f, -0.92f,       0.0f, 1.0f,
        0.78f, -0.96f,       0.0f, 0.0f,
        0.79f, -0.96f,       1.0f, 0.0f,
        0.79f, -0.92f,       1.0f, 1.0f
    };
    float surname7[] = {
        // Positions        // Texture Coords
        0.79f, -0.91f,       0.0f, 1.0f,
        0.79f, -0.96f,       0.0f, 0.0f,
        0.81f, -0.96f,       1.0f, 0.0f,
        0.81f, -0.91f,       1.0f, 1.0f
    };
    float indeks1[] = {
        // Positions        // Texture Coords
        0.83f, -0.92f,       0.0f, 1.0f,
        0.83f, -0.96f,       0.0f, 0.0f,
        0.85f, -0.96f,       1.0f, 0.0f,
        0.85f, -0.92f,       1.0f, 1.0f
    };
    float indeks2[] = {
        // Positions        // Texture Coords
        0.85f, -0.92f,       0.0f, 1.0f,
        0.85f, -0.96f,       0.0f, 0.0f,
        0.87f, -0.96f,       1.0f, 0.0f,
        0.87f, -0.92f,       1.0f, 1.0f
    };
    float indeks3[] = {
        // Positions        // Texture Coords
        0.87f, -0.92f,       0.0f, 1.0f,
        0.87f, -0.96f,       0.0f, 0.0f,
        0.89f, -0.96f,       1.0f, 0.0f,
        0.89f, -0.92f,       1.0f, 1.0f
    };
    float indeks4[] = {
        // Positions        // Texture Coords
        0.89f, -0.92f,       0.0f, 1.0f,
        0.89f, -0.96f,       0.0f, 0.0f,
        0.91f, -0.96f,       1.0f, 0.0f,
        0.91f, -0.92f,       1.0f, 1.0f
    };
    float indeks5[] = {
        // Positions        // Texture Coords
        0.91f, -0.94f,       0.0f, 1.0f,
        0.91f, -0.945f,       0.0f, 0.0f,
        0.92f, -0.945f,       1.0f, 0.0f,
        0.92f, -0.94f,       1.0f, 1.0f
    };
    float indeks6[] = {
        // Positions        // Texture Coords
        0.92f, -0.92f,       0.0f, 1.0f,
        0.92f, -0.96f,       0.0f, 0.0f,
        0.94f, -0.96f,       1.0f, 0.0f,
        0.94f, -0.92f,       1.0f, 1.0f
    };
    float indeks7[] = {
        // Positions        // Texture Coords
        0.94f, -0.92f,       0.0f, 1.0f,
        0.94f, -0.96f,       0.0f, 0.0f,
        0.96f, -0.96f,       1.0f, 0.0f,
        0.96f, -0.92f,       1.0f, 1.0f
    };
    float indeks8[] = {
        // Positions        // Texture Coords
        0.96f, -0.92f,       0.0f, 1.0f,
        0.96f, -0.96f,       0.0f, 0.0f,
        0.98f, -0.96f,       1.0f, 0.0f,
        0.98f, -0.92f,       1.0f, 1.0f
    };
    float indeks9[] = {
        // Positions        // Texture Coords
        0.98f, -0.92f,       0.0f, 1.0f,
        0.98f, -0.96f,       0.0f, 0.0f,
        1.00f, -0.96f,       1.0f, 0.0f,
        1.00f, -0.92f,       1.0f, 1.0f
    };
    unsigned int nameIndices1[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int nameIndices2[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int nameIndices3[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int nameIndices4[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int surnameIndices1[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int surnameIndices2[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int surnameIndices3[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int surnameIndices4[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int surnameIndices5[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int surnameIndices6[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int surnameIndices7[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indexIndices1[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indexIndices2[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indexIndices3[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indexIndices4[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indexIndices5[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indexIndices6[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indexIndices7[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indexIndices8[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int indexIndices9[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int nameVao[11], nameVbo[11], nameEbo[11];
    createVAO(nameVao[0], nameVbo[0], nameEbo[0], name1, sizeof(name1), nameIndices1, sizeof(nameIndices1), 2, texStride, texCoordOffset);
    createVAO(nameVao[1], nameVbo[1], nameEbo[1], name2, sizeof(name2), nameIndices2, sizeof(nameIndices2), 2, texStride, texCoordOffset);
    createVAO(nameVao[2], nameVbo[2], nameEbo[2], name3, sizeof(name3), nameIndices3, sizeof(nameIndices3), 2, texStride, texCoordOffset);
    createVAO(nameVao[3], nameVbo[3], nameEbo[3], name4, sizeof(name4), nameIndices4, sizeof(nameIndices4), 2, texStride, texCoordOffset);
    createVAO(nameVao[4], nameVbo[4], nameEbo[4], surname1, sizeof(surname1), surnameIndices1, sizeof(surnameIndices1), 2, texStride, texCoordOffset);
    createVAO(nameVao[5], nameVbo[5], nameEbo[5], surname2, sizeof(surname2), surnameIndices2, sizeof(surnameIndices2), 2, texStride, texCoordOffset);
    createVAO(nameVao[6], nameVbo[6], nameEbo[6], surname3, sizeof(surname3), surnameIndices3, sizeof(surnameIndices3), 2, texStride, texCoordOffset);
    createVAO(nameVao[7], nameVbo[7], nameEbo[7], surname4, sizeof(surname4), surnameIndices4, sizeof(surnameIndices4), 2, texStride, texCoordOffset);
    createVAO(nameVao[8], nameVbo[8], nameEbo[8], surname5, sizeof(surname5), surnameIndices5, sizeof(surnameIndices5), 2, texStride, texCoordOffset);
    createVAO(nameVao[9], nameVbo[9], nameEbo[9], surname6, sizeof(surname6), surnameIndices6, sizeof(surnameIndices6), 2, texStride, texCoordOffset);
    createVAO(nameVao[10], nameVbo[10], nameEbo[10], surname7, sizeof(surname7), surnameIndices7, sizeof(surnameIndices7), 2, texStride, texCoordOffset);

    unsigned int indexVao[9], indexVbo[9], indexEbo[9];
    createVAO(indexVao[0], indexVbo[0], indexEbo[0], indeks1, sizeof(indeks1), indexIndices1, sizeof(indexIndices1), 2, texStride, texCoordOffset);
    createVAO(indexVao[1], indexVbo[1], indexEbo[1], indeks2, sizeof(indeks2), indexIndices2, sizeof(indexIndices2), 2, texStride, texCoordOffset);
    createVAO(indexVao[2], indexVbo[2], indexEbo[2], indeks3, sizeof(indeks3), indexIndices3, sizeof(indexIndices3), 2, texStride, texCoordOffset);
    createVAO(indexVao[3], indexVbo[3], indexEbo[3], indeks4, sizeof(indeks4), indexIndices4, sizeof(indexIndices4), 2, texStride, texCoordOffset);
    createVAO(indexVao[4], indexVbo[4], indexEbo[4], indeks5, sizeof(indeks5), indexIndices5, sizeof(indexIndices5), 2, texStride, texCoordOffset);
    createVAO(indexVao[5], indexVbo[5], indexEbo[5], indeks6, sizeof(indeks6), indexIndices6, sizeof(indexIndices6), 2, texStride, texCoordOffset);
    createVAO(indexVao[6], indexVbo[6], indexEbo[6], indeks7, sizeof(indeks7), indexIndices7, sizeof(indexIndices7), 2, texStride, texCoordOffset);
    createVAO(indexVao[7], indexVbo[7], indexEbo[7], indeks8, sizeof(indeks8), indexIndices8, sizeof(indexIndices8), 2, texStride, texCoordOffset);
    createVAO(indexVao[8], indexVbo[8], indexEbo[8], indeks9, sizeof(indeks9), indexIndices9, sizeof(indexIndices9), 2, texStride, texCoordOffset);

    //destroyed drone 1
    float destroyed11[] = {
        // Positions        // Texture Coords
        -0.90f, -0.94f,       0.0f, 1.0f,
        -0.90f, -0.98f,       0.0f, 0.0f,
        -0.88f, -0.98f,       1.0f, 0.0f,
        -0.88f, -0.94f,       1.0f, 1.0f
    };
    float destroyed12[] = {
        // Positions        // Texture Coords
        -0.88f, -0.94f,       0.0f, 1.0f,
        -0.88f, -0.98f,       0.0f, 0.0f,
        -0.86f, -0.98f,       1.0f, 0.0f,
        -0.86f, -0.94f,       1.0f, 1.0f
    };
    float destroyed13[] = {
        // Positions        // Texture Coords
        -0.86f, -0.94f,       0.0f, 1.0f,
        -0.86f, -0.98f,       0.0f, 0.0f,
        -0.84f, -0.98f,       1.0f, 0.0f,
        -0.84f, -0.94f,       1.0f, 1.0f
    };
    float destroyed14[] = {
        // Positions        // Texture Coords
        -0.84f, -0.94f,       0.0f, 1.0f,
        -0.84f, -0.98f,       0.0f, 0.0f,
        -0.82f, -0.98f,       1.0f, 0.0f,
        -0.82f, -0.94f,       1.0f, 1.0f
    };
    float destroyed15[] = {
        // Positions        // Texture Coords
        -0.82f, -0.94f,       0.0f, 1.0f,
        -0.82f, -0.98f,       0.0f, 0.0f,
        -0.80f, -0.98f,       1.0f, 0.0f,
        -0.80f, -0.94f,       1.0f, 1.0f
    };
    float destroyed16[] = {
        // Positions        // Texture Coords
        -0.80f, -0.94f,       0.0f, 1.0f,
        -0.80f, -0.98f,       0.0f, 0.0f,
        -0.78f, -0.98f,       1.0f, 0.0f,
        -0.78f, -0.94f,       1.0f, 1.0f
    };
    float destroyed17[] = {
        // Positions        // Texture Coords
        -0.78f, -0.94f,       0.0f, 1.0f,
        -0.78f, -0.98f,       0.0f, 0.0f,
        -0.76f, -0.98f,       1.0f, 0.0f,
        -0.76f, -0.94f,       1.0f, 1.0f
    };
    float destroyed18[] = {
        // Positions        // Texture Coords
        -0.76f, -0.94f,       0.0f, 1.0f,
        -0.76f, -0.98f,       0.0f, 0.0f,
        -0.74f, -0.98f,       1.0f, 0.0f,
        -0.74f, -0.94f,       1.0f, 1.0f
    };
    float destroyed19[] = {
        // Positions        // Texture Coords
        -0.74f, -0.94f,       0.0f, 1.0f,
        -0.74f, -0.98f,       0.0f, 0.0f,
        -0.72f, -0.98f,       1.0f, 0.0f,
        -0.72f, -0.94f,       1.0f, 1.0f
    };
    unsigned int destroyedindices11[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedindices12[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedindices13[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedindices14[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedindices15[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedindices16[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedindices17[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedindices18[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedindices19[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedVao1[9], destroyedVbo1[9], destroyedEbo1[9];
    createVAO(destroyedVao1[0], destroyedVbo1[0], destroyedEbo1[0], destroyed11, sizeof(destroyed11), destroyedindices11, sizeof(destroyedindices11), 2, texStride, texCoordOffset);
    createVAO(destroyedVao1[1], destroyedVbo1[1], destroyedEbo1[1], destroyed12, sizeof(destroyed12), destroyedindices12, sizeof(destroyedindices12), 2, texStride, texCoordOffset);
    createVAO(destroyedVao1[2], destroyedVbo1[2], destroyedEbo1[2], destroyed13, sizeof(destroyed13), destroyedindices13, sizeof(destroyedindices13), 2, texStride, texCoordOffset);
    createVAO(destroyedVao1[3], destroyedVbo1[3], destroyedEbo1[3], destroyed14, sizeof(destroyed14), destroyedindices14, sizeof(destroyedindices14), 2, texStride, texCoordOffset);
    createVAO(destroyedVao1[4], destroyedVbo1[4], destroyedEbo1[4], destroyed15, sizeof(destroyed15), destroyedindices15, sizeof(destroyedindices15), 2, texStride, texCoordOffset);
    createVAO(destroyedVao1[5], destroyedVbo1[5], destroyedEbo1[5], destroyed16, sizeof(destroyed16), destroyedindices16, sizeof(destroyedindices16), 2, texStride, texCoordOffset);
    createVAO(destroyedVao1[6], destroyedVbo1[6], destroyedEbo1[6], destroyed17, sizeof(destroyed17), destroyedindices17, sizeof(destroyedindices17), 2, texStride, texCoordOffset);
    createVAO(destroyedVao1[7], destroyedVbo1[7], destroyedEbo1[7], destroyed18, sizeof(destroyed18), destroyedindices18, sizeof(destroyedindices18), 2, texStride, texCoordOffset);
    createVAO(destroyedVao1[8], destroyedVbo1[8], destroyedEbo1[8], destroyed19, sizeof(destroyed19), destroyedindices19, sizeof(destroyedindices19), 2, texStride, texCoordOffset);


    //destroyed drone 1
    float destroyed21[] = {
        // Positions        // Texture Coords
        -0.40f, -0.94f,       0.0f, 1.0f,
        -0.40f, -0.98f,       0.0f, 0.0f,
        -0.38f, -0.98f,       1.0f, 0.0f,
        -0.38f, -0.94f,       1.0f, 1.0f
    };
    float destroyed22[] = {
        // Positions        // Texture Coords
        -0.38f, -0.94f,       0.0f, 1.0f,
        -0.38f, -0.98f,       0.0f, 0.0f,
        -0.36f, -0.98f,       1.0f, 0.0f,
        -0.36f, -0.94f,       1.0f, 1.0f
    };
    float destroyed23[] = {
        // Positions        // Texture Coords
        -0.36f, -0.94f,       0.0f, 1.0f,
        -0.36f, -0.98f,       0.0f, 0.0f,
        -0.34f, -0.98f,       1.0f, 0.0f,
        -0.34f, -0.94f,       1.0f, 1.0f
    };
    float destroyed24[] = {
        // Positions        // Texture Coords
        -0.34f, -0.94f,       0.0f, 1.0f,
        -0.34f, -0.98f,       0.0f, 0.0f,
        -0.32f, -0.98f,       1.0f, 0.0f,
        -0.32f, -0.94f,       1.0f, 1.0f
    };
    float destroyed25[] = {
        // Positions        // Texture Coords
        -0.32f, -0.94f,       0.0f, 1.0f,
        -0.32f, -0.98f,       0.0f, 0.0f,
        -0.30f, -0.98f,       1.0f, 0.0f,
        -0.30f, -0.94f,       1.0f, 1.0f
    };
    float destroyed26[] = {
        // Positions        // Texture Coords
        -0.30f, -0.94f,       0.0f, 1.0f,
        -0.30f, -0.98f,       0.0f, 0.0f,
        -0.28f, -0.98f,       1.0f, 0.0f,
        -0.28f, -0.94f,       1.0f, 1.0f
    };
    float destroyed27[] = {
        // Positions        // Texture Coords
        -0.28f, -0.94f,       0.0f, 1.0f,
        -0.28f, -0.98f,       0.0f, 0.0f,
        -0.26f, -0.98f,       1.0f, 0.0f,
        -0.26f, -0.94f,       1.0f, 1.0f
    };
    float destroyed28[] = {
        // Positions        // Texture Coords
        -0.26f, -0.94f,       0.0f, 1.0f,
        -0.26f, -0.98f,       0.0f, 0.0f,
        -0.24f, -0.98f,       1.0f, 0.0f,
        -0.24f, -0.94f,       1.0f, 1.0f
    };
    float destroyed29[] = {
        // Positions        // Texture Coords
        -0.24f, -0.94f,       0.0f, 1.0f,
        -0.24f, -0.98f,       0.0f, 0.0f,
        -0.22f, -0.98f,       1.0f, 0.0f,
        -0.22f, -0.94f,       1.0f, 1.0f
    };
    unsigned int destroyedindices21[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedindices22[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedindices23[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedindices24[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedindices25[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedindices26[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedindices27[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedindices28[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedindices29[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int destroyedVao2[9], destroyedVbo2[9], destroyedEbo2[9];
    createVAO(destroyedVao2[0], destroyedVbo2[0], destroyedEbo2[0], destroyed21, sizeof(destroyed21), destroyedindices21, sizeof(destroyedindices21), 2, texStride, texCoordOffset);
    createVAO(destroyedVao2[1], destroyedVbo2[1], destroyedEbo2[1], destroyed22, sizeof(destroyed22), destroyedindices22, sizeof(destroyedindices22), 2, texStride, texCoordOffset);
    createVAO(destroyedVao2[2], destroyedVbo2[2], destroyedEbo2[2], destroyed23, sizeof(destroyed23), destroyedindices23, sizeof(destroyedindices23), 2, texStride, texCoordOffset);
    createVAO(destroyedVao2[3], destroyedVbo2[3], destroyedEbo2[3], destroyed24, sizeof(destroyed24), destroyedindices24, sizeof(destroyedindices24), 2, texStride, texCoordOffset);
    createVAO(destroyedVao2[4], destroyedVbo2[4], destroyedEbo2[4], destroyed25, sizeof(destroyed25), destroyedindices25, sizeof(destroyedindices25), 2, texStride, texCoordOffset);
    createVAO(destroyedVao2[5], destroyedVbo2[5], destroyedEbo2[5], destroyed26, sizeof(destroyed26), destroyedindices26, sizeof(destroyedindices26), 2, texStride, texCoordOffset);
    createVAO(destroyedVao2[6], destroyedVbo2[6], destroyedEbo2[6], destroyed27, sizeof(destroyed27), destroyedindices27, sizeof(destroyedindices27), 2, texStride, texCoordOffset);
    createVAO(destroyedVao2[7], destroyedVbo2[7], destroyedEbo2[7], destroyed28, sizeof(destroyed28), destroyedindices28, sizeof(destroyedindices28), 2, texStride, texCoordOffset);
    createVAO(destroyedVao2[8], destroyedVbo2[8], destroyedEbo2[8], destroyed29, sizeof(destroyed29), destroyedindices29, sizeof(destroyedindices29), 2, texStride, texCoordOffset);


    //led
    float ledVertices[] =
    {
        -0.93, -0.78,    0.184f, 0.22f, 0.196f, 1.0,
        -0.43, -0.78,    0.184f, 0.22f, 0.196f, 1.0
    };
    stride = 6 * sizeof(float);
    unsigned int ledVAO;
    glGenVertexArrays(1, &ledVAO);
    glBindVertexArray(ledVAO);
    unsigned int ledVBO;
    glGenBuffers(1, &ledVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ledVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ledVertices), ledVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // textures
    unsigned texture1 = loadAndConfigureTexture("res/1.png", textureShader, "uTex", 0);
    unsigned texture2 = loadAndConfigureTexture("res/2.png", textureShader, "uTex", 0);
    unsigned texture3 = loadAndConfigureTexture("res/3.png", textureShader, "uTex", 0);
    unsigned texture4 = loadAndConfigureTexture("res/4.png", textureShader, "uTex", 0);
    unsigned texture5 = loadAndConfigureTexture("res/5.png", textureShader, "uTex", 0);
    unsigned texture6 = loadAndConfigureTexture("res/6.png", textureShader, "uTex", 0);
    unsigned texture7 = loadAndConfigureTexture("res/7.png", textureShader, "uTex", 0);
    unsigned texture8 = loadAndConfigureTexture("res/8.png", textureShader, "uTex", 0);
    unsigned texture9 = loadAndConfigureTexture("res/9.png", textureShader, "uTex", 0);
    unsigned texture0 = loadAndConfigureTexture("res/0.png", textureShader, "uTex", 0);
    unsigned textureX = loadAndConfigureTexture("res/x.png", textureShader, "uTex", 0);
    unsigned textureY = loadAndConfigureTexture("res/y.png", textureShader, "uTex", 0);
    unsigned textureDot = loadAndConfigureTexture("res/dot.png", textureShader, "uTex", 0);
    unsigned textureMinus = loadAndConfigureTexture("res/minus.png", textureShader, "uTex", 0);
    unsigned textureM = loadAndConfigureTexture("res/m.png", textureShader, "uTex", 0);
    unsigned textureI = loadAndConfigureTexture("res/i.png", textureShader, "uTex", 0);
    unsigned textureL = loadAndConfigureTexture("res/l.png", textureShader, "uTex", 0);
    unsigned textureA = loadAndConfigureTexture("res/a.png", textureShader, "uTex", 0);
    unsigned textureO = loadAndConfigureTexture("res/o.png", textureShader, "uTex", 0);
    unsigned textureV = loadAndConfigureTexture("res/v.png", textureShader, "uTex", 0);
    unsigned textureC = loadAndConfigureTexture("res/c.png", textureShader, "uTex", 0);
    unsigned textureS = loadAndConfigureTexture("res/s.png", textureShader, "uTex", 0);
    unsigned textureD = loadAndConfigureTexture("res/d.png", textureShader, "uTex", 0);
    unsigned textureE = loadAndConfigureTexture("res/e.png", textureShader, "uTex", 0);
    unsigned textureT = loadAndConfigureTexture("res/t.png", textureShader, "uTex", 0);
    unsigned textureR = loadAndConfigureTexture("res/r.png", textureShader, "uTex", 0);
    unsigned static1 = loadAndConfigureTexture("res/static1.jpg", textureShader, "uTex", 0);
    unsigned static2 = loadAndConfigureTexture("res/static2.jpg", textureShader, "uTex", 0);
    unsigned static3 = loadAndConfigureTexture("res/static3.jpg", textureShader, "uTex", 0);
    unsigned static4 = loadAndConfigureTexture("res/static4.jpg", textureShader, "uTex", 0);
    unsigned static5 = loadAndConfigureTexture("res/static5.jpg", textureShader, "uTex", 0);
    unsigned mapTexture = loadAndConfigureTexture("res/majevica.jpg", mapShader, "uTex", 0);



    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++            UNIFORME            +++++++++++++++++++++++++++++++++++++++++++++++++

    glm::mat4 model = glm::mat4(1.0f); 
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 50.0f, 0.2f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.01f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(100.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f);

    shaderProgram.use();
    shaderProgram.setMat4("uM", model);    // Set model matrix
    shaderProgram.setMat4("uV", view);     // Set view matrix
    shaderProgram.setMat4("uP", projection); // Set projection matrix

    //60 FPS
    float timeSinceLastUpdate = 0.0f;
    const float fixedTimeStep = 0.016f;
    float accumulator = 0.0f;
    
    //depth testing and face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    int viewportWidth = wWidth / 2; // Adjust for your layout
    int viewportHeight = wHeight / 2;

    glClearColor(0.529, 0.808, 0.922, 1.0);

    while (!glfwWindowShouldClose(window))
    {

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - timeSinceLastUpdate;
        timeSinceLastUpdate = currentTime;
        accumulator += deltaTime;
        while (accumulator >= fixedTimeStep) {

            // Input handling and changes
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(window, GL_TRUE);
            }
            if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
                noFlyZone = { 0.05f, -0.07f, 0.20f, false, false };
            }
            if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
                if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
                    drone1.active = true;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
                if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
                    drone2.active = true;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
                if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
                    drone1.active = false;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
                if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
                    drone2.active = false;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
                if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
                    drone1.cameraOn = true;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
                if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
                    drone2.cameraOn = true;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
                if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
                    drone1.cameraOn = false;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
                if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
                    drone2.cameraOn = false;
                }
            }
            if (drone1.active && !drone1.destroyed) {
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                    drone1.y += 0.005;
                    if (drone1.y > 1) drone1.destroyed = true;
                }
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                    drone1.y -= 0.005;
                    if (drone1.y < -0.65) drone1.destroyed = true;
                }
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                    drone1.x -= 0.005;
                    if (drone1.x < -1) drone1.destroyed = true;
                }
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                    drone1.x += 0.005;
                    if (drone1.x > 1) drone1.destroyed = true;
                }
            }
            if (drone2.active && !drone2.destroyed) {
                if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                    drone2.y += 0.005;
                    if (drone2.y > 1) drone2.destroyed = true;
                }
                if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                    drone2.y -= 0.005;
                    if (drone2.y < -0.65) drone2.destroyed = true;
                }
                if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                    drone2.x -= 0.005;
                    if (drone2.x < -1) drone2.destroyed = true;
                }
                if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                    drone2.x += 0.005;
                    if (drone2.x > 1) drone2.destroyed = true;
                }
            }
            if (drone1.active && drone1.batteryLevel > 0) {
                drone1.batteryLevel -= 0.1f;
            }
            if (drone2.active && drone2.batteryLevel > 0) {
                drone2.batteryLevel -= 0.1f;
            }

            if (drone1.cameraOn) {
                drone1.batteryLevel -= 0.02;
            }
            if (drone2.cameraOn) {
                drone2.batteryLevel -= 0.02;
            }

            if (drone1.destroyed) {
                drone1.batteryLevel = 0.0f;
            }
            if (drone2.destroyed) {
                drone2.batteryLevel = 0.0f;
            }

            if (drone1.batteryLevel <= 0.0f) {
                drone1.destroyed = true;
            }
            if (drone2.batteryLevel <= 0.0f) {
                drone2.destroyed = true;
            }

            //2 drones
            if (areClashing(drone1.x, drone1.y, drone1.radius, drone2.x, drone2.y, drone2.radius)) {
                drone1.destroyed = true;
                drone2.destroyed = true;
            }

            // drone 1 and no fly zone
            if (areClashing(drone1.x, drone1.y, drone1.radius, noFlyZone.x, noFlyZone.y, noFlyZone.radius)) {
                drone1.destroyed = true;
            }

            // drone 2 and no fly zone
            if (areClashing(drone2.x, drone2.y, drone2.radius, noFlyZone.x, noFlyZone.y, noFlyZone.radius)) {
                drone2.destroyed = true;
            }


            if (noFlyZone.resizing) {
                float aspectRatio = 0.75f;
                float maxXEdge = noFlyZone.x + noFlyZone.radius * aspectRatio;
                float minXEdge = noFlyZone.x - noFlyZone.radius * aspectRatio;
                float maxYEdge = noFlyZone.y + noFlyZone.radius;
                float minYEdge = noFlyZone.y - noFlyZone.radius;

                const float mapMaxX = 1.0f;
                const float mapMinX = -1.0f;
                const float mapMaxY = 1.0f;
                const float mapMinY = -0.7f;

                if (maxXEdge < mapMaxX && minXEdge > mapMinX && maxYEdge < mapMaxY && minYEdge > mapMinY) {
                    noFlyZone.radius += 0.001f;
                }
            }

            if ((!drone1.active || drone1.destroyed) && (!drone2.active || drone2.destroyed)) {

                float newLedVertices[] =
                {
                    -0.93, -0.78,    0.184f, 0.22f, 0.196f, 1.0,
                    -0.43, -0.78,    0.184f, 0.22f, 0.196f, 1.0
                };
                memcpy(ledVertices, newLedVertices, sizeof(newLedVertices));
            }
            else if ((!drone1.active || drone1.destroyed) && drone2.active) {

                float newLedVertices[] =
                {
                    -0.93, -0.78,    0.184f, 0.22f, 0.196f, 1.0,
                    -0.43, -0.78,    0.329f, 0.612f, 0.404f, 1.0
                };
                memcpy(ledVertices, newLedVertices, sizeof(newLedVertices));
            }
            else if (drone1.active && (!drone2.active || drone2.destroyed)) {

                float newLedVertices[] =
                {
                    -0.93, -0.78,    0.329f, 0.612f, 0.404f, 1.0,
                    -0.43, -0.78,    0.184f, 0.22f, 0.196f, 1.0
                };
                memcpy(ledVertices, newLedVertices, sizeof(newLedVertices));
            }
            else {

                float newLedVertices[] =
                {
                    -0.93, -0.78,    0.329f, 0.612f, 0.404f, 1.0,
                    -0.43, -0.78,    0.329f, 0.612f, 0.404f, 1.0
                };
                memcpy(ledVertices, newLedVertices, sizeof(newLedVertices));
            }

            if (!drone1.destroyed) {
                updateDroneVertices(drone1, droneVertices1, 0.75f);
            }
            if (!drone2.destroyed) {
                updateDroneVertices(drone2, droneVertices2, 0.75f);
            }


            updateProgressVertices(-0.9f, -0.74f, 0.4f, 0.08f, drone1.batteryLevel / 100.0f, 0.329f, 0.612f, 0.404f, progressVertices1);
            updateProgressVertices(-0.4f, -0.74f, 0.4f, 0.08f, drone2.batteryLevel / 100.0f, 0.329f, 0.612f, 0.404f, progressVertices2);


            timeSinceLastUpdate = currentTime;
            accumulator -= fixedTimeStep;

        }

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.setVec3("lightPos", glm::vec3(0.0f, 0.0f, 10.0f));
        shaderProgram.setVec3("viewPos", glm::vec3(0.0f, 0.0f, 100.0f)); // Camera position
        shaderProgram.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f)); // White light


        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++            CRTANJE 3D DELA            +++++++++++++++++++++++++++++++++++++++++++++++++

        glViewport(0, wHeight - viewportHeight, viewportWidth, viewportHeight);  // top 1/4 of the screen

        glDisable(GL_DEPTH_TEST);
        glScissor(0, wHeight - viewportHeight, viewportWidth, viewportHeight);
        glEnable(GL_SCISSOR_TEST);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_SCISSOR_TEST);  // Ensure scissor test is off for 3D

        glClearColor(0.529f, 0.808f, 0.922f, 1.0f); // Light blue for 3D
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.use();
        shaderProgram.setMat4("uM", model);
        shaderProgram.setMat4("uV", view);
        shaderProgram.setMat4("uP", projection);

        // render the majevica model
        glm::mat4 majevicaModelMatrix = glm::mat4(1.0f);
        shaderProgram.setMat4("model", majevicaModelMatrix);
        shaderProgram.setMat4("uM", majevicaModelMatrix);
        majevicaModel.Draw(shaderProgram);

        // render the first drone model
        glm::mat4 droneModel1 = glm::mat4(1.0f);
        droneModel1 = glm::scale(droneModel1, glm::vec3(0.8f, 0.8f, 0.8f));
        droneModel1 = glm::translate(droneModel1, glm::vec3(15.0f, 9.0f, 19.0f));
        shaderProgram.setMat4("model", droneModel1);
        shaderProgram.setMat4("uM", droneModel1);
        //shaderProgram.setVec3("objectColor", glm::vec3(0.0f, 1.0f, 0.0f));
        droneModel.Draw(shaderProgram);

        // render the second drone model
        glm::mat4 droneModel2 = glm::mat4(1.0f);
        droneModel2 = glm::scale(droneModel2, glm::vec3(0.8f, 0.8f, 0.8f));
        droneModel2 = glm::translate(droneModel2, glm::vec3(-18.0f, 9.0f, 19.0f));
        shaderProgram.setMat4("model", droneModel2);
        shaderProgram.setMat4("uM", droneModel2);
        droneModel.Draw(shaderProgram);



        //first drone camera view
        if (drone1.cameraOn && !drone1.destroyed) {
            glDisable(GL_DEPTH_TEST);
            glScissor(0, 0, viewportWidth, viewportHeight);
            glEnable(GL_SCISSOR_TEST);
            glClearColor(0.529f, 0.808f, 0.922f, 1.0f); // blue
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_SCISSOR_TEST);
            glViewport(0, 0, wWidth / 2, wHeight / 2);  //bottom left quarter
            glm::mat4 drone1View = glm::lookAt(
                glm::vec3(30.0f, 8.0f, 24.0f), // Camera position (below the drone)
                glm::vec3(30.0f, 8.5f, 25.0f), // Look to front
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
            shaderProgram.setMat4("uV", drone1View);
            shaderProgram.setMat4("uP", projection);

            shaderProgram.setMat4("uM", majevicaModelMatrix);
            majevicaModel.Draw(shaderProgram);
        }
        else {
            glDisable(GL_DEPTH_TEST);
            glScissor(0, 0, viewportWidth, viewportHeight);
            glEnable(GL_SCISSOR_TEST);
            glClearColor(0.0, 0.f, 0.f, 1.0f); // black for background
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_SCISSOR_TEST);
            glViewport(0, 0, wWidth / 2, wHeight / 2);

            glm::mat4 orthoProjection = glm::ortho(0.0f, static_cast<float>(viewportWidth),
                0.0f, static_cast<float>(viewportHeight));
            shaderProgram.setMat4("projection", orthoProjection);
            shaderProgram.setMat4("model", glm::mat4(1.0f));

            //static texture
            glUseProgram(staticShader1);
            int uTime = glGetUniformLocation(staticShader1, "uTime");
            glUniform1f(uTime, static_cast<float>(glfwGetTime()));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, static5);
            glBindVertexArray(staticVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }


        //second drone camera view
        if (drone2.cameraOn && !drone2.destroyed) {
            if (!drone1.cameraOn) {
                shaderProgram.setMat4("projection", projection);
                shaderProgram.use();
            }
            glDisable(GL_DEPTH_TEST);
            glScissor(wWidth / 2, 0, viewportWidth, viewportHeight);
            glEnable(GL_SCISSOR_TEST);
            glClearColor(0.529f, 0.808f, 0.922f, 1.0f); // blue
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_SCISSOR_TEST);
            glViewport(wWidth / 2, 0, wWidth / 2, wHeight / 2);
            glm::mat4 drone2View = glm::lookAt(
                glm::vec3(-18.0f, 8.0f, 24.0f), // Camera position (below the drone)
                glm::vec3(-18.0f, 8.5f, 25.0f), // Look to front
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
            shaderProgram.setMat4("uV", drone2View);
            shaderProgram.setMat4("uP", projection);

            shaderProgram.setMat4("uM", majevicaModelMatrix);
            majevicaModel.Draw(shaderProgram);
        }
        else {
            glDisable(GL_DEPTH_TEST);
            glScissor(wWidth / 2, 0, viewportWidth, viewportHeight);
            glEnable(GL_SCISSOR_TEST);
            glClearColor(0.0, 0.f, 0.f, 1.0f); // black for background
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_SCISSOR_TEST);
            glViewport(wWidth / 2, 0, wWidth / 2, wHeight / 2);

            glm::mat4 orthoProjection = glm::ortho(0.0f, static_cast<float>(viewportWidth),
                0.0f, static_cast<float>(viewportHeight));
            shaderProgram.setMat4("projection", orthoProjection);
            shaderProgram.setMat4("model", glm::mat4(1.0f));

            //static texture
            glUseProgram(staticShader2);
            int uTime = glGetUniformLocation(staticShader2, "uTime");
            glUniform1f(uTime, static_cast<float>(glfwGetTime()));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, static4);
            glBindVertexArray(staticVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }


        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++            CRTANJE 2D DELA            +++++++++++++++++++++++++++++++++++++++++++++++++

        //inicijalizacija da se crta na 1/4 ekrana
        glViewport(wWidth - viewportWidth, wHeight - viewportHeight, viewportWidth, viewportHeight);

        glDisable(GL_DEPTH_TEST);
        glScissor(wWidth - viewportWidth, wHeight - viewportHeight, viewportWidth, viewportHeight);
        glEnable(GL_SCISSOR_TEST);

        glClearColor(0.184f, 0.341f, 0.227f, 1.0f); // Greenish for 2D
        glClear(GL_COLOR_BUFFER_BIT);

        // Set up an orthographic projection for 2D rendering
        glm::mat4 orthoProjection = glm::ortho(0.0f, static_cast<float>(viewportWidth),
            0.0f, static_cast<float>(viewportHeight));
        shaderProgram.setMat4("projection", orthoProjection);
        shaderProgram.setMat4("model", glm::mat4(1.0f));

        //map texture
        glUseProgram(mapShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mapTexture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        //no fly zone
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(unifiedShader);
        updateNoFlyZoneVertices(noFlyZoneVertices, 0.75f);
        glBindBuffer(GL_ARRAY_BUFFER, noFlyZoneVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(noFlyZoneVertices), noFlyZoneVertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(noFlyZoneVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 31);

        //drone 1
        if (!drone1.destroyed) {
            glBindBuffer(GL_ARRAY_BUFFER, droneVBO1);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(droneVertices1), droneVertices1);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(droneVAO1);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 31);
        }

        //drone 2
        if (!drone2.destroyed) {
            glBindBuffer(GL_ARRAY_BUFFER, droneVBO2);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(droneVertices2), droneVertices2);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(droneVAO2);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 31);
        }
        glUseProgram(unifiedShader);
        glBindVertexArray(ledVAO);
        glBindBuffer(GL_ARRAY_BUFFER, ledVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ledVertices), ledVertices);  // Update buffer data
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(ledVAO);
        glPointSize(8);
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glDrawArrays(GL_POINTS, 0, 2);
        glBindVertexArray(0);
        glUseProgram(0);


        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        //progress bar 1
        glBindVertexArray(progressVAO1);
        glBindBuffer(GL_ARRAY_BUFFER, progressVBO1);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(progressVertices1), progressVertices1);  // Update buffer data
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(unifiedShader);
        glBindVertexArray(progressVAO1);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glUseProgram(0);

        //progress bar 2
        glBindVertexArray(progressVAO2);
        glBindBuffer(GL_ARRAY_BUFFER, progressVBO2);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(progressVertices2), progressVertices2);  // Update buffer data
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(unifiedShader);
        glBindVertexArray(progressVAO2);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);


        glUseProgram(textureShader);

        // battery level for drone 1
        int battery1 = static_cast<int>(round(drone1.batteryLevel));
        drawBattery(battery1, texture0, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, batteryVao1);

        // Process battery level for Drone 2
        int battery2 = static_cast<int>(round(drone2.batteryLevel));
        drawBattery(battery2, texture0, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, batteryVao2);

        //coordinates
        // x for drone 1
        glBindTexture(GL_TEXTURE_2D, textureX);
        glBindVertexArray(coordinatesVao1[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // : for x for drone 1
        glBindTexture(GL_TEXTURE_2D, textureDot);
        glBindVertexArray(coordinatesVao1[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindTexture(GL_TEXTURE_2D, textureDot);
        glBindVertexArray(coordinatesVao1[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        if (drone1.x < 0 || drone1.destroyed) {
            // - for x for drone 1
            glBindTexture(GL_TEXTURE_2D, textureMinus);
            glBindVertexArray(coordinatesVao1[3]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        if (!drone1.destroyed) {
            // num1 for x for drone 1
            int num1 = 0;
            if (drone1.x <= -1 || drone1.x >= 1) num1 = 1;
            switch (num1) {
            case 0: {
                glBindTexture(GL_TEXTURE_2D, texture0);
                break;
            }
            case 1: {
                glBindTexture(GL_TEXTURE_2D, texture1);
                break;
            }
            default: glBindTexture(GL_TEXTURE_2D, texture0); // Fallback, should not occur
            }
            glBindVertexArray(coordinatesVao1[4]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            // . for x for drone 1
            glBindTexture(GL_TEXTURE_2D, textureDot);
            glBindVertexArray(coordinatesVao1[5]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            // num2 for x for drone 1
            float scaledX = abs(drone1.x) * 100.0f;
            int precise = static_cast<int>(round(scaledX));
            int first = (precise / 10) % 10;
            int second = precise % 10;
            drawDigit(6, first, texture0, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, coordinatesVao1);
            drawDigit(7, second, texture0, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, coordinatesVao1);
        }

        // y for drone 1
        glBindTexture(GL_TEXTURE_2D, textureY);
        glBindVertexArray(coordinatesVao1y[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // : for y for drone 1
        glBindTexture(GL_TEXTURE_2D, textureDot);
        glBindVertexArray(coordinatesVao1y[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindTexture(GL_TEXTURE_2D, textureDot);
        glBindVertexArray(coordinatesVao1y[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        if (drone1.y < 0 || drone1.destroyed) {
            // - for y for drone 1
            glBindTexture(GL_TEXTURE_2D, textureMinus);
            glBindVertexArray(coordinatesVao1y[3]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        if (!drone1.destroyed) {
            // num1 for y for drone 1
            int num1 = 0;
            if (drone1.y <= -1 || drone1.y >= 1) num1 = 1;
            switch (num1) {
            case 0: {
                glBindTexture(GL_TEXTURE_2D, texture0);
                break;
            }
            case 1: {
                glBindTexture(GL_TEXTURE_2D, texture1);
                break;
            }
            default: glBindTexture(GL_TEXTURE_2D, texture0); // Fallback, should not occur
            }
            glBindVertexArray(coordinatesVao1y[4]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            // . for x for drone 1
            glBindTexture(GL_TEXTURE_2D, textureDot);
            glBindVertexArray(coordinatesVao1y[5]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            // num2 for x for drone 1
            float scaledY = abs(drone1.y) * 100.0f;
            int precise = static_cast<int>(round(scaledY));
            int first = (precise / 10) % 10;
            int second = precise % 10;
            drawDigit(6, first, texture0, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, coordinatesVao1y);
            drawDigit(7, second, texture0, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, coordinatesVao1y);
        }

        // x for drone 2
        glBindTexture(GL_TEXTURE_2D, textureX);
        glBindVertexArray(coordinatesVao2[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        // : for x for drone 2
        glBindTexture(GL_TEXTURE_2D, textureDot);
        glBindVertexArray(coordinatesVao2[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindTexture(GL_TEXTURE_2D, textureDot);
        glBindVertexArray(coordinatesVao2[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        if (drone2.x < 0 || drone2.destroyed) {
            // - for x for drone 1
            glBindTexture(GL_TEXTURE_2D, textureMinus);
            glBindVertexArray(coordinatesVao2[3]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        // num1 for x for drone 2
        if (!drone2.destroyed) {
            int num1 = 0;
            if (drone2.x <= -1 || drone2.x >= 1) num1 = 1;
            switch (num1) {
            case 0: {
                glBindTexture(GL_TEXTURE_2D, texture0);
                break;
            }
            case 1: {
                glBindTexture(GL_TEXTURE_2D, texture1);
                break;
            }
            default: glBindTexture(GL_TEXTURE_2D, texture0); // Fallback, should not occur
            }
            glBindVertexArray(coordinatesVao2[4]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            // . for x for drone 2
            glBindTexture(GL_TEXTURE_2D, textureDot);
            glBindVertexArray(coordinatesVao2[5]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            // num2 for x for drone 2
            int scaledX = abs(drone2.x) * 100.0f;
            int precise = static_cast<int>(round(scaledX));
            int first = (precise / 10) % 10;
            int second = precise % 10;

            drawDigit(6, first, texture0, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, coordinatesVao2);
            drawDigit(7, second, texture0, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, coordinatesVao2);
        }

        // y for drone 2
        glBindTexture(GL_TEXTURE_2D, textureY);
        glBindVertexArray(coordinatesVao2y[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // : for y for drone 2
        glBindTexture(GL_TEXTURE_2D, textureDot);
        glBindVertexArray(coordinatesVao2y[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindTexture(GL_TEXTURE_2D, textureDot);
        glBindVertexArray(coordinatesVao2y[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        if (drone2.y < 0 || drone2.destroyed) {
            // - for y for drone 2
            glBindTexture(GL_TEXTURE_2D, textureMinus);
            glBindVertexArray(coordinatesVao2y[3]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        // num1 for y for drone 2
        if (!drone2.destroyed) {
            int num1 = 0;
            if (drone2.y <= -1 || drone2.y >= 1) num1 = 1;
            switch (num1) {
            case 0: {
                glBindTexture(GL_TEXTURE_2D, texture0);
                break;
            }
            case 1: {
                glBindTexture(GL_TEXTURE_2D, texture1);
                break;
            }
            default: glBindTexture(GL_TEXTURE_2D, texture0); // Fallback, should not occur
            }
            glBindVertexArray(coordinatesVao2y[4]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            // . for x for drone 2
            glBindTexture(GL_TEXTURE_2D, textureDot);
            glBindVertexArray(coordinatesVao2y[5]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            // num2 for x for drone 1
            int scaledY = abs(drone2.y) * 100.0f;
            int precise = static_cast<int>(round(scaledY));
            int first = (precise / 10) % 10;
            int second = precise % 10;

            drawDigit(6, first, texture0, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, coordinatesVao2y);
            drawDigit(7, second, texture0, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, coordinatesVao2y);
        }

        //IME I PREZIME
        //m
        glBindTexture(GL_TEXTURE_2D, textureM);
        glBindVertexArray(nameVao[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //i
        glBindTexture(GL_TEXTURE_2D, textureI);
        glBindVertexArray(nameVao[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //l
        glBindTexture(GL_TEXTURE_2D, textureL);
        glBindVertexArray(nameVao[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //a
        glBindTexture(GL_TEXTURE_2D, textureA);
        glBindVertexArray(nameVao[3]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //m
        glBindTexture(GL_TEXTURE_2D, textureM);
        glBindVertexArray(nameVao[4]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //i
        glBindTexture(GL_TEXTURE_2D, textureI);
        glBindVertexArray(nameVao[5]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //l
        glBindTexture(GL_TEXTURE_2D, textureL);
        glBindVertexArray(nameVao[6]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //o
        glBindTexture(GL_TEXTURE_2D, textureO);
        glBindVertexArray(nameVao[7]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //v
        glBindTexture(GL_TEXTURE_2D, textureV);
        glBindVertexArray(nameVao[8]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //i
        glBindTexture(GL_TEXTURE_2D, textureI);
        glBindVertexArray(nameVao[9]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //c
        glBindTexture(GL_TEXTURE_2D, textureC);
        glBindVertexArray(nameVao[10]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //s
        glBindTexture(GL_TEXTURE_2D, textureS);
        glBindVertexArray(indexVao[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //v
        glBindTexture(GL_TEXTURE_2D, textureV);
        glBindVertexArray(indexVao[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //2
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(indexVao[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //2
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(indexVao[3]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //-
        glBindTexture(GL_TEXTURE_2D, textureMinus);
        glBindVertexArray(indexVao[4]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //2
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(indexVao[5]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //0
        glBindTexture(GL_TEXTURE_2D, texture0);
        glBindVertexArray(indexVao[6]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //2
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(indexVao[7]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //1
        glBindTexture(GL_TEXTURE_2D, texture1);
        glBindVertexArray(indexVao[8]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        //DESTROYED 1
        if (drone1.destroyed) {
            //d
            glBindTexture(GL_TEXTURE_2D, textureD);
            glBindVertexArray(destroyedVao1[0]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //e
            glBindTexture(GL_TEXTURE_2D, textureE);
            glBindVertexArray(destroyedVao1[1]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //s
            glBindTexture(GL_TEXTURE_2D, textureS);
            glBindVertexArray(destroyedVao1[2]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //t
            glBindTexture(GL_TEXTURE_2D, textureT);
            glBindVertexArray(destroyedVao1[3]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //r
            glBindTexture(GL_TEXTURE_2D, textureR);
            glBindVertexArray(destroyedVao1[4]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //o
            glBindTexture(GL_TEXTURE_2D, textureO);
            glBindVertexArray(destroyedVao1[5]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //y
            glBindTexture(GL_TEXTURE_2D, textureY);
            glBindVertexArray(destroyedVao1[6]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //e
            glBindTexture(GL_TEXTURE_2D, textureE);
            glBindVertexArray(destroyedVao1[7]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //d
            glBindTexture(GL_TEXTURE_2D, textureD);
            glBindVertexArray(destroyedVao1[8]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        //DESTROYED 2
        if (drone2.destroyed) {
            //d
            glBindTexture(GL_TEXTURE_2D, textureD);
            glBindVertexArray(destroyedVao2[0]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //e
            glBindTexture(GL_TEXTURE_2D, textureE);
            glBindVertexArray(destroyedVao2[1]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //s
            glBindTexture(GL_TEXTURE_2D, textureS);
            glBindVertexArray(destroyedVao2[2]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //t
            glBindTexture(GL_TEXTURE_2D, textureT);
            glBindVertexArray(destroyedVao2[3]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //r
            glBindTexture(GL_TEXTURE_2D, textureR);
            glBindVertexArray(destroyedVao2[4]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //o
            glBindTexture(GL_TEXTURE_2D, textureO);
            glBindVertexArray(destroyedVao2[5]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //y
            glBindTexture(GL_TEXTURE_2D, textureY);
            glBindVertexArray(destroyedVao2[6]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //e
            glBindTexture(GL_TEXTURE_2D, textureE);
            glBindVertexArray(destroyedVao2[7]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //d
            glBindTexture(GL_TEXTURE_2D, textureD);
            glBindVertexArray(destroyedVao2[8]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        //disable for the 3d project
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_SCISSOR_TEST);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ POSPREMANJE +++++++++++++++++++++++++++++++++++++++++++++++++

    glDeleteProgram(unifiedShader);
    glDeleteProgram(textureShader);
    glDeleteProgram(mapShader);
    glDeleteProgram(staticShader1);
    glDeleteProgram(staticShader2);

    glDeleteTextures(1, &mapTexture);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &noFlyZoneVBO);
    glDeleteVertexArrays(1, &noFlyZoneVAO);
    glDeleteBuffers(1, &droneVBO1);
    glDeleteVertexArrays(1, &droneVAO1);
    glDeleteBuffers(1, &droneVBO2);
    glDeleteVertexArrays(1, &droneVAO2);
    glDeleteVertexArrays(1, &progressVAO1);
    glDeleteBuffers(1, &progressVBO1);
    glDeleteBuffers(1, &progressEBO1);
    glDeleteVertexArrays(1, &progressVAO2);
    glDeleteBuffers(1, &progressVBO2);
    glDeleteBuffers(1, &progressEBO2);
    glDeleteVertexArrays(3, batteryVao1);
    glDeleteBuffers(3, batteryVbo1);
    glDeleteBuffers(3, batteryEbo1);
    glDeleteVertexArrays(3, batteryVao2);
    glDeleteBuffers(3, batteryVbo2);
    glDeleteBuffers(3, batteryEbo2);
    glDeleteVertexArrays(8, coordinatesVao1);
    glDeleteBuffers(8, coordinatesVbo1);
    glDeleteBuffers(8, coordinatesEbo1);
    glDeleteVertexArrays(8, coordinatesVao1y);
    glDeleteBuffers(8, coordinatesVbo1y);
    glDeleteBuffers(8, coordinatesEbo1y);
    glDeleteVertexArrays(8, coordinatesVao2);
    glDeleteBuffers(8, coordinatesVbo2);
    glDeleteBuffers(8, coordinatesEbo2);
    glDeleteVertexArrays(8, coordinatesVao2y);
    glDeleteBuffers(8, coordinatesVbo2y);
    glDeleteBuffers(8, coordinatesEbo2y);
    glDeleteVertexArrays(11, nameVao);
    glDeleteBuffers(11, nameVbo);
    glDeleteBuffers(11, nameEbo);
    glDeleteVertexArrays(9, indexVao);
    glDeleteBuffers(9, indexVbo);
    glDeleteBuffers(9, indexEbo);
    glDeleteVertexArrays(9, destroyedVao1);
    glDeleteBuffers(9, destroyedVbo1);
    glDeleteBuffers(9, destroyedEbo1);
    glDeleteVertexArrays(9, destroyedVao2);
    glDeleteBuffers(9, destroyedVbo2);
    glDeleteBuffers(9, destroyedEbo2);
    glDeleteBuffers(1, &ledVBO);
    glDeleteVertexArrays(1, &ledVAO);

    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
    std::string temp = ss.str();
    const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);

    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}
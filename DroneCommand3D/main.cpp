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
    unsigned int wWidth = 500;
    unsigned int wHeight = 500;
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


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++            MODELI            +++++++++++++++++++++++++++++++++++++++++++++++++

    Shader shaderProgram("basic_3d.vert", "basic_3d.frag"); // Adjust paths if needed
    Model droneModel("res/drone.obj");
    Model majevicaModel("res/majevicamala.obj");


    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++            UNIFORME            +++++++++++++++++++++++++++++++++++++++++++++++++

    glm::mat4 model = glm::mat4(1.0f); 
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 100.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f);

    shaderProgram.use();
    shaderProgram.setMat4("uM", model);    // Set model matrix
    shaderProgram.setMat4("uV", view);     // Set view matrix
    shaderProgram.setMat4("uP", projection); // Set projection matrix


    glClearColor(0.5, 0.5, 0.5, 1.0);
    glCullFace(GL_BACK);//Biranje lica koje ce se eliminisati (tek nakon sto ukljucimo Face Culling)
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        // Input handling
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.setVec3("lightPos", glm::vec3(0.0f, 0.0f, 10.0f));
        shaderProgram.setVec3("viewPos", glm::vec3(0.0f, 0.0f, 100.0f)); // Camera position
        shaderProgram.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f)); // White light

        // Update the transformation matrix for the majevica model
        glm::mat4 majevicaModelMatrix = glm::mat4(1.0f);
        majevicaModelMatrix = glm::scale(majevicaModelMatrix, glm::vec3(0.001f, 0.001f, 0.001f));  // Adjust the scale factor
        majevicaModelMatrix = glm::translate(majevicaModelMatrix, glm::vec3(0.0f, 0.0f, -100.0f));  // Move it back to fit within the view
        shaderProgram.setMat4("model", majevicaModelMatrix);
        //shaderProgram.setVec3("objectColor", glm::vec3(0.7f, 0.2f, 0.1f));

        unsigned int textureID = majevicaModel.textures_loaded[0].id; // Assuming you have a function to get the texture ID
        glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
        glBindTexture(GL_TEXTURE_2D, textureID);
        shaderProgram.setInt("texture1", 0);

        // Render the majevica model
        majevicaModel.Draw(shaderProgram);

        // Update the transformation matrix for the drone model
        glm::mat4 droneModelMatrix = glm::mat4(1.0f);
        droneModelMatrix = glm::translate(droneModelMatrix, glm::vec3(0.0f, -0.5f, 0.0f)); // Example translation
        droneModelMatrix = glm::scale(droneModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f)); // Example scaling
        shaderProgram.setMat4("model", droneModelMatrix);
        //shaderProgram.setVec3("objectColor", glm::vec3(0.1f, 0.1f, 0.15f));

        // Render the drone model
        droneModel.Draw(shaderProgram);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ POSPREMANJE +++++++++++++++++++++++++++++++++++++++++++++++++

    glDeleteProgram(unifiedShader);

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
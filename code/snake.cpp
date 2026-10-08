#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include "shader.h"
#include <atomic> 

std::atomic<bool> startGame(false);

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float r = 0.2f, g = 0.3f, b = 0.3f;

void initBuffers();
void points();
void renderpoints(Shader& shader);
void renderRandomCubes(Shader& shader);
void DrawSnake(const glm::vec3& snakePosition);
void DrawSnakehead(const glm::vec3& snakePosition);
void snakeheadColor();
void renderSnake(Shader& shader);
void checkOutOfBounds(glm::vec3& position);
void checkCollision(GLFWwindow* window, Shader& shader);


void start(GLFWwindow* window, Shader& shader);
void gameOver(GLFWwindow* window, Shader& shader);
void resetGame(GLFWwindow* window, Shader& shader);





unsigned int VBO, VAO;
unsigned int BVBO, BVAO;
unsigned int GVBO, GVAO;
unsigned int SVBO, SVAO;
unsigned int HVBO, HVAO;

unsigned int CVBO, CVAO;

unsigned int H1VBO, H1VAO;
unsigned int H2VBO, H2VAO;
unsigned int H3VBO, H3VAO;

unsigned int S1VBO, S1VAO;
unsigned int S2VBO, S2VAO;
unsigned int S3VBO, S3VAO;


static glm::vec3* selectedPoint = nullptr;


glm::vec3 snakePosition(0.0f, 0.0f, 0.0f); // Kezdeti pozíció
std::vector<glm::vec3> redCubePositions;
std::vector<glm::vec3> blueCubePositions;
std::vector<glm::vec3> greenCubePositions;
std::vector<glm::vec3> snakeSegments;
std::vector<glm::vec3> snakeHead;
std::vector<glm::vec3> ccpoints;


enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

Direction snakeDirection = RIGHT; // Az elején jobbra néz a kígyó
int snakeLength = 1; // kígyó hossza

const float MOVE_SPEED = 0.0004f; // Mozgási sebesség

double lastTime;
double deltaTime;
const double moveInterval = 0.05;


double lt;
double dt;
const double rendertime = 8.0;


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Snake", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader ourShader("Shaders/4.0.shader.vs", "Shaders/4.0.shader.fs");


    initBuffers();

    DrawSnake(snakePosition);

    snakeheadColor();

    points();

    


    lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glClearColor(r, g, b, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ourShader.use();

        glfwSetKeyCallback(window, key_callback);


        start(window, ourShader);


        double currentTime = glfwGetTime();
        deltaTime += (currentTime - lastTime);
        lastTime = currentTime;

        if (deltaTime >= moveInterval) {

            switch (snakeDirection) {
            case UP:
                snakeHead[0].y += 0.03f;
                break;
            case DOWN:
                snakeHead[0].y -= 0.03f;
                break;
            case LEFT:
                snakeHead[0].x -= 0.03f;
                break;
            case RIGHT:
                snakeHead[0].x += 0.03f;
                break;
            }

            for (int i = snakeSegments.size() - 1; i > 0; --i) {

                snakeSegments[i] = snakeSegments[i - 1];

            }
            snakeSegments[0] = snakeHead[0];

            deltaTime = 0.0;

        }

        checkCollision(window, ourShader);


        dt += (currentTime - lt);
        lt = currentTime;

        if (dt >= rendertime) {


            if (blueCubePositions.size() < 6) {

                float posX, posY, posZ = 0.0f;

                posX = ((rand() % 200) - 100) / 100.0f;
                posY = ((rand() % 200) - 100) / 100.0f;

                blueCubePositions.push_back(glm::vec3(posX, posY, posZ));
            }

            dt = 0.0;

        }
        checkOutOfBounds(snakePosition);

        renderRandomCubes(ourShader);

        renderSnake(ourShader);

        renderpoints(ourShader);



        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}


void start(GLFWwindow* window, Shader& shader) {

    while (!startGame.load() && !glfwWindowShouldClose(window)) {
        glfwWaitEvents();
        Shader ourShader("Shaders/4.0.shader.vs", "Shaders/4.0.shader.fs");

        glfwSetMouseButtonCallback(window, mouse_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (const auto& p : ccpoints) {
            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, p);
            trans = glm::scale(trans, glm::vec3(0.03f, 0.03f, 0.03f));

            shader.setMat4("transform", trans);

            glBindVertexArray(CVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }


       glfwSwapBuffers(window);


    }
}



void resetGame(GLFWwindow* window, Shader& shader) {

    glClearColor(r, g, b, 1.0);

    snakeSegments.clear();
    snakeHead.clear();
    snakePosition = glm::vec3(0.0f, 0.0f, 0.0f);
    snakeDirection = RIGHT;
    snakeLength = 1;
    redCubePositions.clear();
    blueCubePositions.clear();
    greenCubePositions.clear();

    initBuffers();

    DrawSnake(snakePosition);

    std::cout << "A jatek ujrakezdodott, sok szerencset!" << std::endl;
}


void gameOver(GLFWwindow* window, Shader& shader) {
    std::cout << "Az elert pontod: " << snakeLength << std::endl;
    std::cout << "Nyomj Entert az ujrakezdeshez vagy Esc-et a kilépeshez." << std::endl;


    ccpoints.clear();
    while (!(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) && !glfwWindowShouldClose(window)) {
        glfwSetMouseButtonCallback(window, mouse_callback);
        glfwPollEvents();

        Shader ourShader("Shaders/4.0.shader.vs", "Shaders/4.0.shader.fs");


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (const auto& p : ccpoints) {
            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, p);
            trans = glm::scale(trans, glm::vec3(0.03f, 0.03f, 0.03f));

            shader.setMat4("transform", trans);

            glBindVertexArray(CVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }


        glfwSwapBuffers(window);
    }


    while (true) {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
            resetGame(window, shader);
            return;
        }
        else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
            return;
        }
    }
}





void points() {
    float vertpoints[] = {
        0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,

        0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
       -0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &CVAO);
    glGenBuffers(1, &CVBO);

    glBindVertexArray(CVAO);
    glBindBuffer(GL_ARRAY_BUFFER, CVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertpoints), vertpoints, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderpoints(Shader& shader) {
    for (const auto& p : ccpoints) {
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, p);
        trans = glm::scale(trans, glm::vec3(0.03f, 0.03f, 0.03f));

        shader.setMat4("transform", trans);

        glBindVertexArray(CVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}


void initBuffers() {
    float vertices[] = {

        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,

        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f
    };

    float bluecubes[] = {

        0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
       -0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,

        0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
       -0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f
    };

    float greencubes[] = {
        0.5f,  0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
       -0.5f,  0.5f, 0.0f,   0.0f, 1.0f, 0.0f,

        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
       -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
       -0.5f,  0.5f, 0.0f,   0.0f, 1.0f, 0.0f
    };



    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float posX, posY, posZ = 0.0f;

    posX = ((rand() % 200) - 100) / 100.0f;
    posY = ((rand() % 200) - 100) / 100.0f;

    redCubePositions.push_back(glm::vec3(posX, posY, posZ));


    for (int i = 0; i < 1; ++i) {
        glGenVertexArrays(1, &BVAO);
        glGenBuffers(1, &BVBO);

        glBindVertexArray(BVAO);
        glBindBuffer(GL_ARRAY_BUFFER, BVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(bluecubes), bluecubes, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        float posX, posY, posZ = 0.0f;

        posX = -2.0;
        posY = -2.0;

        blueCubePositions.push_back(glm::vec3(posX, posY, posZ));
    }


    for (int i = 0; i < 3; ++i) {
        glGenVertexArrays(1, &GVAO);
        glGenBuffers(1, &GVBO);

        glBindVertexArray(GVAO);
        glBindBuffer(GL_ARRAY_BUFFER, GVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(greencubes), greencubes, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        float posX, posY, posZ = 0.0f;

        posX = 0.85f + i * 0.05;
        posY = 0.95f;

        greenCubePositions.push_back(glm::vec3(posX, posY, posZ));
    }
}




void renderRandomCubes(Shader& shader) {
    for (const auto& red : redCubePositions) {
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, red);
        trans = glm::scale(trans, glm::vec3(0.03f, 0.03f, 0.03f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));

        shader.setMat4("transform", trans);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }


    for (const auto& blue : blueCubePositions) {
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, blue);
        trans = glm::scale(trans, glm::vec3(0.03f, 0.03f, 0.03f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));

        shader.setMat4("transform", trans);

        glBindVertexArray(BVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }



    for (const auto& green : greenCubePositions) {
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, green);
        trans = glm::scale(trans, glm::vec3(0.03f, 0.03f, 0.03f));

        shader.setMat4("transform", trans);

        glBindVertexArray(GVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}


//Kígyó fej szinezése
//Az X tengely 3 részre van bontva és mindegyikben más színnel rajzolódik ki a kígyó feje és a teste is

void snakeheadColor() {
    float snakeheadc1[] = {

        0.5f,  0.5f, 0.0f,   1.0f, 0.588, 0.588f,
        0.5f, -0.5f, 0.0f,   1.0f, 0.588, 0.588f,
       -0.5f,  0.5f, 0.0f,   1.0f, 0.588, 0.588f,

        0.5f, -0.5f, 0.0f,   1.0f, 0.588, 0.588f,
       -0.5f, -0.5f, 0.0f,   1.0f, 0.588, 0.588f,
       -0.5f,  0.5f, 0.0f,   1.0f, 0.588, 0.588f
    };

    float snakeheadc2[] = {

        0.5f,  0.5f, 0.0f,   0.196f, 0.784f, 0.196f,
        0.5f, -0.5f, 0.0f,   0.196f, 0.784f, 0.196f,
       -0.5f,  0.5f, 0.0f,   0.196f, 0.784f, 0.196f,

        0.5f, -0.5f, 0.0f,   0.196f, 0.784f, 0.196f,
       -0.5f, -0.5f, 0.0f,   0.196f, 0.784f, 0.196f,
       -0.5f,  0.5f, 0.0f,   0.196f, 0.784f, 0.196f
    };

    float snakeheadc3[] = {

        0.5f,  0.5f, 0.0f,   0.2f, 1.0f, 2.0f,
        0.5f, -0.5f, 0.0f,   0.2f, 1.0f, 2.0f,
       -0.5f,  0.5f, 0.0f,   0.2f, 1.0f, 2.0f,

        0.5f, -0.5f, 0.0f,   0.2f, 1.0f, 2.0f,
       -0.5f, -0.5f, 0.0f,   0.2f, 1.0f, 2.0f,
       -0.5f,  0.5f, 0.0f,   0.2f, 1.0f, 2.0f,
    };



    float snakesegmentsc1[] = {

        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 1.0f,
       -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 1.0f,

        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 1.0f,
       -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 1.0f
    };

    float snakesegmentsc2[] = {

        0.5f,  0.5f, 0.0f,   0.2f, 1.0f, 2.0f,
        0.5f, -0.5f, 0.0f,   0.2f, 1.0f, 2.0f,
       -0.5f,  0.5f, 0.0f,   0.2f, 1.0f, 2.0f,

        0.5f, -0.5f, 0.0f,   0.2f, 1.0f, 2.0f,
       -0.5f, -0.5f, 0.0f,   0.2f, 1.0f, 2.0f,
       -0.5f,  0.5f, 0.0f,   0.2f, 1.0f, 2.0f
    };

    float snakesegmentsc3[] = {

        0.5f,  0.5f, 0.0f,   0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.0f,   0.5f, 0.5f, 0.5f,
       -0.5f,  0.5f, 0.0f,   0.5f, 0.5f, 0.5f,

        0.5f, -0.5f, 0.0f,   0.5f, 0.5f, 0.5f,
       -0.5f, -0.5f, 0.0f,   0.5f, 0.5f, 0.5f,
       -0.5f,  0.5f, 0.0f,   0.5f, 0.5f, 0.5f
    };


    glGenVertexArrays(1, &H1VAO);
    glGenBuffers(1, &H1VBO);

    glBindVertexArray(H1VAO);
    glBindBuffer(GL_ARRAY_BUFFER, H1VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(snakeheadc1), snakeheadc1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);




    glGenVertexArrays(1, &H2VAO);
    glGenBuffers(1, &H2VBO);

    glBindVertexArray(H2VAO);
    glBindBuffer(GL_ARRAY_BUFFER, H2VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(snakeheadc2), snakeheadc2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    glGenVertexArrays(1, &H3VAO);
    glGenBuffers(1, &H3VBO);

    glBindVertexArray(H3VAO);
    glBindBuffer(GL_ARRAY_BUFFER, H3VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(snakeheadc3), snakeheadc3, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);




    //snakesegmentscolor
    glGenVertexArrays(1, &S1VAO);
    glGenBuffers(1, &S1VBO);

    glBindVertexArray(S1VAO);
    glBindBuffer(GL_ARRAY_BUFFER, S1VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(snakesegmentsc1), snakesegmentsc1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    glGenVertexArrays(1, &S2VAO);
    glGenBuffers(1, &S2VBO);

    glBindVertexArray(S2VAO);
    glBindBuffer(GL_ARRAY_BUFFER, S2VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(snakesegmentsc2), snakesegmentsc2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    glGenVertexArrays(1, &S3VAO);
    glGenBuffers(1, &S3VBO);

    glBindVertexArray(S3VAO);
    glBindBuffer(GL_ARRAY_BUFFER, S3VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(snakesegmentsc3), snakesegmentsc3, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void DrawSnake(const glm::vec3& snakePosition) {

    float snakevert[] = {

        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 1.0f,
       -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 1.0f,

        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 1.0f,
       -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 1.0f
    };

    float snakehead[] = {

        0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,

        0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
       -0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f
    };



    for (int i = 0; i < 5; ++i) {

        glGenVertexArrays(1, &SVAO);
        glGenBuffers(1, &SVBO);

        glBindVertexArray(SVAO);
        glBindBuffer(GL_ARRAY_BUFFER, SVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(snakevert), snakevert, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);


        float posX = 0.0f - i * 0.03f;
        float posY = 0.0f;
        float posZ = 0.0f;

        snakeSegments.push_back(glm::vec3(posX, posY, posZ));
    }


    glGenVertexArrays(1, &HVAO);
    glGenBuffers(1, &HVBO);

    glBindVertexArray(HVAO);
    glBindBuffer(GL_ARRAY_BUFFER, HVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(snakehead), snakehead, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = 0.0f;

    snakeHead.push_back(glm::vec3(posX, posY, posZ));
}


void renderSnake(Shader& shader) {

    for (int i = 0; i < snakeSegments.size(); i++) {
        if (snakeSegments[i].x > -0.99 && snakeSegments[i].x < -0.33) {
            float posX = snakeSegments[i].x;
            float posY = snakeSegments[i].y;
            float posZ = snakeSegments[i].z;

            snakeSegments.erase(snakeSegments.begin() + i);

            snakeSegments.insert(snakeSegments.begin() + i, glm::vec3(posX, posY, posZ));
            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, snakeSegments[i]);
            trans = glm::scale(trans, glm::vec3(0.03, 0.03, 0.03));

            shader.setMat4("transform", trans);

            glBindVertexArray(S1VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

        }
    }

    for (int i = 0; i < snakeSegments.size(); i++) {
        if (snakeSegments[i].x > -0.33 && snakeSegments[i].x < 0.33) {
            float posX = snakeSegments[i].x;
            float posY = snakeSegments[i].y;
            float posZ = snakeSegments[i].z;

            snakeSegments.erase(snakeSegments.begin() + i);

            snakeSegments.insert(snakeSegments.begin() + i, glm::vec3(posX, posY, posZ));
            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, snakeSegments[i]);
            trans = glm::scale(trans, glm::vec3(0.03, 0.03, 0.03));

            shader.setMat4("transform", trans);

            glBindVertexArray(S2VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    for (int i = 0; i < snakeSegments.size(); i++) {
        if (snakeSegments[i].x > 0.33 && snakeSegments[i].x < 0.99) {
            float posX = snakeSegments[i].x;
            float posY = snakeSegments[i].y;
            float posZ = snakeSegments[i].z;

            snakeSegments.erase(snakeSegments.begin() + i);

            snakeSegments.insert(snakeSegments.begin() + i, glm::vec3(posX, posY, posZ));
            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, snakeSegments[i]);
            trans = glm::scale(trans, glm::vec3(0.03, 0.03, 0.03));

            shader.setMat4("transform", trans);

            glBindVertexArray(S3VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }




    if (snakeHead[0].x > -0.99 && snakeHead[0].x < -0.33) {

        float posX = snakeHead[0].x;
        float posY = snakeHead[0].y;
        float posZ = snakeHead[0].z;

        snakeHead.clear();

        snakeHead.push_back(glm::vec3(posX, posY, posZ));

        for (const auto& head : snakeHead) {
            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, head);
            trans = glm::scale(trans, glm::vec3(0.03, 0.03, 0.03));

            shader.setMat4("transform", trans);

            glBindVertexArray(H1VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    if (snakeHead[0].x > -0.33 && snakeHead[0].x < 0.33) {


        float posX = snakeHead[0].x;
        float posY = snakeHead[0].y;
        float posZ = snakeHead[0].z;

        snakeHead.clear();

        snakeHead.push_back(glm::vec3(posX, posY, posZ));

        for (const auto& head : snakeHead) {
            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, head);
            trans = glm::scale(trans, glm::vec3(0.03, 0.03, 0.03));

            shader.setMat4("transform", trans);

            glBindVertexArray(H2VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    if (snakeHead[0].x > 0.33 && snakeHead[0].x < 0.99) {

        float posX = snakeHead[0].x;
        float posY = snakeHead[0].y;
        float posZ = snakeHead[0].z;

        snakeHead.clear();

        snakeHead.push_back(glm::vec3(posX, posY, posZ));

        for (const auto& head : snakeHead) {
            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, head);
            trans = glm::scale(trans, glm::vec3(0.03, 0.03, 0.03));

            shader.setMat4("transform", trans);

            glBindVertexArray(H3VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

}



void checkCollision(GLFWwindow* window, Shader& shader) {
    Shader ourShader("Shaders/4.0.shader.vs", "Shaders/4.0.shader.fs");
    if (greenCubePositions.size() == 0) {
        std::cout << "Game Over! Elfogytak az eleteid." << std::endl;
        gameOver(window, ourShader);
    }
    for (size_t i = 1; i < snakeSegments.size(); ++i) {
        if (snakeHead[0] == snakeSegments[i]) {
            std::cout << "Game Over! A kigyo magaval utkozott." << std::endl;
            gameOver(window, ourShader);

        }
    }


    for (const auto& redCubePos : redCubePositions) {

        if (glm::distance(snakeHead[0], redCubePos) < 0.03f) {

            snakeLength++;

            glm::vec3 newSnakePart = snakeSegments[snakeLength - 2] - glm::normalize(snakeSegments[snakeLength - 2] - snakeSegments[snakeLength - 1]) * 0.03f;
            snakeSegments.push_back(newSnakePart);

            redCubePositions.erase(std::remove(redCubePositions.begin(), redCubePositions.end(), redCubePos), redCubePositions.end());

            float posX, posY, posZ = 0.0f;

            posX = ((rand() % 200) - 100) / 100.0f;
            posY = ((rand() % 200) - 100) / 100.0f;

            redCubePositions.push_back(glm::vec3(posX, posY, posZ));

        }
    }


    for (const auto& blueCubePos : blueCubePositions) {

        if (glm::distance(snakeHead[0], blueCubePos) < 0.03f) {

            blueCubePositions.erase(std::remove(blueCubePositions.begin(), blueCubePositions.end(), blueCubePos), blueCubePositions.end());

            greenCubePositions.erase(greenCubePositions.begin());


        }
    }

    for (const auto& pos : ccpoints) {

        if (glm::distance(snakeHead[0], pos) < 0.03f) {
            std::cout << "Game Over! Nekiutkoztel egy falnak." << std::endl;
            gameOver(window, ourShader);
        }
    }
}



void checkOutOfBounds(glm::vec3& position) {
    if (snakeHead[0].x > 0.99f)
        snakeHead[0].x = -0.99f;
    else if (snakeHead[0].x < -0.99f)
        snakeHead[0].x = 0.99f;
    else if (snakeHead[0].y > 0.99f)
        snakeHead[0].y = -0.99f;
    else if (snakeHead[0].y < -0.99f)
        snakeHead[0].y = 0.99f;
}


static void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        float x = (2.0f * xpos) / SCR_WIDTH - 1.0f;
        float y = 1.0f - (2.0f * ypos) / SCR_HEIGHT;
        glm::vec3 newPoint(x, y, 0.0f);
        ccpoints.push_back(newPoint);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    
        if (action == GLFW_PRESS) {

            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            
            float x = (2.0f * xpos) / SCR_WIDTH - 1.0f;
            float y = 1.0f - (2.0f * ypos) / SCR_HEIGHT;


            for (auto& p : ccpoints) {
                if (abs(x - p.x) < 0.03f && abs(y - p.y) < 0.03f) {
                    selectedPoint = &p;

                    

                    break;
                }
            }
        }
        else if (action == GLFW_RELEASE) {
            selectedPoint = nullptr;
        }
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && selectedPoint != nullptr) {
        float x = (2.0f * xpos) / SCR_WIDTH - 1.0f;
        float y = 1.0f - (2.0f * ypos) / SCR_HEIGHT;
        *selectedPoint = glm::vec3(x, y, 0.0f);
    }
}



static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ENTER:
            startGame.store(true);
            break;
        }
    }



    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_UP:
            if (snakeDirection != DOWN)
                snakeDirection = UP;
            break;
        case GLFW_KEY_DOWN:
            if (snakeDirection != UP)
                snakeDirection = DOWN;
            break;
        case GLFW_KEY_LEFT:
            if (snakeDirection != RIGHT)
                snakeDirection = LEFT;
            break;
        case GLFW_KEY_RIGHT:
            if (snakeDirection != LEFT)
                snakeDirection = RIGHT;
            break;
        }
    }

}


static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


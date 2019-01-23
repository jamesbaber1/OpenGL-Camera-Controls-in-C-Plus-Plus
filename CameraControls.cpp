//James Baber
// 271 Project 5

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "src/SOIL.h"
#include <cmath>


using namespace std;
using namespace glm;

const char* vertex_shader =
"#version 400\n"
"in vec3 vp;"
"in vec3 inColor;"
"in vec2 texcoord;"
"out vec3 vertexColor;"
"out vec2 Texcoord;"
"uniform mat4 modelMat;"
"uniform mat4 viewMat;"
"uniform mat4 projMat;"
"void main() {"
"  vertexColor = inColor;"
"  Texcoord = texcoord;"
"  gl_Position = projMat * viewMat * modelMat * vec4(vp, 1.0);"
"}";


const char* fragment_shader =
"#version 400\n"
"in vec3 vertexColor;"
"in vec2 Texcoord;"
"out vec4 frag_colour;"
"uniform sampler2D tex;"
"void main() {"
"  frag_colour = texture(tex, Texcoord);"
"}";

float fov = 45.0f;

/////////////// declare print function ///////////////////////////
void printMatrix(const float*, int);
void scroll_callback(GLFWwindow*, double, double);

int main(void)
{
    GLFWwindow* window;
    
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 640, "Ground", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        cout << "Glew error" << endl;
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    
    float points[] = {
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f
    };
    
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
    
    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);
    
    GLint posAttrib = glGetAttribLocation(shader_programme, "vp");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
    
    GLint colAttrib = glGetAttribLocation(shader_programme, "inColor");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    
    GLint texAttrib = glGetAttribLocation(shader_programme, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    
    int width, height;
    unsigned char* image = SOIL_load_image("kitten.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    cout<< width << ", "<< height << endl;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    vec3 cubePositions[] = {
        vec3( 0.0f,  0.0f,  0.0f),
        vec3( 1.0f,  0.0f,  0.0f),
        vec3( 2.0f,  0.0f,  0.0f),
        vec3( 3.0f,  0.0f,  0.0f),
        vec3( 4.0f,  0.0f,  0.0f),
        vec3( 5.0f,  0.0f,  0.0f),
        vec3( -1.0f,  0.0f,  0.0f),
        vec3( -2.0f,  0.0f,  0.0f),
        vec3( -3.0f,  0.0f,  0.0f),
        vec3( -4.0f,  0.0f,  0.0f),
        vec3( -5.0f,  0.0f,  0.0f),
        vec3( 0.0f,  0.0f,  1.0f),
        vec3( 1.0f,  0.0f,  1.0f),
        vec3( 2.0f,  0.0f,  1.0f),
        vec3( 3.0f,  0.0f,  1.0f),
        vec3( 4.0f,  0.0f,  1.0f),
        vec3( 5.0f,  0.0f,  1.0f),
        vec3( -1.0f,  0.0f,  1.0f),
        vec3( -2.0f,  0.0f,  1.0f),
        vec3( -3.0f,  0.0f,  1.0f),
        vec3( -4.0f,  0.0f,  1.0f),
        vec3( -5.0f,  0.0f,  1.0f),
        vec3( 0.0f,  0.0f,  2.0f),
        vec3( 1.0f,  0.0f,  2.0f),
        vec3( 2.0f,  0.0f,  2.0f),
        vec3( 3.0f,  0.0f,  2.0f),
        vec3( 4.0f,  0.0f,  2.0f),
        vec3( 5.0f,  0.0f,  2.0f),
        vec3( -1.0f,  0.0f,  2.0f),
        vec3( -2.0f,  0.0f,  2.0f),
        vec3( -3.0f,  0.0f,  2.0f),
        vec3( -4.0f,  0.0f,  2.0f),
        vec3( -5.0f,  0.0f,  2.0f),
        vec3( 0.0f,  0.0f,  3.0f),
        vec3( 1.0f,  0.0f,  3.0f),
        vec3( 2.0f,  0.0f,  3.0f),
        vec3( 3.0f,  0.0f,  3.0f),
        vec3( 4.0f,  0.0f,  3.0f),
        vec3( 5.0f,  0.0f,  3.0f),
        vec3( -1.0f,  0.0f,  3.0f),
        vec3( -2.0f,  0.0f,  3.0f),
        vec3( -3.0f,  0.0f,  3.0f),
        vec3( -4.0f,  0.0f,  3.0f),
        vec3( -5.0f,  0.0f,  3.0f),
        vec3( 0.0f,  0.0f,  4.0f),
        vec3( 1.0f,  0.0f,  4.0f),
        vec3( 2.0f,  0.0f,  4.0f),
        vec3( 3.0f,  0.0f,  4.0f),
        vec3( 4.0f,  0.0f,  4.0f),
        vec3( 5.0f,  0.0f,  4.0f),
        vec3( -1.0f,  0.0f,  4.0f),
        vec3( -2.0f,  0.0f,  4.0f),
        vec3( -3.0f,  0.0f,  4.0f),
        vec3( -4.0f,  0.0f,  4.0f),
        vec3( -5.0f,  0.0f,  4.0f),
        vec3( 0.0f,  0.0f,  5.0f),
        vec3( 1.0f,  0.0f,  5.0f),
        vec3( 2.0f,  0.0f,  5.0f),
        vec3( 3.0f,  0.0f,  5.0f),
        vec3( 4.0f,  0.0f,  5.0f),
        vec3( 5.0f,  0.0f,  5.0f),
        vec3( -1.0f,  0.0f,  5.0f),
        vec3( -2.0f,  0.0f,  5.0f),
        vec3( -3.0f,  0.0f,  5.0f),
        vec3( -4.0f,  0.0f,  5.0f),
        vec3( -5.0f,  0.0f,  5.0f),
        vec3( 0.0f,  0.0f,  -1.0f),
        vec3( 1.0f,  0.0f,  -1.0f),
        vec3( 2.0f,  0.0f,  -1.0f),
        vec3( 3.0f,  0.0f,  -1.0f),
        vec3( 4.0f,  0.0f,  -1.0f),
        vec3( 5.0f,  0.0f,  -1.0f),
        vec3( -1.0f,  0.0f,  -1.0f),
        vec3( -2.0f,  0.0f,  -1.0f),
        vec3( -3.0f,  0.0f,  -1.0f),
        vec3( -4.0f,  0.0f,  -1.0f),
        vec3( -5.0f,  0.0f,  -1.0f),
        vec3( 0.0f,  0.0f,  -2.0f),
        vec3( 1.0f,  0.0f,  -2.0f),
        vec3( 2.0f,  0.0f,  -2.0f),
        vec3( 3.0f,  0.0f,  -2.0f),
        vec3( 4.0f,  0.0f,  -2.0f),
        vec3( 5.0f,  0.0f,  -2.0f),
        vec3( -1.0f,  0.0f,  -2.0f),
        vec3( -2.0f,  0.0f,  -2.0f),
        vec3( -3.0f,  0.0f,  -2.0f),
        vec3( -4.0f,  0.0f,  -2.0f),
        vec3( -5.0f,  0.0f,  -2.0f),
        vec3( 0.0f,  0.0f,  -3.0f),
        vec3( 1.0f,  0.0f,  -3.0f),
        vec3( 2.0f,  0.0f,  -3.0f),
        vec3( 3.0f,  0.0f,  -3.0f),
        vec3( 4.0f,  0.0f,  -3.0f),
        vec3( 5.0f,  0.0f,  -3.0f),
        vec3( -1.0f,  0.0f,  -3.0f),
        vec3( -2.0f,  0.0f,  -3.0f),
        vec3( -3.0f,  0.0f,  -3.0f),
        vec3( -4.0f,  0.0f,  -3.0f),
        vec3( -5.0f,  0.0f,  -3.0f),
        vec3( 0.0f,  0.0f,  -4.0f),
        vec3( 1.0f,  0.0f,  -4.0f),
        vec3( 2.0f,  0.0f,  -4.0f),
        vec3( 3.0f,  0.0f,  -4.0f),
        vec3( 4.0f,  0.0f,  -4.0f),
        vec3( 5.0f,  0.0f,  -4.0f),
        vec3( -1.0f,  0.0f,  -4.0f),
        vec3( -2.0f,  0.0f,  -4.0f),
        vec3( -3.0f,  0.0f,  -4.0f),
        vec3( -4.0f,  0.0f,  -4.0f),
        vec3( -5.0f,  0.0f,  -4.0f),
        vec3( 0.0f,  0.0f,  -5.0f),
        vec3( 1.0f,  0.0f,  -5.0f),
        vec3( 2.0f,  0.0f,  -5.0f),
        vec3( 3.0f,  0.0f,  -5.0f),
        vec3( 4.0f,  0.0f,  -5.0f),
        vec3( 5.0f,  0.0f,  -5.0f),
        vec3( -1.0f,  0.0f,  -5.0f),
        vec3( -2.0f,  0.0f,  -5.0f),
        vec3( -3.0f,  0.0f,  -5.0f),
        vec3( -4.0f,  0.0f,  -5.0f),
        vec3( -5.0f,  0.0f,  -5.0f),
        
    };
    
    mat4 modelMat = mat4(1.0f);
    unsigned int modelMatLoc = glGetUniformLocation(shader_programme, "modelMat");
    
    mat4 viewMat = mat4(1.0f);
    viewMat = translate(viewMat, vec3(0.0f, 0.0f, -0.1f));
    unsigned int viewMatLoc = glGetUniformLocation(shader_programme, "viewMat");
    
    mat4 projMat = perspective(radians(45.0f), 640/480.0f, 0.1f, 100.0f);
    unsigned int projMatLoc = glGetUniformLocation(shader_programme, "projMat");
    
    
    //////////////// start //////////////////////
    vec3 cameraPosition = vec3(0.0f, 0.0f, 3.0f);
    vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    vec3 cameraDirection = normalize(cameraTarget - cameraPosition);
    vec3 cameraFront = normalize( cameraDirection + vec3(0.0f, 0.0f, -1.0f));
    vec3 cameraJump = normalize( cameraDirection + vec3(0.0f, 1.0f, 0.0f));
    float speed = 0.01;
    
    float deltaTime = 0.0f;
    float lastFrameTime = 0.0f;
    float currentFrameTime = 0.0f;
    ////////////// end ///////////////////////
    int jumpCounter = 0;
    
    /* Loop until the user closes the window */
    while(!glfwWindowShouldClose(window)) {
        // wipe the drawing surface clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_programme);
        
        
        /////////////// start ////////////////////////////////////////////
        currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
        //cout<<deltaTime << endl;
        
        /////////////// start ////////////////////////////////////////////
        glfwSetScrollCallback(window, scroll_callback);
        projMat = perspective(radians(fov), 640/480.0f, 0.1f, 100.0f);
        ///////////////////// end /////////////////////////////////////////////
        
        
        
        speed = 5 * deltaTime;
        
        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            cameraPosition += cameraFront * speed;
            cameraTarget += cameraFront * speed;
        }
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            cameraPosition -= cameraFront * speed;
            cameraTarget -= cameraFront * speed;
        }
        if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            vec3 cameraLeft = cross(up, cameraFront);
            cameraPosition += cameraLeft * speed;
            cameraTarget += cameraLeft * speed;
        }
        if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            vec3 cameraRight = cross(cameraFront, up);
            cameraPosition += cameraRight * speed;
            cameraTarget += cameraRight * speed;
        }
        
        if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            cameraPosition += cameraJump * speed;
            cameraTarget += cameraJump * speed;
            jumpCounter++;
        }
        
        else if(jumpCounter>0)
        {
            cameraPosition -= cameraJump * speed;
            cameraTarget -= cameraJump * speed;
            jumpCounter--;
        }
        
        viewMat = lookAt(cameraPosition, cameraTarget, up);
        
        glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, value_ptr(viewMat));
        glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));
        
        for(unsigned int i = 0; i < 121; i++)
        {
            mat4 model = translate(modelMat, cubePositions[i]);
            model = rotate(model, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
            //model = scale(model, vec3(10.0f, 10.0f, 10.0f));
            glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, sizeof(points)/sizeof(float)/8);
        }
        ///////////////////// end /////////////////////////////////////////////
        
        glfwPollEvents();
        glfwSwapInterval(1);
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}

void printMatrix(const float* mat, int dim)
{
    for (int j = 0; j < dim; j++)
    {
        for(int i=0; i<dim; i++)
            cout<< fixed << mat[i*dim+j]<<", ";
        
        cout<<endl;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov += yoffset * 3;
}

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <string>
#include <iostream>

float translate_x_mod = 0.f;
float translate_y_mod = 0.f;
float rotate_x_mod = 0.f;
float rotate_y_mod = 1.f;
float scale_mod = 1.f;
float zoom_mod = -5.f;

void Key_Callback(GLFWwindow* window,
    int key, //keycode of press
    int scancode, //physical position of press
    int action, //press or release
    int mods) //modifier keys
{
    if (key == GLFW_KEY_W &&
        (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        //move bunny up
        translate_y_mod += 0.25f;
    }

    if (key == GLFW_KEY_S &&
        (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        //move bunny down
        translate_y_mod -= 0.25f;
    }

    if (key == GLFW_KEY_D &&
        (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        //move bunny right
        translate_x_mod += 0.25f;
    }

    if (key == GLFW_KEY_A &&
        (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        //move bunny left
        translate_x_mod -= 0.25f;
    }
    if (key == GLFW_KEY_RIGHT &&
        (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        //rotate bunny to right
        rotate_x_mod += 15.f;
    }
    if (key == GLFW_KEY_LEFT &&
        (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        //rotate bunny to left
        rotate_x_mod -= 15.f;
    }
    if (key == GLFW_KEY_UP &&
        (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        //rotate bunny up
        rotate_y_mod += 15.f;
    }

    if (key == GLFW_KEY_DOWN &&
        (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        //rotate bunny down
        rotate_y_mod -= 15.f;
    }
    if (key == GLFW_KEY_Q &&
        (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        //decrease scale
        scale_mod -= 0.2f;
    }
    if (key == GLFW_KEY_E &&
        (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        //increase scale
        scale_mod += 0.2f;
    }
    if (key == GLFW_KEY_Z &&
        (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        //zoom bunny in
        zoom_mod += 0.15f;
    }

    if (key == GLFW_KEY_X &&
        (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        //zoom bunny out
        zoom_mod -= 0.15f;
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    float window_width = 800.0f;
    float window_height = 800.0f;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(window_width, window_height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    /*glViewport(0, //min x
        0, //min y
        640, //x
        480); //y */

    int img_width, //texture width
        img_height, //texture height
        colorChannels; //number of collor channels

    //flip texture
    stbi_set_flip_vertically_on_load(true);
    //load texture
    unsigned char* tex_bytes = stbi_load("3D/brickwall.jpg", //texture path
        &img_width, //fill out width
        &img_height, //fill out height
        &colorChannels, //fill out colors
        0);

    //opengl reference to textures
    GLuint texture;
    //generate reference
    glGenTextures(1, &texture);
    //set current texture working on
    glActiveTexture(GL_TEXTURE0);
    //bind texture
    glBindTexture(GL_TEXTURE_2D, texture);


    //assing texture to opengl reference
    glTexImage2D(GL_TEXTURE_2D,
        0, //texture 0
        GL_RGB, //target color format
        img_width, //texture width
        img_height, //texture height
        0,
        GL_RGB, //color format of texture
        GL_UNSIGNED_BYTE,
        tex_bytes); //load texture in bytes

    //generate mipmap
    glGenerateMipmap(GL_TEXTURE_2D);
    //free up loaded bytes
    stbi_image_free(tex_bytes);

    //below are loading normals
    int img_width2, img_height2, colorChannel2;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* normal_bytes = stbi_load("3D/brickwall_normal.jpg",
        &img_width2,
        &img_height2,
        &colorChannel2,
        0);

    //opengl reference to normal textures
    GLuint norm_tex;
    //generate normal reference
    glGenTextures(1, &norm_tex);
    //set normals since texture 0 is used alrready
    glActiveTexture(GL_TEXTURE1);
    //bind normal texture
    glBindTexture(GL_TEXTURE_2D, norm_tex);
    //set parameters to be the same as diffuse color
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    //assing normal texture to opengl reference
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        img_width2,
        img_height2,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        normal_bytes);

    //generate mipmap of normal map
    glGenerateMipmap(GL_TEXTURE_2D);
    //free up loaded bytes
    stbi_image_free(normal_bytes);

    //load skybox textures
    std::string facesSkybox[]{
        "Skybox/rainbow_rt.png", //right
        "Skybox/rainbow_lf.png", //left
        "Skybox/rainbow_up.png", //up
        "Skybox/rainbow_dn.png", //down
        "Skybox/rainbow_ft.png", //front
        "Skybox/rainbow_bk.png"  //back
    };

    //initialize skybox texture
    unsigned int skyboxTex;
    //generate sky textures
    glGenTextures(1, &skyboxTex);
    //initialize as cubemap
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
    //filtering cubemap
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //stretch to edge
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++) {
        //height, width, color channel
        int w, h, skyCChannel;
        //temporarily disable flipping image
        stbi_set_flip_vertically_on_load(false);
        //load image
        unsigned char* data = stbi_load(facesSkybox[i].c_str(), &w, &h, &skyCChannel, 0);

        if (data) {
            glTexImage2D(
                //cubemap address start at positive x
                //increment 1
                // right to left to top to bottom to front to back
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                w,
                h,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );
            //cleanup
            stbi_image_free(data);
        }
    }
    //re-enable image flip
    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    glfwSetKeyCallback(window, Key_Callback);

    //load vert shader
    std::fstream vertSrc("Shaders/sample.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();
    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    //load frag shader
    std::fstream fragSrc("Shaders/sample.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    //load sky vert shader
    std::fstream skyVertSrc("Shaders/skybox.vert");
    std::stringstream skyVertBuff;
    skyVertBuff << skyVertSrc.rdbuf();
    std::string skyVertS = skyVertBuff.str();
    const char* skyV = skyVertS.c_str();

    //load sky frag shader
    std::fstream skyFragSrc("Shaders/skybox.frag");
    std::stringstream skyFragBuff;
    skyFragBuff << skyFragSrc.rdbuf();
    std::string skyFragS = skyFragBuff.str();
    const char* skyF = skyFragS.c_str();

    //add vertex shader
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &v, NULL);
    glCompileShader(vertShader);

    GLint isCompiled = 0;

    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(vertShader, maxLength, &maxLength, &errorLog[0]);
        std::cout << &errorLog[0];
        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(vertShader); // Don't leak the shader.
    }

    //add frag shader
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &f, NULL);
    glCompileShader(fragShader);

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(fragShader, maxLength, &maxLength, &errorLog[0]);
        std::cout << &errorLog[0];
        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(fragShader); // Don't leak the shader.
    }

    //add sky vertex shader
    GLuint skyVertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(skyVertShader, 1, &skyV, NULL);
    glCompileShader(skyVertShader);

    glGetShaderiv(skyVertShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(skyVertShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(skyVertShader, maxLength, &maxLength, &errorLog[0]);
        std::cout << &errorLog[0];
        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(skyVertShader); // Don't leak the shader.
    }

    //add sky frag shader
    GLuint skyFragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(skyFragShader, 1, &skyF, NULL);
    glCompileShader(skyFragShader);

    glGetShaderiv(skyFragShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(skyFragShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(skyFragShader, maxLength, &maxLength, &errorLog[0]);
        std::cout << &errorLog[0];
        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(skyFragShader); // Don't leak the shader.
    }

    //create shader program and attach compiled shaders
    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertShader);
    glAttachShader(shaderProg, fragShader);

    //finalize the compilation process
    glLinkProgram(shaderProg);

    GLuint skyShaderProg = glCreateProgram();
    glAttachShader(skyShaderProg, skyVertShader);
    glAttachShader(skyShaderProg, skyFragShader);

    glLinkProgram(skyShaderProg);

    std::string path = "3D/plane.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &material,
        &warning,
        &error,
        path.c_str()
    );

    GLfloat UV[]{
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        0.f, 0.f
    };

    /*
  7--------6
 /|       /|
4--------5 |
| |      | |
| 3------|-2
|/       |/
0--------1
*/
//Vertices for the cube
    float skyboxVertices[]{
        -1.f, -1.f, 1.f, //0
        1.f, -1.f, 1.f,  //1
        1.f, -1.f, -1.f, //2
        -1.f, -1.f, -1.f,//3
        -1.f, 1.f, 1.f,  //4
        1.f, 1.f, 1.f,   //5
        1.f, 1.f, -1.f,  //6
        -1.f, 1.f, -1.f  //7
    };

    //Skybox Indices
    unsigned int skyboxIndices[]{
        1,2,6,
        6,5,1,

        0,4,7,
        7,3,0,

        4,5,6,
        6,7,4,

        0,3,2,
        2,1,0,

        0,1,5,
        5,4,0,

        3,7,6,
        6,2,3
    };

    //get EBO indices array
    std::vector<GLuint> mesh_indices;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(
            shapes[0].mesh.indices[i].vertex_index
        );
    }

    //vector to hold tangents
    std::vector<glm::vec3> tangents;
    //vector to hold bitangents
    std::vector<glm::vec3> bitangents;

    for (int i = 0; i < shapes[0].mesh.indices.size(); i += 3) {
        //get vertex data for triangle
        tinyobj::index_t vData1 = shapes[0].mesh.indices[i];
        tinyobj::index_t vData2 = shapes[0].mesh.indices[i + 1];
        tinyobj::index_t vData3 = shapes[0].mesh.indices[i + 2];

        //position of vertex 1
        glm::vec3 v1 = glm::vec3(
            attributes.vertices[vData1.vertex_index * 3],
            attributes.vertices[(vData1.vertex_index * 3) + 1],
            attributes.vertices[(vData1.vertex_index * 3) + 2]
        );

        //position of vertex 2
        glm::vec3 v2 = glm::vec3(
            attributes.vertices[vData2.vertex_index * 3],
            attributes.vertices[(vData2.vertex_index * 3) + 1],
            attributes.vertices[(vData2.vertex_index * 3) + 2]
        );

        //position of vertex 3
        glm::vec3 v3 = glm::vec3(
            attributes.vertices[vData3.vertex_index * 3],
            attributes.vertices[(vData3.vertex_index * 3) + 1],
            attributes.vertices[(vData3.vertex_index * 3) + 2]
        );

        //uv of vertex 1
        glm::vec2 uv1 = glm::vec2(
            attributes.texcoords[(vData1.texcoord_index * 2)],
            attributes.texcoords[(vData1.texcoord_index * 2) + 1]
        );

        //uv of vertex 2
        glm::vec2 uv2 = glm::vec2(
            attributes.texcoords[(vData2.texcoord_index * 2)],
            attributes.texcoords[(vData2.texcoord_index * 2) + 1]
        );

        //uv of vertex 3
        glm::vec2 uv3 = glm::vec2(
            attributes.texcoords[(vData3.texcoord_index * 2)],
            attributes.texcoords[(vData3.texcoord_index * 2) + 1]
        );

        //edges of triangle: position delta
        glm::vec3 deltaPos1 = v2 - v1;
        glm::vec3 deltaPos2 = v3 - v1;

        //uv delta
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float r = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));
        //tangent (T)
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        //bitangent (B)
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        //push back tangent and bitangent three times
        tangents.push_back(tangent);
        tangents.push_back(tangent);
        tangents.push_back(tangent);

        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
    }

    //initialize array of vertex data
    std::vector<GLfloat> fullVertexData;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];

        //push x position
        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3)]
        );
        //push y position
        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 1]
        );
        //push z position
        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 2]
        );
        //push x normals
        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3)]
        );
        //push y normals
        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 1]
        );
        //push z normals
        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 2]
        );
        //push u position
        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2)]
        );
        //push v position
        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2) + 1]
        );

        //push tangent and bitangent to vertex data
        fullVertexData.push_back(
            tangents[i].x
        );

        fullVertexData.push_back(
            tangents[i].y
        );

        fullVertexData.push_back(
            tangents[i].z
        );

        fullVertexData.push_back(
            bitangents[i].x
        );

        fullVertexData.push_back(
            bitangents[i].y
        );

        fullVertexData.push_back(
            bitangents[i].z
        );
    }

    GLuint VAO, VBO, EBO, VBO_UV;
    //Initialize VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &VBO_UV);
    //glGenBuffers(1, &EBO);

    unsigned int skyVAO, skyVBO, skyEBO;
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glGenBuffers(1, &skyEBO);

    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &skyboxIndices, GL_STATIC_DRAW);

    //enable vertices
    glEnableVertexAttribArray(0);

    //tell opengl to use shader for VAOs below
    glUseProgram(shaderProg);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //add in vertex data
    glBufferData(
        GL_ARRAY_BUFFER,
        //size of array in bytes
        sizeof(GLfloat) * fullVertexData.size(),
        //data of array
        fullVertexData.data(),
        GL_DYNAMIC_DRAW
    );

    glVertexAttribPointer(
        0, //vertex position
        3, // x y z
        GL_FLOAT, //data type of array
        GL_FALSE,
        //vertex data has 8 floats
        //X,Y,Z,3 normals, U,V
        14 * sizeof(float), //size of vertex data in bytes
        (void*)0
    );
    //enable vertices, this time for skybox
    glEnableVertexAttribArray(0);

    //normal starts at index 3
    GLintptr normalPtr = 3 * sizeof(float);

    glVertexAttribPointer(
        1, //normals
        3, // x y z
        GL_FLOAT, //data type of array
        GL_FALSE,
        //vertex data has 8 floats
        //X,Y,Z,3 normals, U,V
        14 * sizeof(float), //size of vertex data in bytes
        (void*)normalPtr
    );
    //enable normals
    glEnableVertexAttribArray(1);

    //uv starts at index 6
    GLintptr uvPtr = 6 * sizeof(float);

    //get uv data from array
    glVertexAttribPointer(
        2, //uv coords
        2, // u v
        GL_FLOAT, //data type of array
        GL_FALSE,
        //vertex data has 8 floats
        //X,Y,Z,3 normals, U,V
        14 * sizeof(float), //size of vertex data in bytes
        //offset
        (void*)uvPtr
    );

    //enable textures
    glEnableVertexAttribArray(2);

    //tangent starts at index 8
    GLintptr tangentPtr = 8 * sizeof(float);

    glVertexAttribPointer(
        3, //tangent
        3, // T (xyz)
        GL_FLOAT, //data type of array
        GL_FALSE,
        14 * sizeof(float), //size of vertex data in bytes
        //offset
        (void*)tangentPtr
    );
    //enable tangent
    glEnableVertexAttribArray(3);

    //bitangent starts at index 11
    GLintptr bitangentPtr = 11 * sizeof(float);

    glVertexAttribPointer(
        4, //bitangent
        3, // T (xyz)
        GL_FLOAT, //data type of array
        GL_FALSE,
        14 * sizeof(float), //size of vertex data in bytes
        //offset
        (void*)bitangentPtr
    );
    //enable bitangent
    glEnableVertexAttribArray(4);

    /*
    //VBO
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * attributes.vertices.size(), //bytes
        &attributes.vertices[0],         // == attributes.vertices.data()
        GL_STATIC_DRAW);

    glVertexAttribPointer(
        0, //0 pos, 1 tex, 2 norms
        3, //xyz
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GL_FLOAT),
        (void*)0
    ); */

    /*
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * mesh_indices.size(),
        &mesh_indices[0],
        GL_STATIC_DRAW);*/

        /*
        //bind uv
        glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
        //buffer data for uv
        glBufferData(GL_ARRAY_BUFFER,
            sizeof(GLfloat)* (sizeof(UV) / sizeof(UV[0])), //float * size of uv array
            &UV[0], //uv array
            GL_DYNAMIC_DRAW); */

            //interpret uv array
            /* glVertexAttribPointer(
                2, //2 for uv coords
                2, //UV
                GL_FLOAT, //type of array
                GL_FALSE,
                2 * sizeof(float), //every 2 index
                (void*)0
            );*/

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    float x, y, z;
    x = y = z = 0.0f;
    z = -20.0f;
    glm::mat3 identity_matrix3 = glm::mat3(1.0f);
    glm::mat4 identity_matrix4 = glm::mat4(1.0f);

    float scale_x, scale_y, scale_z = 1.f;
    scale_x = scale_y = scale_z = 1.f;

    float axis_x, axis_y, axis_z, theta = 0.f;
    axis_x = axis_y = axis_z = theta = 0.f;
    axis_y = 1.f;

    /*glm::mat4 projectionMatrix = glm::ortho(
        -1.f, //left
        1.f, //right
        -1.f, //bottom
        1.f, //top
        -1.f, //znear
        1.f //zfar
    );*/

    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(90.f), //fov
        window_height / window_width, //aspect ratio
        0.1f, //znear > 0
        200.0f //zfar
    );

    //set camera position
    glm::vec3 cameraPos = glm::vec3(rotate_x_mod, 0, 100.f);

    glm::mat4 cameraPositionMatrix = glm::translate(glm::mat4(1.0f),
        cameraPos * -1.0f);
    //world up
    glm::vec3 WorldUp = glm::vec3(0, 1.0f, 0);
    //camera center
    glm::vec3 Center = glm::vec3(0, 3.f, 0);

    glm::vec3 F = glm::vec3(Center - cameraPos);
    F = glm::normalize(F);

    glm::vec3 R = glm::normalize(glm::cross(F, WorldUp));

    glm::vec3 U = glm::normalize(glm::cross(R, F));

    //camera orientation matrix
    glm::mat4 cameraOrientation = glm::mat4(1.f);

    //assign matrix
    //Matrix[Column][Row]
    cameraOrientation[0][0] = R.x;
    cameraOrientation[1][0] = R.y;
    cameraOrientation[2][0] = R.z;

    cameraOrientation[0][1] = U.x;
    cameraOrientation[1][1] = U.y;
    cameraOrientation[2][1] = U.z;

    cameraOrientation[0][2] = -F.x;
    cameraOrientation[1][2] = -F.y;
    cameraOrientation[2][2] = -F.z;

    glm::mat4 viewMatrix = cameraOrientation * cameraPositionMatrix;

    /*glm::mat4 viewMatrix = glm::lookAt(
        glm::vec3(rotate_x_mod, 0, 10.f), //eye
        glm::vec3(0, 3.f, 0), //center
        glm::vec3(0, 1.0f, 0) //worldup
    );*/

    //light position
    glm::vec3 lightPos = glm::vec3(-10, 3, 0);
    //diff light color
    glm::vec3 lightColor = glm::vec3(2, 2, 2);
    //ambient strength
    float ambientStr = 0.1f;
    //ambient color
    glm::vec3 ambientColor = lightColor;
    //spec strength
    float specStr = 0.5f;
    //spec phong
    float specPhong = 16;

    //enable blending
    glEnable(GL_BLEND);

    //blending function
    glBlendFunc(GL_SRC_ALPHA, //source factor
        GL_ONE_MINUS_SRC_ALPHA //destination factor
    );

    glBlendEquation(GL_FUNC_ADD);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        x = translate_x_mod;
        y = translate_y_mod;
        z = zoom_mod;
        theta = rotate_x_mod;
        axis_y = rotate_y_mod;

        glm::mat4 transformation_matrix = glm::translate(
            identity_matrix4,
            glm::vec3(x, y, z)
        );

        transformation_matrix = glm::scale(
            transformation_matrix,
            glm::vec3(scale_x + scale_mod, scale_y + scale_mod, scale_z)
        );

        transformation_matrix = glm::rotate(
            transformation_matrix,
            glm::radians(theta),
            glm::normalize(glm::vec3(axis_x, axis_y, axis_z))
        );

        transformation_matrix = glm::rotate(
            transformation_matrix,
            glm::radians(axis_y),
            glm::normalize(glm::vec3(1.0f, 0.0f, axis_z))
        );

        //rotate_x_mod += 0.05f;

        //projection
        unsigned int projectionLoc = glGetUniformLocation(shaderProg, "projection");

        glUniformMatrix4fv(projectionLoc, //address of projection variable
            1, //how many matrices to assign
            GL_FALSE, //transpose
            glm::value_ptr(projectionMatrix) //pointer to matrix
        );

        //camera
        unsigned int viewLoc = glGetUniformLocation(shaderProg, "view");

        glUniformMatrix4fv(viewLoc, //address of view variable
            1, //how many matrices to assign
            GL_FALSE, //transpose
            glm::value_ptr(viewMatrix) //pointer to matrix
        );

        //disable mask
        glDepthMask(GL_FALSE);
        //change depth function into <=
        glDepthFunc(GL_LEQUAL);
        //use skybox texture
        glUseProgram(skyShaderProg);

        glm::mat4 sky_view = glm::mat4(1.f);
        sky_view = glm::mat4(
            glm::mat3(viewMatrix)
        );

        unsigned int skyViewLoc = glGetUniformLocation(skyShaderProg, "view");
        glUniformMatrix4fv(skyViewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(sky_view)
        );

        unsigned int skyProjLoc = glGetUniformLocation(skyShaderProg, "projection");
        glUniformMatrix4fv(skyProjLoc,
            1,
            GL_FALSE,
            glm::value_ptr(projectionMatrix)
        );

        //bind skybox vao
        glBindVertexArray(skyVAO);
        //set texture index to use
        glActiveTexture(GL_TEXTURE0);
        //bind cubemap to texture index
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        //draw skybox
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //reset depth to true
        glDepthMask(GL_TRUE);
        //reset depth to normal
        glDepthFunc(GL_LESS);

        //draw other stuff below
        glUseProgram(shaderProg);

        //model translation
        //get location of transform variable in shader
        unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");

        //assign matrix
        glUniformMatrix4fv(transformLoc, //address of transform variable
            1, //how many matrices to assign
            GL_FALSE, //transpose
            glm::value_ptr(transformation_matrix) //pointer to matrix
        );

        glBindVertexArray(VAO);

        //get location of tex0
        GLuint tex0Loc = glGetUniformLocation(shaderProg, "tex0");
        //tell opengl to use textures
        glBindTexture(GL_TEXTURE_2D, texture);
        //use texture at 0
        glUniform1i(tex0Loc, 0);

        // Normal map index
        glActiveTexture(GL_TEXTURE1);
        // Get the uniform location for norm_tex
        GLint tex1Loc = glGetUniformLocation(shaderProg, "norm_tex");
        // Bind the normal map texture to texture unit 1
        glBindTexture(GL_TEXTURE_2D, norm_tex);
        // Pass texture unit index to the shader uniform
        glUniform1i(tex1Loc, 1);

        //get address of light position from shader
        GLuint lightAddress = glGetUniformLocation(shaderProg, "lightPos");
        glUniform3fv(lightAddress, 1, glm::value_ptr(lightPos));
        //get address of light color from shader
        GLuint lightColorAddress = glGetUniformLocation(shaderProg, "lightColor");
        glUniform3fv(lightColorAddress, 1, glm::value_ptr(lightColor));

        //get address of ambient strength from shader
        GLuint ambientStrAddress = glGetUniformLocation(shaderProg, "ambientStr");
        glUniform1f(ambientStrAddress, ambientStr);
        //get address of ambient color from shader
        GLuint ambientColorAddress = glGetUniformLocation(shaderProg, "ambientColor");
        glUniform3fv(ambientColorAddress, 1, glm::value_ptr(ambientColor));
        //get address of camerapos from shader
        GLuint cameraPosAddress = glGetUniformLocation(shaderProg, "cameraPos");
        glUniform3fv(cameraPosAddress, 1, glm::value_ptr(cameraPos));
        //get address of spec str from shader
        GLuint specStrAddress = glGetUniformLocation(shaderProg, "specStr");
        glUniform1f(specStrAddress, specStr);
        //get address of spec phong from shader
        GLuint specPhongAddress = glGetUniformLocation(shaderProg, "specPhong");
        glUniform1f(specPhongAddress, specPhong);

        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 14);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
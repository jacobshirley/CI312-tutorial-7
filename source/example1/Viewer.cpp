#pragma once

#include "Viewer.h";

// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include <iostream>
#include <vector>
#include "common/shader.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"
#include "common/controls.hpp"
#include <glm/gtx/transform.hpp>

Viewer::Viewer(int width, int height) {

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->width = width;
    this->height = height;

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(width * 2, height, "Basic Example", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    //we set the cursor at the centre so that it always start on our origin (0,0,0)
    glfwSetCursorPos(window, width / 2, height / 2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile


    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return;
    }

    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
}

void Viewer::addTriangleWithViewport(int x, int y) {
    glViewport(x, y, width, height);

    //********Add the Geometry
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(
        1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // Draw the geometry !
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void Viewer::setup() {
    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f
    };

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);


    static const GLfloat g_color_buffer_data[] = {
        0.0,  1.0,  0.0f,
        0.0,  1.0,  0.0f,
        0.0,  1.0,  0.0f,
    };

    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    this->programId = LoadShaders("SimpleVertexShader.hlsl", "SimpleFragmentShader.hlsl");
}

void Viewer::render() {
    do {
        // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glUseProgram(this->programId);
        computeMatricesFromInputs(window, width, height);

        addTriangleWithViewport(0, 0);
        addTriangleWithViewport(width, 0);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        //std::cout << "DEBUG" << std::endl;
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);
}

void Viewer::cleanup() {
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteProgram(programId);
    glDeleteVertexArrays(1, &VertexArrayID);


    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}
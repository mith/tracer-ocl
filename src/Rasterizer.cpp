#include "Rasterizer.hpp"
#include "Utils.hpp"
#include "GLutils.hpp"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>

Rasterizer::Rasterizer()
    : framebuffer(0)
    , color_renderbuffer(0)
    , depth_renderbuffer(0)
{
    load_shaders();
    glGenVertexArrays(1, &vao);
}

void Rasterizer::load_shaders()
{
    auto vertex_shader_src = file_to_str(shaders_dir + shader_filenames[0]);
    auto fragment_shader_src = file_to_str(shaders_dir + shader_filenames[1]);
    shader = CreateProgram({CreateShader(GL_VERTEX_SHADER, vertex_shader_src),
                     CreateShader(GL_FRAGMENT_SHADER, fragment_shader_src)});
}

void Rasterizer::set_scene(const Scene& scene)
{
    current_scene = &scene;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, scene.glview.vertexBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, scene.glview.vertexAttributesBuffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributes), nullptr);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttributes), 
                          (void*)offsetof(struct VertexAttributes, texcoord));
}

void Rasterizer::set_texture(GLuint texture, int width, int height)
{
    target_texture = texture;
    this->width = width;
    this->height = height;

    glBindVertexArray(vao);

    if (framebuffer == 0) {
        glGenFramebuffers(1, &framebuffer);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (depth_renderbuffer == 0) {
        glGenRenderbuffers(1, &depth_renderbuffer);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, depth_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depth_renderbuffer);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "framebuffer incomplete." << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);
}

void Rasterizer::render()
{
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);
    glDepthMask(GL_TRUE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "framebuffer incomplete." << std::endl;
    }
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(shader);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_scene->glview.indicesBuffer);

    auto orientationAttrib = glGetUniformLocation(shader, "orientation");
    auto translationAttrib = glGetUniformLocation(shader, "translation");
    auto scaleAttrib = glGetUniformLocation(shader, "scale");
    auto perspMatAttrib = glGetUniformLocation(shader, "perspMat");

    auto perspMat = glm::perspective(glm::radians(80.0f), (float)width/height, 0.5f, 500.0f)
                  * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
                                glm::vec3(0.0f, 0.0f, -1.0f),
                                glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(perspMatAttrib, 1, GL_FALSE, glm::value_ptr(perspMat));

    for (auto & mesh : current_scene->clmeshes) {
        auto rotMat = glm::mat4_cast(mesh.orientation);
        glUniformMatrix4fv(orientationAttrib, 1, GL_FALSE, glm::value_ptr(rotMat));
        glUniform3fv(translationAttrib, 1, (GLfloat*)&mesh.position);
        glUniform3fv(scaleAttrib, 1, (GLfloat*)&mesh.scale);
        glDrawElementsBaseVertex(GL_TRIANGLES, mesh.num_indices,
                                 GL_UNSIGNED_INT, (void*)mesh.base_indice,
                                 mesh.base_vertex);
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glViewport(0, 0, width * 2, height * 2);
}

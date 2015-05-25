#include "Rasterizer.hpp"
#include "Utils.hpp"
#include "GLutils.hpp"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, scene.glview.vertexAttributesBuffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6, nullptr);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6, 
                          (void*)(sizeof(GL_FLOAT) * 3));
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

    if (color_renderbuffer == 0) {
        glGenRenderbuffers(1, &color_renderbuffer);
    }

    glBindRenderbuffer(GL_RENDERBUFFER, color_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);

    if (depth_renderbuffer == 0) {
        glGenRenderbuffers(1, &depth_renderbuffer);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, depth_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                              GL_RENDERBUFFER, color_renderbuffer);
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
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)[0]);
    float depth_clear = 0.0f;
    glClearBufferfv(GL_DEPTH, 0, &depth_clear);

    glUseProgram(shader);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_scene->glview.indicesBuffer);

    auto orientationAttrib = glGetUniformLocation(shader, "orientation");
    auto positionAttrib = glGetUniformLocation(shader, "translation");
    auto scaleAttrib = glGetUniformLocation(shader, "scale");
    auto perspMatAttrib = glGetUniformLocation(shader, "perspMat");

    auto perspMat = glm::perspective(glm::radians(50.0f), (float)width/height, 0.5f, 500.0f);

    glUniformMatrix4fv(perspMatAttrib, 1, GL_FALSE, (GLfloat*)glm::value_ptr(perspMat));

    for (auto & mesh : current_scene->clmeshes) {
        auto rotMat = glm::mat4_cast(mesh.orientation);
        glUniformMatrix4fv(orientationAttrib, 1, GL_FALSE, (GLfloat*)glm::value_ptr(rotMat));
        glUniform3fv(positionAttrib, 1, (GLfloat*)&mesh.position);
        glUniform3fv(scaleAttrib, 1, (GLfloat*)&mesh.scale);
        glDrawElementsBaseVertex(GL_TRIANGLES, mesh.num_triangles,
                                 GL_UNSIGNED_INT, (void*)mesh.base_triangle,
                                 mesh.base_vertex);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBindTexture(GL_TEXTURE_2D, target_texture);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

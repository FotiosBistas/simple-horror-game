#include "Occluder.h"

Occluder::Occluder(size_t screen_width, size_t screen_height){

    GLCall(glGenTextures(1, &depth_map));
    GLCall(glBindTexture(GL_TEXTURE_2D, depth_map));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screen_width, screen_height, 0,
                        GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo));
    GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_map, 0));

    GLCall(GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete! Status: " << status << "\n";
    }
    GLCall(glDrawBuffer(GL_NONE));
    GLCall(glReadBuffer(GL_NONE));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    this->screen_width = screen_width; 
    this->screen_height = screen_height;
}

void Occluder::depth_pass(
    std::shared_ptr<Shader> shader,
    const glm::mat4& view,
    const glm::mat4& projection
) const{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo));
    GLCall(glViewport(0, 0, screen_width, screen_height));
    GLCall(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
    GLCall(glDepthMask(GL_TRUE));
    GLCall(glClear(GL_DEPTH_BUFFER_BIT));

    shader->use();
    shader->set_mat4("uView", view);
    shader->set_mat4("uProj", projection);

    // Occlusion culling will happen after 
    // frustum culling. This means that if an occluder
    // is not visible to the camera we don't need any info on it
    if(!occluder->is_in_frustum()){
        return;
    }

    occluder->draw_depth(shader);

    GLCall(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GLCall(glUseProgram(0));
}

void Occluder::create_queries() const{
    if (queries.size() != occludees.size()) {
        queries.resize(occludees.size());
        glGenQueries(static_cast<GLsizei>(queries.size()), queries.data());
    }
}

void Occluder::occlusion_query_pass(std::shared_ptr<Shader> shader,
                                    const glm::mat4& view,
                                    const glm::mat4& projection) const {

    shader->use();
    shader->set_mat4("uView", view);
    shader->set_mat4("uProj", projection);

    // Disable color writes
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    // don't write to depth again, just test against
    glDepthMask(GL_FALSE);

    for (size_t i = 0; i < occludees.size(); ++i) {
        if (!occludees[i]->is_in_frustum())
            continue; // skip if not even visible to the camera

        glBeginQuery(GL_ANY_SAMPLES_PASSED, queries[i]);
        occludees[i]->draw_depth(shader);
        glEndQuery(GL_ANY_SAMPLES_PASSED);
    }

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glUseProgram(0);
}
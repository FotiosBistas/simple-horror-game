#include <vector>
#include "Model.h"
#include "Shader.h"
#include "GlMacros.h"

using namespace GlHelpers;

class Occluder {
private:
    std::weak_ptr<Models::Model> occluder;
    std::vector<std::weak_ptr<Models::Model>> occludees;

    GLuint   depth_map_fbo;
    GLuint   depth_map;
    mutable std::vector<GLuint> queries; 

    size_t screen_width; 
    size_t screen_height;
public:

    Occluder(size_t screen_width, size_t screen_height, std::shared_ptr<Models::Model> occluder);

    void initiliaze_occludees(const std::vector<std::shared_ptr<Models::Model>>& model);

    inline void set_occluder(std::weak_ptr<Models::Model> model){
        this->occluder = model;
    }

    inline void set_model_as_occluder(std::shared_ptr<Models::Model>& model) {
        model->set_is_occluder(true);
    }

    inline std::weak_ptr<Models::Model> get_occluder() const{
        return occluder;
    }

    inline void add_occludee(std::weak_ptr<Models::Model> model){
        auto model_sptr = model.lock();
        auto occluder_model_sptr = occluder.lock();
        if(model_sptr->name() == occluder_model_sptr->name()){
            return;
        }
        occludees.push_back(model);
    }

    void create_queries() const;
    void occlusion_query_pass(std::shared_ptr<Shader> shader, const glm::mat4& view, const glm::mat4& projection) const;
    void depth_pass(std::shared_ptr<Shader> shader, const glm::mat4& view, const glm::mat4& projection) const;
};
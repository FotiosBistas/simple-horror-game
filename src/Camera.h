#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Light.h"

namespace Camera{

    inline float distance_from_using_aabb(const glm::vec3& cen,const glm::vec3& bmin,const glm::vec3& bmax) {
                
            // Raise the center to make the "sphere" collision behave taller
            // Might be used for interaction radius 
            glm::vec3 convenience_offset = glm::vec3(0.0f,-0.6f,0.0f);
            glm::vec3 offset_cen = cen + convenience_offset;
            glm::vec3 closest = glm::clamp(offset_cen, bmin, bmax);
            float dist2 = glm::length2(closest - offset_cen);
            return dist2;
    }

    inline bool intersects_sphere_aabb(const glm::vec3& cen, float r, const glm::vec3& bmin, const glm::vec3& bmax)
        {
            
            return distance_from_using_aabb(cen, bmin, bmax)<= r * r;
        }

    class CameraObj{
    private: 
        glm::vec3 position;
        glm::vec3 world_up;
        glm::vec3 front, up, right;

        float camera_speed;
        float mouse_sensitivity;

        float fov    = glm::radians(45.0f);
        float aspect = 16.0f / 9.0f;
        float near_z  = 0.1f;
        float far_z   = 500.0f;
        // euler angles
        // yaw represents the magnitude of looking left to right
        // pitch represents how much we are looking up or down 
        float yaw, pitch;

        float collision_radius = 0.6f;    // how “fat” the camera is

        Light* flashlight; 
    public: 
        
        CameraObj(int window_width, int window_height,glm::vec3 position): 
        position(position),
        world_up(0.0f, 3.0f, 0.0f),
        camera_speed(10.0f),
        mouse_sensitivity(0.1f),
        yaw(-90.0f),//import to initialize at -90 to start at 0,0,-1
        pitch(0.0f)
        {
            aspect = float(window_width)/float(window_height);
            updateCameraVectors();
            // cursor is centered
            SDL_SetRelativeMouseMode(SDL_TRUE);
        }

          
        

        void update(float delta_time);

        void process_input(const SDL_Event& event);
        inline glm::mat4 get_view_matrix() const {
            return glm::lookAt(position, position + front, up);
        }

        inline glm::mat4 get_projection_matrix() const {
            return glm::perspective(fov, aspect, near_z, far_z);
        }

        inline glm::vec3 get_direction() const {
            return front;
        }

        inline glm::vec3 get_right() const {
            return right;
        }

        inline glm::vec3 get_position() const {
            return position;
        }

        inline void set_position(const glm::vec3& position){
            this->position = position;
        }

        inline void set_direction(const glm::vec3& direction){
            this->front = direction;
        }

        inline void set_pitch(float pitch){
            this->pitch = pitch;
        }

        inline float get_radius() const {
            return collision_radius;
        }
        
        
        float distance_from_camera_using_AABB(const glm::vec3& cen,const glm::vec3& bmin,const glm::vec3& bmax) {
                
            // Raise the center to make the "sphere" collision behave taller
            // Might be used for interaction radius 
            glm::vec3 convenience_offset = glm::vec3(0.0f,-0.6f,0.0f);
            glm::vec3 offset_cen = cen + convenience_offset;
            glm::vec3 closest = glm::clamp(offset_cen, bmin, bmax);
            float dist2 = glm::length2(closest - offset_cen);
            return dist2;
        }
        
        bool intersect_sphere_aabb(const glm::vec3& cen, float r, const glm::vec3& bmin, const glm::vec3& bmax){
            
            return this->distance_from_camera_using_AABB(cen, bmin, bmax)<= r * r;
        }

        void updateCameraVectors();
    };
}

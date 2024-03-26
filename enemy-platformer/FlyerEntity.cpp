#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"
#include "FlyerEntity.h"

FlyerEntity::FlyerEntity(float dash_time, float rest_time, float sight_range) {
	m_ai_state = IDLE;
	m_dash_time = dash_time;
	m_rest_time = rest_time;
	m_timer = 0;
	m_sight_range = sight_range;
	m_target_point = get_position();
	set_motion_type(TOP_DOWN);
}

FlyerEntity::~FlyerEntity() {

}

void FlyerEntity::update(float delta_time, Entity* collidable_entities, int collidable_entity_count, Entity* player) {
	glm::vec3 pos = get_position();
	glm::vec3 playerPos = player->get_position();

	set_movement(glm::vec3(0.0f));
	switch (m_ai_state) {
	case IDLE:
		if (glm::length(playerPos - pos) <= m_sight_range) {
			m_ai_state = DASHING;
			m_timer = m_dash_time;
			m_target_point = playerPos;
		}
		break;
	case DASHING:
		if (m_timer <= 0 or glm::length(m_target_point - pos) <= 0.1) {
			m_ai_state = RESTING;
			m_timer = m_rest_time;
		}
		set_movement(glm::normalize(m_target_point - pos));
		if (get_movement().x > 0) m_animation_indices = m_walking[RIGHT];
		else m_animation_indices = m_walking[LEFT];
		m_timer -= delta_time;
		break;
	case RESTING:
		if (m_timer <= 0) {
			if (glm::length(playerPos - pos) <= m_sight_range) {
				m_ai_state = DASHING;
				m_timer = m_dash_time;
				m_target_point = playerPos;
			} else {
				m_ai_state = IDLE;
			}
		}
		m_timer -= delta_time;
		break;
	default:
		break;
	}

	Entity::update(delta_time, collidable_entities, collidable_entity_count);
}
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
#include "WalkerEntity.h"

WalkerEntity::WalkerEntity(bool dir) {
	m_ai_state = dir? WALK_RIGHT : WALK_LEFT;
	m_edge_check_offset = glm::vec3((dir? 0.2f : -0.2f) * get_width(), -0.6f * get_height(), 0.0f);
}

WalkerEntity::~WalkerEntity() {

}

void WalkerEntity::update(float delta_time, Entity* collidable_entities, int collidable_entity_count) {
	// basic motion
	switch (m_ai_state) {
	case WALK_LEFT:
		move_left();
		m_animation_indices = m_walking[LEFT];
		break;
	case WALK_RIGHT:
		move_right();
		m_animation_indices = m_walking[RIGHT];
		break;
	default:
		break;
	}

	// check for platform edge
	bool edge = true;
	for (int i = 0; i < collidable_entity_count; i++) {
		Entity* other = &collidable_entities[i];
		if (!other->get_active()) continue;

		glm::vec3 edge_check_pos = m_edge_check_offset + get_position();
		glm::vec3 otherPos = other->get_position();
		float x_distance = fabs(edge_check_pos.x - otherPos.x) - (other->get_width() / 2.0f);
		float y_distance = fabs(edge_check_pos.y - otherPos.y) - (other->get_height() / 2.0f);

		if (x_distance < 0.0f && y_distance < 0.0f) edge = false;
	}
	// if at the edge, turn around
	if (edge) {
		if (m_ai_state == WALK_LEFT) m_ai_state = WALK_RIGHT;
		else m_ai_state = WALK_LEFT;
		m_edge_check_offset.x *= -1;
	}

	Entity::update(delta_time, collidable_entities, collidable_entity_count);
}
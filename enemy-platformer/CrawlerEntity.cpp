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
#include "CrawlerEntity.h"

CrawlerEntity::CrawlerEntity(int state, bool dir) {
	m_ai_state = (!state)? GROUND_DOWN : (state==1)? GROUND_RIGHT : (state==2)? GROUND_UP : GROUND_LEFT;
	m_clockwise = dir;
	m_edge_check_offsets[GROUND_DOWN] = glm::vec3((m_clockwise)? 0.01f: -0.01f, -0.5f * get_height(), 0.0f);
	m_edge_check_offsets[GROUND_RIGHT] = glm::vec3(0.5f * get_height(), (m_clockwise) ? 0.01f : -0.01f, 0.0f);
	m_edge_check_offsets[GROUND_UP] = glm::vec3((m_clockwise) ? -0.01f : 0.01f, 0.5f * get_height(), 0.0f);
	m_edge_check_offsets[GROUND_LEFT] = glm::vec3(-0.5f * get_height(), (m_clockwise) ? -0.01f : 0.01f, 0.0f);
	m_current_ec_offset = m_edge_check_offsets[m_ai_state];
}

CrawlerEntity::~CrawlerEntity() {

}

void CrawlerEntity::update(float delta_time, Entity* collidable_entities, int collidable_entity_count) {
	m_current_ec_offset = m_edge_check_offsets[m_ai_state];
	
	// basic motion
	set_movement(glm::vec3(0.0f));
	switch (m_ai_state) {
	case GROUND_DOWN:
		if (m_clockwise) {
			move_right();
			m_animation_indices = m_walking[RIGHT];
		} else {
			move_left();
			m_animation_indices = m_walking[LEFT];
		}
		break;
	case GROUND_RIGHT:
		if (m_clockwise) {
			move_up();
			m_animation_indices = m_walking[RIGHT];
		}
		else {
			move_down();
			m_animation_indices = m_walking[LEFT];
		}
		break;
	case GROUND_UP:
		if (m_clockwise) {
			move_left();
			m_animation_indices = m_walking[RIGHT];
		}
		else {
			move_right();
			m_animation_indices = m_walking[LEFT];
		}
		break;
	case GROUND_LEFT:
		if (m_clockwise) {
			move_down();
			m_animation_indices = m_walking[RIGHT];
		}
		else {
			move_up();
			m_animation_indices = m_walking[LEFT];
		}
		break;
	default:
		break;
	}

	// check for platform edge
	bool edge = true;
	for (int i = 0; i < collidable_entity_count; i++) {
		Entity* other = &collidable_entities[i];
		if (!other->get_active()) continue;

		glm::vec3 edge_check_pos = m_current_ec_offset + get_position();
		glm::vec3 otherPos = other->get_position();
		float x_distance = fabs(edge_check_pos.x - otherPos.x) - (other->get_width() / 2.0f);
		float y_distance = fabs(edge_check_pos.y - otherPos.y) - (other->get_height() / 2.0f);

		if (x_distance < 0.0f && y_distance < 0.0f) edge = false;
	}

	// if at the edge, rotate accordingly
	if (edge) {
		set_movement(glm::vec3(0.0f));
		glm::vec3 pos = get_position();
		switch (m_ai_state) {
		case GROUND_DOWN:
			if (m_clockwise) {
				m_ai_state = GROUND_LEFT;
				set_angle(-90);
			} else {
				m_ai_state = GROUND_RIGHT;
				set_angle(90);
			}
			set_position(glm::vec3(
				pos.x + get_width() / ((m_clockwise)? 2 : -2),
				pos.y - get_height() / 2 - 0.2f,
				0.0f
			));
			break;
		case GROUND_RIGHT:
			if (m_clockwise) {
				m_ai_state = GROUND_DOWN;
				set_angle(0);
			}
			else {
				m_ai_state = GROUND_UP;
				set_angle(180);
			}
			set_position(glm::vec3(
				pos.x + get_height() / 2 + 0.2f,
				pos.y + get_width() / ((m_clockwise) ? 2 : -2),
				0.0f
			));
			break;
		case GROUND_UP:
			if (m_clockwise) {
				m_ai_state = GROUND_RIGHT;
				set_angle(90);
			}
			else {
				m_ai_state = GROUND_LEFT;
				set_angle(-90);
			}
			set_position(glm::vec3(
				pos.x + get_width() / ((m_clockwise) ? -2 : 2),
				pos.y + get_height() / 2 + 0.2f,
				0.0f
			));
			break;
		case GROUND_LEFT:
			if (m_clockwise) {
				m_ai_state = GROUND_UP;
				set_angle(180);
			}
			else {
				m_ai_state = GROUND_DOWN;
				set_angle(0);
			}
			set_position(glm::vec3(
				pos.x - get_height() / 2 - 0.2f,
				pos.y + get_width() / ((m_clockwise) ? -2 : 2),
				0.0f
			));
			break;
		default:
			break;
		}
	}

	Entity::update(delta_time, collidable_entities, collidable_entity_count);
}
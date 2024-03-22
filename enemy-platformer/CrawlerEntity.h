class CrawlerEntity : public Entity {
	enum AIState { GROUND_DOWN = 0, GROUND_RIGHT = 1, GROUND_UP = 2, GROUND_LEFT = 3 };
private:
	AIState m_ai_state;
	glm::vec3 m_edge_check_offsets[4];
	glm::vec3 m_current_ec_offset;
	bool m_clockwise;
public:
	CrawlerEntity(int state, bool dir);
	~CrawlerEntity();

	// ————— METHOD OVERRIDES ————— //
	void update(float delta_time, Entity* collidable_entities, int collidable_entity_count, Entity* player);
	float const get_width() const { return (m_clockwise)? Entity::get_width() : Entity::get_height(); };
	float const get_height() const { return (m_clockwise) ? Entity::get_height() : Entity::get_width(); };

	// ————— GETTERS ————— //
	AIState const get_ai_state() const { return m_ai_state; };

	// ————— SETTERS ————— //
	void const set_ai_state(AIState new_state) { m_ai_state = new_state; };
};

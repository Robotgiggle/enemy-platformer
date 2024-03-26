class CrawlerEntity : public Entity {
	enum AIState { GROUND_DOWN, GROUND_RIGHT, GROUND_UP, GROUND_LEFT };
private:
	AIState m_ai_state;
	glm::vec3 m_edge_check_offsets[4];
	glm::vec3 m_current_ec_offset;
	bool m_clockwise;
public:
	CrawlerEntity(int state, bool dir);
	~CrawlerEntity();

	// ————— METHOD OVERRIDES ————— //
	void update(float delta_time, Entity* collidable_entities, int collidable_entity_count);
	/*float const get_width() const;
	float const get_height() const;*/

	// ————— GETTERS ————— //
	AIState const get_ai_state()  const { return m_ai_state; };
	bool    const get_clockwise() const { return m_clockwise; };

	// ————— SETTERS ————— //
	void const set_ai_state(AIState new_state) { m_ai_state = new_state; };
	void const set_clockwise(bool new_dir) { m_clockwise = new_dir; };
};

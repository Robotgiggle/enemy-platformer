class WalkerEntity : public Entity {
	enum AIState { WALK_LEFT, WALK_RIGHT };
private:
	AIState m_ai_state;
	glm::vec3 m_edge_check_offset;
public:
	WalkerEntity(bool dir);
	~WalkerEntity();

	// ————— METHOD OVERRIDES ————— //
	void update(float delta_time, Entity* collidable_entities, int collidable_entity_count, Entity* player);

	// ————— GETTERS ————— //
	AIState const get_ai_state() const { return m_ai_state; };

	// ————— SETTERS ————— //
	void const set_ai_state(AIState new_state) { m_ai_state = new_state; };
};

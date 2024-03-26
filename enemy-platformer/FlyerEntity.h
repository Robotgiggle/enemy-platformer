class FlyerEntity : public Entity {
private:
	enum AIState { IDLE, DASHING, RESTING };
	AIState m_ai_state;
	float m_dash_time;
	float m_rest_time;
	float m_timer;
	float m_sight_range;
	glm::vec3 m_target_point;
public:
	FlyerEntity(float dash_time, float rest_time, float sight_range);
	~FlyerEntity();

	// ————— METHOD OVERRIDES ————— //
	void update(float delta_time, Entity* collidable_entities, int collidable_entity_count, Entity* player);

	// ————— GETTERS ————— //
	AIState const get_ai_state()    const { return m_ai_state; };
	float   const get_dash_time()   const { return m_dash_time; };
	float   const get_rest_time()   const { return m_rest_time; };
	float   const get_sight_range() const { return m_sight_range; };

	// ————— SETTERS ————— //
	void const set_ai_state(AIState new_state) { m_ai_state = new_state; };
	void const set_dash_time(float new_dash) { m_dash_time = new_dash; };
	void const set_rest_time(float new_rest) { m_rest_time = new_rest; };
	void const get_sight_range(float new_range) { m_sight_range = new_range; };
};
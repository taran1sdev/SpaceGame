#include <glm/glm.hpp>

//  Class for BH related physics

class RigidBody;

class BlackHoleGravity {
    public:
        glm::vec3 position {0.0f};

        float mass = 1000.0f;
        float rs = 2.0f;

        float captureRadius = 0.0f;
        float softening = 2.0f;
        float maxAccel = 200.0f;
        
        float sinkStrength = 600.0f;
        float tangentialDamp = 4.0f;

        void apply(RigidBody& body, float dt) const;
};

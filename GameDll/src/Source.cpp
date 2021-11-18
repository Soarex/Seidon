#include <Seidon.h>
#include <ExtensionEntryPoint.h>

struct PlayerComponent
{
    float speed = 10;
    float jumpHeight = 2;
    glm::vec3 velocity;
};

class CameraSystem : public Seidon::System
{
private:
    float distance = 10;
    float rotationSpeed = 5;
    glm::vec3 orbitAngles;
public:
    void Update(float deltaTime) override
    {
        auto players = scene->GetRegistry().group<PlayerComponent>(entt::get<Seidon::TransformComponent>);
        auto cameras = scene->GetRegistry().view<Seidon::CameraComponent>();

        if (players.empty()) return;

        for (auto e : cameras)
        {
            auto& cameraTransform = scene->GetRegistry().get<Seidon::TransformComponent>(e);
            auto& playerTransform = players.get<Seidon::TransformComponent>(players.front());

            glm::vec3 input = glm::vec3(-inputManager->GetMouseOffset().y, inputManager->GetMouseOffset().x, 0);

            orbitAngles += rotationSpeed * deltaTime * input;

            if (orbitAngles.y < 0) orbitAngles.y += 360;
            if (orbitAngles.y >= 360) orbitAngles.y -= 360;
            if (orbitAngles.x > 0) orbitAngles.x = 0;
            if (orbitAngles.x <= -45) orbitAngles.x = -45;

            glm::vec3 lookDirection = glm::quat(glm::radians(orbitAngles)) * glm::vec3(0, 0, 1);
            cameraTransform.position = playerTransform.position + lookDirection * distance;


            cameraTransform.rotation = glm::radians(orbitAngles);   
        }
    }
};

class PlayerSystem : public Seidon::System
{
private:
    static constexpr float gravity = -9.81f;
public:
    void Update(float deltaTime) override
    {
        auto players = scene->GetRegistry().group<PlayerComponent>(entt::get<Seidon::TransformComponent>);
        auto cameras = scene->GetRegistry().view<Seidon::CameraComponent>();

        if (players.empty()) return;
        for (auto e : cameras)
        {
            auto& cameraTransform = scene->GetRegistry().get<Seidon::TransformComponent>(e);
            auto& playerTransform = players.get<Seidon::TransformComponent>(players.front());
            auto& playerComponent = players.get<PlayerComponent>(players.front());

            glm::vec3 input = { 0, 0, 0 };

            input.x = -inputManager->GetKey(GET_KEYCODE(A)) + inputManager->GetKey(GET_KEYCODE(D));
            input.z = -inputManager->GetKey(GET_KEYCODE(S)) + inputManager->GetKey(GET_KEYCODE(W));

            glm::vec3 forward = cameraTransform.GetForwardDirection();
            forward = glm::normalize(forward);

            glm::vec3 right = cameraTransform.GetRightDirection();
            right = glm::normalize(right);

            glm::vec3 direction = forward * input.z + right * input.x;
            direction.y = 0;

            if (glm::length2(input) > 0)
            {
                direction /= glm::length2(direction);
                playerTransform.rotation.y = std::atan2(direction.x, direction.z);
            }

            playerComponent.velocity.x = direction.x * playerComponent.speed;
            playerComponent.velocity.z = direction.z * playerComponent.speed;
            if (IsGrounded(playerTransform))
            {
                playerComponent.velocity.y = 0;
                if (inputManager->GetKeyPressed(GET_KEYCODE(SPACE)))
                    Jump(playerComponent);
            }
            else
                playerComponent.velocity.y += gravity * deltaTime;

            playerTransform.position += playerComponent.velocity * deltaTime;
        }
    }

    bool IsGrounded(Seidon::TransformComponent& transform)
    {
        return transform.position.y <= 1.5;
    }

    void Jump(PlayerComponent& player)
    {
        player.velocity.y = std::sqrt(-2.0f * gravity * player.jumpHeight);
    }
};

void Init(Seidon::Application& app)
{
    app.RegisterComponent<PlayerComponent>()
        .AddMember("Speed", &PlayerComponent::speed)
        .AddMember("Jump Height", &PlayerComponent::jumpHeight);

    app.RegisterSystem<PlayerSystem>();
    app.RegisterSystem<CameraSystem>();
}

void Destroy(Seidon::Application& app)
{
    app.UnregisterSystem<CameraSystem>();
    app.UnregisterSystem<PlayerSystem>();
}
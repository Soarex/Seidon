#include <Seidon.h>
#include <ExtensionEntryPoint.h>

struct PlayerComponent
{
    float speed = 10;
    float jumpHeight = 2;
    glm::vec3 velocity;
};

struct UIComponent
{
    glm::vec3 baseColor;
    glm::vec3 hoverColor;
    glm::vec3 clickColor;
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

            glm::vec3 input = glm::vec3(-inputManager->GetMouseOffset().y, -inputManager->GetMouseOffset().x, 0);

            if (inputManager->GetGamepadAxis(GET_GAMEPAD_AXISCODE(RIGHT_Y)) != 0 || inputManager->GetGamepadAxis(GET_GAMEPAD_AXISCODE(RIGHT_X)) != 0)
            {
                input.x = -inputManager->GetGamepadAxis(GET_GAMEPAD_AXISCODE(RIGHT_Y)) * 20;
                input.y = -inputManager->GetGamepadAxis(GET_GAMEPAD_AXISCODE(RIGHT_X)) * 20;
            }
            orbitAngles += rotationSpeed * deltaTime * input;

            if (orbitAngles.y < 0) orbitAngles.y += 360;
            if (orbitAngles.y >= 360) orbitAngles.y -= 360;
            if (orbitAngles.x > 45) orbitAngles.x = 45;
            if (orbitAngles.x <= -80) orbitAngles.x = -80;

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
        auto players = scene->GetRegistry().group<PlayerComponent>(entt::get<Seidon::TransformComponent, Seidon::CharacterControllerComponent>);
        auto cameras = scene->GetRegistry().view<Seidon::CameraComponent>();

        if (players.empty()) return;
        for (auto e : cameras)
        {
            auto& cameraTransform = scene->GetRegistry().get<Seidon::TransformComponent>(e);
            auto& playerTransform = players.get<Seidon::TransformComponent>(players.front());
            auto& playerComponent = players.get<PlayerComponent>(players.front());
            auto& characterController = players.get<Seidon::CharacterControllerComponent>(players.front());

            glm::vec3 input = { 0, 0, 0 };

            input.x = -inputManager->GetKey(GET_KEYCODE(A)) + inputManager->GetKey(GET_KEYCODE(D));
            input.z = -inputManager->GetKey(GET_KEYCODE(S)) + inputManager->GetKey(GET_KEYCODE(W));
            
            if (inputManager->GetGamepadAxis(GET_GAMEPAD_AXISCODE(LEFT_X)) != 0 || inputManager->GetGamepadAxis(GET_GAMEPAD_AXISCODE(LEFT_Y)) != 0)
            {
                input.x = inputManager->GetGamepadAxis(GET_GAMEPAD_AXISCODE(LEFT_X));
                input.z = -inputManager->GetGamepadAxis(GET_GAMEPAD_AXISCODE(LEFT_Y));
            }

            glm::vec3 forward = cameraTransform.GetForwardDirection();
            forward = glm::normalize(forward);

            glm::vec3 right = cameraTransform.GetRightDirection();
            right = glm::normalize(right);

            glm::vec3 direction = forward * input.z + right * input.x;
            direction.y = 0;

            if (glm::length(direction) > 0)
            {
                direction /= glm::length(direction);
                playerTransform.rotation.y = std::atan2(direction.x, direction.z);
            }

            playerComponent.velocity.x = direction.x * playerComponent.speed;
            playerComponent.velocity.z = direction.z * playerComponent.speed;

            if (IsGrounded(characterController))
            {
                playerComponent.velocity.y = 0;
                if (inputManager->GetKeyPressed(GET_KEYCODE(SPACE)) || inputManager->GetGamepadButton(GET_GAMEPAD_BUTTONCODE(A)))
                    Jump(playerComponent);
            }
            else
                playerComponent.velocity.y += gravity * deltaTime;

            playerTransform.position += playerComponent.velocity * deltaTime;
        }
    }

    bool IsGrounded(Seidon::CharacterControllerComponent& characterController)
    {
        return characterController.isGrounded;//transform.position.y <= 0.51;
    }

    void Jump(PlayerComponent& player)
    {
        player.velocity.y = std::sqrt(-2.0f * gravity * player.jumpHeight);
    }
};

class UISystem : public Seidon::System
{
private:

public:
    void Update(float deltaTime) override
    {
        auto elements = scene->GetRegistry().group<UIComponent>(entt::get<Seidon::MouseSelectionComponent, Seidon::RenderComponent>);

        for (auto e : elements)
        {
            auto& uiComponent = elements.get<UIComponent>(e);
            auto& selectionComponent = elements.get<Seidon::MouseSelectionComponent>(e);
            auto& renderComponent = elements.get<Seidon::RenderComponent>(e);

            switch (selectionComponent.status)
            {
            case Seidon::SelectionStatus::NONE:
                //renderComponent.materials[0]->tint = uiComponent.baseColor;
                break;

            case Seidon::SelectionStatus::HOVERED:
                //renderComponent.materials[0]->tint = uiComponent.hoverColor;
                break;

            case Seidon::SelectionStatus::CLICKED: case Seidon::SelectionStatus::HELD:
                //renderComponent.materials[0]->tint = uiComponent.clickColor;
                break;
            }
        }
    }
};

void Init(Seidon::Application& app)
{
    app.RegisterComponent<PlayerComponent>()
        .AddMember("Speed", &PlayerComponent::speed)
        .AddMember("Jump Height", &PlayerComponent::jumpHeight);

    app.RegisterComponent<UIComponent>()
        .AddMember("Base Color", &UIComponent::baseColor, Seidon::Types::VECTOR3_COLOR)
        .AddMember("Hover Color", &UIComponent::hoverColor, Seidon::Types::VECTOR3_COLOR)
        .AddMember("Click Color", &UIComponent::clickColor, Seidon::Types::VECTOR3_COLOR);

    app.RegisterSystem<PlayerSystem>();
    app.RegisterSystem<CameraSystem>();
    app.RegisterSystem<UISystem>();
}

void Destroy(Seidon::Application& app)
{
    app.UnregisterSystem<CameraSystem>();
    app.UnregisterSystem<PlayerSystem>();
    app.UnregisterSystem<UISystem>();
}
#include <Seidon.h>
#include <ExtensionEntryPoint.h>

struct TestComponent
{
	int a;
};

class TestSystem : public Seidon::System
{
public:
    void Update(float deltaTime) override
    {
        auto test = scene->GetRegistry().group<TestComponent>(entt::get<Seidon::TransformComponent>);

        for (auto e : test)
        {
            auto& transformComponent = test.get<Seidon::TransformComponent>(e);
            transformComponent.position += glm::vec3(1, 0, 0) * deltaTime;
        }
    }
};

void Init(Seidon::Application& app)
{
	app.RegisterComponent<TestComponent>();
    app.RegisterSystem<TestSystem>();
}

void Destroy(Seidon::Application& app)
{
    app.UnregisterSystem<TestSystem>();
}
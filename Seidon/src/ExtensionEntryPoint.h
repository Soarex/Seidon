#pragma once
#include "Core/Application.h"

#define SEIDON_API extern "C" __declspec(dllexport)

EcsContext* mainContext;

template<typename Type>
struct entt::type_seq<Type>
{
    [[nodiscard]] static id_type value() ENTT_NOEXCEPT {
        static const entt::id_type value = mainContext->value(entt::type_hash<Type>::value());
        return value;
    }
};

void Init(Seidon::Application& app);
void Destroy(Seidon::Application& app);

SEIDON_API void ExtInit(Seidon::Application& app)
{
    app.instance = &app;
    mainContext = app.GetEcsContext();
    mainContext->SetMetaContext();
    Init(app);
}

SEIDON_API void ExtDestroy(Seidon::Application& app)
{
    Destroy(app);
}
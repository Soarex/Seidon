#pragma once
#include "Core/Application.h"

#define SEIDON_API extern "C" __declspec(dllexport)

void Init(Seidon::Application& app);
void Destroy(Seidon::Application& app);

SEIDON_API void ExtInit(Seidon::Application& app)
{
    app.instance = &app;
    Init(app);
}

SEIDON_API void ExtDestroy(Seidon::Application& app)
{
    Destroy(app);
}
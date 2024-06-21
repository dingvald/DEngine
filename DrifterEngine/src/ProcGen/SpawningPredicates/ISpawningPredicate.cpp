#include "pch.h"
#include "ISpawningPredicate.h"

void drft::gen::ISpawningPredicate::setContext(GenerationContext& context)
{
	_context = &context;
}

void drft::gen::ISpawningPredicate::setParameters(GenerationParameters& params)
{
	_params = &params;
}

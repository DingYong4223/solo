#include "SoloEngineCreationArgs.h"
#include "platform/stub/SoloStubEffect.h"
#include "platform/opengl/SoloGLSLEffect.h"

using namespace solo;


shared<Effect> EffectFactory::create(EngineMode engineMode, const std::string& vsSrc, const std::string& fsSrc)
{
	if (engineMode == EngineMode::OpenGL)
		return SL_NEW2(GLSLEffect, vsSrc, fsSrc);
	return SL_NEW2(StubEffect);
}


EffectVariable* Effect::findVariable(const std::string& name) const
{
	auto where = variables.find(name);
	if (where != variables.end())
		return where->second.get();
	return nullptr;
}

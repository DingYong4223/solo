#pragma once

#include "TestBase.h"

const char * vs =
	"#version 330 core\n"

	"uniform float testFloat;\n"
	"uniform sampler2D testSampler;\n"
	"uniform float testArray[3];\n"
	"uniform vec3 testVector;\n"

	"void main()\n"
	"{\n"
	"	float nonsense = testFloat * texture(testSampler, testVector.xy).x;\n"
	"	gl_Position = vec4(nonsense, testArray[0], testArray[1], testArray[2]);\n"
	"}";

const char *fs =
	"#version 330 core\n"

	"layout (location = 0) out vec4 color;\n"

	"void main()\n"
	"{\n"
	"	color = vec4(1, 1, 1, 1);\n"
	"}";


class MaterialsTest : public TestBase
{
public:
	MaterialsTest(Engine *engine) : TestBase(engine)
	{
	}

	virtual void run() override
	{
		testEffectCompilation();
		testEffectVariablesDetection();
	}

	void testEffectVariablesDetection()
	{
		auto effect = engine->getResourceManager()->getEffect(vs, fs);
		auto var1 = effect->findVariable("testFloat");
		auto var2 = effect->findVariable("testSampler");
		auto var3 = effect->findVariable("testArray");
		assert(var1);
		assert(var2);
		assert(var3);
		assert(var1->getName() == "testFloat");
		assert(var2->getName() == "testSampler");
		assert(var3->getName() == "testArray");
	}

	void testEffectCompilation()
	{
		auto effect = engine->getResourceManager()->getEffect(vs, fs);
		assert(effect->isValid());
		assert(effect->getLog().empty());
	}
};

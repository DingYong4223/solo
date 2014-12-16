#include "SoloBaseInternal.h"
#include "SoloEngine.h"

using namespace solo;


extern "C" IEngine *getEngine()
{
	return Engine::getEngine();
}


extern "C" size_t computeHash(const std::string &s)
{
	return std::hash<std::string>()(s);
}
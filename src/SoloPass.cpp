#include "SoloPass.h"

using namespace solo;


std::shared_ptr<Pass> Pass::create()
{
	return NEW2(Pass);
}

#pragma once
#include <chrono>
#include "tagged_type.h"


namespace thermo
{

MakeTagged(Pin, int);
MakeTagged(MaxCurrent, float);
MakeTagged(UpperTemp, float);
MakeTagged(LowerTemp, float);
MakeTagged(FilterCoefficient, float);
MakeTagged(MaxOnTime, std::chrono::seconds);

} //namespace thermo

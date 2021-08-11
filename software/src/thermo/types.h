#pragma once
#include "tagged_type.h"


namespace thermo
{

MakeTagged(Pin, int);
MakeTagged(MaxCurrent, float);
MakeTagged(UpperTemp, float);
MakeTagged(LowerTemp, float);
MakeTagged(FilterCoefficient, float);

} //namespace thermo

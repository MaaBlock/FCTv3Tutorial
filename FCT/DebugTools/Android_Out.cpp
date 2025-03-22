#include "Android_Out.h"

Android_Out androidOut("AO");
AndroidErr androidErr("AE");
std::ostream aout(&androidOut);
std::ostream aerr(&androidErr);
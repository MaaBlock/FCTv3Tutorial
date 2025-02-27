#include "AndroidOut.h"

AndroidOut androidOut("AO");
AndroidErr androidErr("AE");
std::ostream aout(&androidOut);
std::ostream aerr(&androidErr);
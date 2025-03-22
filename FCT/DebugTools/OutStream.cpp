#include "OutStream.h"

namespace FCT {
#ifdef FCT_ANDROID
    std::ostream& fout = aout;
    std::ostream& ferr = aerr;
#else
    std::ostream& fout = std::cout;
    std::ostream& ferr = std::cout;
#endif
}
#include "MemoryCheak.h"
#ifdef FCT_MEMORY_CHEAK
namespace FCT {
	std::vector<_fct_object_t*> fct_object_list = std::vector<_fct_object_t*>();
	std::string fct_object_info = std::string();
	int fct_object_count_delete_without_object = 0;
	std::mutex fct_object_mutex;
}
#endif // FCT_MEMORY_CHEAK

#include "ittia/os/os_config.h"

C_HEADER_BEGIN

#define SEC 1
#define MILLI_SEC 1000
#define MICRO_SEC 1000000
#define NANO_SEC  1000000000

#define SCALE_TIME(tm,src_scale,dst_scale) (\
                        (src_scale) == (dst_scale) ?     (tm) : \
                        (src_scale) <  (dst_scale) ?     (tm) * ((dst_scale)/(src_scale)) : \
                      /*(src_scale) >  (dst_scale) ? */  (tm) / ((src_scale)/(dst_scale)))



#ifdef HAVE_RAMFS_FILE
    int ramfs_file_remove(const db_fname_t * file_name);
#else
#   define ramfs_file_remove(x)    (DB_ENOENT)
#endif

C_HEADER_END

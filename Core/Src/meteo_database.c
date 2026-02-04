/**************************************************************************/
/*                                                                        */
/*      METEO Database Implementation - ITTIA DB                          */
/*                                                                        */
/**************************************************************************/

#include "meteo_database.h"
#include <string.h>

// Index key compare function for table: meteo_readings4
static dbstatus_t compare_meteo_readings_by_PK(const void *v1, const void *v2, size_t key_field_count, uint32_t flags)
{
    const uint8_t *data1 = (const uint8_t *)v1;
    const uint8_t *data2 = (const uint8_t *)v2;

    int32_t key1_id;
    int32_t key2_id;

    (void)memcpy(&key1_id, &data1[0], sizeof key1_id);
    (void)memcpy(&key2_id, &data2[0], sizeof key2_id);

    if (key1_id < key2_id) {
        return BTREE_KEY_LT;
    }
    else if (key1_id > key2_id) {
        return BTREE_KEY_GT;
    }
    else {
        return BTREE_KEY_EQ;
    }
}

dbstatus_t open_meteo_database(const char * database_name, const db_database_config_t * config)
{
    static const db_index_compare_keys_t compare_functions[1] = {
        &compare_meteo_readings_by_PK,
    };

    return db_open_index_storage(database_name, config, compare_functions, 1);
}

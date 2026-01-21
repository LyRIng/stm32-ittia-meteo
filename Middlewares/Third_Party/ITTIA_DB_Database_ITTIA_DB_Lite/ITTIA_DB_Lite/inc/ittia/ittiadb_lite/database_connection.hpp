
#ifndef ITTIADB_LITE_DATABASE_CONNECTION_HPP
#define ITTIADB_LITE_DATABASE_CONNECTION_HPP

#include "ittia/db/db_index_storage.h"

namespace ittia {
namespace ittiadb_lite {

/**
 * @brief A connection to an ITTIA DB Lite database storage.
 * 
 * Do not share a database connection object between threads.
 */
class DatabaseConnection {
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

public:
    /**
     * @brief Construct a new database connection object.
     */
    DatabaseConnection() {}
    /**
     * @brief Destroy the database connection object.
     * 
     * The connection is disconnected automatically.
     */
    ~DatabaseConnection() { (void)disconnect(); }

    /**
     * @brief Connect to an open database.
     * 
     * @param database_name
     *   Name of an open database.
     * @return dbstatus_t error code.
     */
    dbstatus_t connect(const char * database_name);

    /**
     * @brief Disconnect from the database.
     * 
     * Do not use the database connection after it has been disconnected.
     * 
     * @return dbstatus_t error code.
     */
    dbstatus_t disconnect();

public:
    /**
     * @brief Begin a read-only database transaction.
     * 
     * @return dbstatus_t error code.
     */
    dbstatus_t begin_tx_read_only();

    /**
     * @brief Begin a read-write database transaction.
     * 
     * @return dbstatus_t error code.
     */
    dbstatus_t begin_tx_read_write();

    /**
     * @brief Commit the current database transaction.
     * 
     * @return dbstatus_t error code.
     */
    dbstatus_t commit_tx();

    /**
     * @brief Roll back the current database transaction.
     * 
     * @return dbstatus_t error code.
     */
    dbstatus_t rollback_tx();

protected:
    db_t database = nullptr;
};

inline dbstatus_t DatabaseConnection::connect(const char * database_name)
{
    return ::db_connect(&database, database_name, nullptr, nullptr, this);
}

inline dbstatus_t DatabaseConnection::disconnect()
{
    const dbstatus_t status = ::db_disconnect(database);
    database = nullptr;
    return status;
}

inline dbstatus_t DatabaseConnection::begin_tx_read_only()
{
    return DB_OK == db_begin_tx(database, DB_TX_READ_ONLY) ? DB_NOERROR : get_db_error();
}

inline dbstatus_t DatabaseConnection::begin_tx_read_write()
{
    return DB_OK == db_begin_tx(database, DB_TX_READ_WRITE) ? DB_NOERROR : get_db_error();
}

inline dbstatus_t DatabaseConnection::commit_tx()
{
    return DB_OK == db_commit_tx(database, 0) ? DB_NOERROR : get_db_error();
}

inline dbstatus_t DatabaseConnection::rollback_tx()
{
    return DB_OK == db_abort_tx(database, 0) ? DB_NOERROR : get_db_error();
}

}
}

#endif // ITTIADB_LITE_DATABASE_CONNECTION_HPP

/**************************************************************************/
/*                                                                        */
/*      Copyright (c) 2005-2025 by ITTIA L.L.C. All rights reserved.      */
/*                                                                        */
/*  This software is copyrighted by and is the sole property of ITTIA     */
/*  L.L.C.  All rights, title, ownership, or other interests in the       */
/*  software remain the property of ITTIA L.L.C.  This software may only  */
/*  be used in accordance with the corresponding license agreement.  Any  */
/*  unauthorized use, duplication, transmission, distribution, or         */
/*  disclosure of this software is expressly forbidden.                   */
/*                                                                        */
/*  This Copyright notice may not be removed or modified without prior    */
/*  written consent of ITTIA L.L.C.                                       */
/*                                                                        */
/*  ITTIA L.L.C. reserves the right to modify this software without       */
/*  notice.                                                               */
/*                                                                        */
/*  info@ittia.com                                                        */
/*  https://www.ittia.com                                                 */
/*                                                                        */
/*                                                                        */
/**************************************************************************/


/* Do not edit! This file generated automatically from tools/errors. */

#include "dbs_error_info.h"
#include <string.h>

/* NOTE: This array must be sorted by error_info_t.code. */
static const dbs_error_info_t errors[] =
{
    {  +113, "DB_INCOMPLETE",               "Operation did not finish completely" },
    {     0, "DB_NOERROR",                  "No error occurred" },
    {    -1, "DB_FAILURE",                  "Operation failed" },
    {    -2, "DB_EBADF",                    "Invalid file handle" },
    {    -3, "DB_EEXIST",                   "File already exists" },
    {    -4, "DB_ENOENT",                   "File doesn't exist" },
    {    -5, "DB_ENOSPACE",                 "No space available" },
    {    -6, "DB_EIO",                      "I/O error" },
    {    -7, "DB_EACCESS",                  "File cannot be opened due to invalid access mode or insufficient file system privileges" },
    {    -8, "DB_EEXTEND",                  "File operation is out of file size" },
    {    -9, "DB_EDEADLOCK",                "Unable to lock - deadlock condition detected" },
    {   -10, "DB_ELOCKED",                  "Unable to acquire lock" },
    {   -11, "DB_EINVAL",                   "Invalid argument" },
    {   -12, "DB_ERANGE",                   "Argument is out of range" },
    {   -13, "DB_ESTATE",                   "Object state is not compatible with the method called or arguments" },
    {   -14, "DB_ENOTIMPL",                 "The requested functionality is not implemented in the current library package" },
    {   -15, "DB_ENOMEM",                   "No memory available for allocation" },
    {   -16, "DB_EOSERROR",                 "General operating system error" },
    {   -17, "DB_ENOTAPPLICABLE",           "Not applicable" },
    {   -18, "DB_ENOTHREAD",                "No such thread" },
    {   -19, "DB_EINTERNAL",                "Internal error detected" },
    {   -20, "DB_EINVPATH",                 "Invalid path encoding" },
    {   -21, "DB_EINVFS",                   "Invalid file system requested" },
    {   -22, "DB_EDBGFAILURE",              "Simulated failure for debugging" },
    {   -23, "DB_ENLSSPACE",                "Not enough space for NLS conversion" },
    {   -24, "DB_EINVCHAR",                 "Invalid character for NLS conversion" },
    {   -25, "DB_EINVENC",                  "Invalid encoding for NLS conversion" },
    {   -26, "DB_ENOSYS",                   "A required function is not supported on this platform" },
    {   -27, "DB_EINVLOCALE",               "Invalid or unknown locale name" },
    {   -50, "DB_ESOCKETSTARTUP",           "Socket startup error" },
    {   -51, "DB_ESOCKETOPEN",              "Socket open error" },
    {   -52, "DB_ESOCKET",                  "General socket error" },
    {   -53, "DB_ESOCKETHOSTNAME",          "Host name error" },
    {   -54, "DB_ESOCKETCONNECTION",        "Connection error" },
    {   -55, "DB_ESOCKETRECV",              "Socket data receive error" },
    {   -56, "DB_ESOCKETSEND",              "Socket data send error" },
    {   -57, "DB_ESOCKETNOTCONN",           "Socket is not connected" },
    {   -60, "DB_ESSLCIPHERKEY",            "Private key is not valid" },
    {   -61, "DB_ESSLDHPARAMS",             "Diffie-Hellmap parameteres are not valid" },
    {   -62, "DB_ESSLCERTIFICATE",          "Certificate chain is not valid" },
    {   -63, "DB_ESSLCALOCATION",           "CA file is not valid" },
    {  -100, "DB_EDUPLICATE",               "Duplicate value not allowed" },
    {  -101, "DB_ENOTFOUND",                "Item not found" },
    {  -102, "DB_ENOMOREDATA",              "No more data" },
    {  -103, "DB_EKEYOUTOFRANGE",           "Key out of range" },
    {  -104, "DB_EEMPTYRANGE",              "An attempt was made to set an empty range" },
    {  -105, "DB_EBTREESLOTTOOLARGE",       "An attempt was made to insert into a B+tree a key and data that is too large" },
    {  -106, "DB_ENEEDREVALIDATE",          "B+tree iterator needs revalidation" },
    {  -107, "DB_ENORECORD",                "No record found" },
    {  -109, "DB_ENAME",                    "Invalid name" },
    {  -110, "DB_EROWRANGE",                "Too many records in a table" },
    {  -111, "DB_ESHUTDOWN",                "Shutdown is in progress" },
    {  -112, "DB_ECONNLOCK",                "Connection is used by another thread" },
    {  -114, "DB_ECURSORMODE",              "Invalid or unsupported cursor mode" },
    {  -115, "DB_ECIPHER",                  "Invalid cipher type" },
    {  -116, "DB_ECIPHERDATA",              "Invalid cipher data" },
    {  -120, "DB_ETABLE",                   "General table error" },
    {  -121, "DB_ETABLETYPE",               "Unknown table type" },
    {  -122, "DB_ENOTABLE",                 "No such table" },
    {  -123, "DB_ENOMEMSTORAGE",            "Memory storage is not open, memory_storage_size is 0" },
    {  -125, "DB_ENOCONSTRAINT",            "No such constraint defined" },
    {  -126, "DB_ETABLEEXISTS",             "Table already exists" },
    {  -130, "DB_EINDEX",                   "General index error" },
    {  -131, "DB_EPRIMARYDUP",              "Multiple primary keys defined" },
    {  -132, "DB_EFKREFERENCE",             "Referenced row not found" },
    {  -133, "DB_EFKRESTRICTED",            "Operation is restricted due to references" },
    {  -134, "DB_EFKMATCHFULL",             "Operation is restricted due to incomplete full row match" },
    {  -135, "DB_EFKEYDEF",                 "Invalid foreign key definition" },
    {  -136, "DB_EFKLIMIT",                 "Too many cascade operations" },
    {  -137, "DB_ECLUSTERKEY",              "Invalid cluster key" },
    {  -140, "DB_EFIELD",                   "General field error" },
    {  -141, "DB_EDUPLICATEFIELDS",         "Cannot create duplicate fields" },
    {  -142, "DB_EFIELDSIZE",               "Invalid field size" },
    {  -143, "DB_EBINDSIZE",                "Invalid bind size" },
    {  -144, "DB_EPARAM",                   "Invalid parameter" },
    {  -145, "DB_EINDVALUE",                "Length indicator value exceeds buffer size or is an invalid negative number" },
    {  -146, "DB_ENULLFIELD",               "Insert or update would violate a NOT NULL constraint, or primary key must be NOT NULL" },
    {  -147, "DB_EFIELDTYPE",               "Invalid field type" },
    {  -148, "DB_EBLOB",                    "Invalid blob operation" },
    {  -150, "DB_ESEQ",                     "General sequence error" },
    {  -151, "DB_ESEQ_OVERFLOW",            "Sequence overflow: no unique values remain" },
    {  -152, "DB_ESEQ_NOCURVAL",            "Sequence does not yet have a current value, get the next value first" },
    {  -160, "DB_EINVALROWTYPE",            "Invalid row type" },
    {  -161, "DB_EINVALKEYTYPE",            "Invalid key type" },
    {  -162, "DB_EKEYTOOSHORT",             "Key is too short" },
    {  -163, "DB_ERECORDSIZE",              "Record size is too large for chosen page size" },
    {  -167, "DB_EBOOKMARK",                "Bookmark is not valid" },
    {  -168, "DB_EINVALIDINTERVAL",         "Invalid INTERVAL type literal" },
    {  -169, "DB_ECURSOR",                  "Invalid cursor for the given operation" },
    {  -170, "DB_EDATA",                    "Invalid field data value" },
    {  -171, "DB_ECONVERT",                 "Incompatible types: conversion is not possible" },
    {  -172, "DB_ENOBUFFER",                "No available buffers" },
    {  -173, "DB_EINVTYPE",                 "Invalid column/var type" },
    {  -174, "DB_ESTORAGE",                 "Storage not recognized as a valid database" },
    {  -175, "DB_ECRC",                     "CRC verification failed: database integrity has been compromised" },
    {  -176, "DB_ERECOVERY",                "Storage needs crash recovery" },
    {  -177, "DB_EUNDOSPACE",               "Overflow in undo log space, unable to write entries required for rollback" },
    {  -178, "DB_EJOURNALTYPE",             "Invalid journal type" },
    {  -179, "DB_EJOURNAL",                 "Invalid journal or missing log file" },
    {  -180, "DB_EJOURNALCRC",              "Journal CRC validation failed" },
    {  -181, "DB_EPREPARED",                "Prepared transactions support is required to complete operation" },
    {  -182, "DB_ENOJOURNALREC",            "Requested journal record not found" },
    {  -183, "DB_EINVPAGE",                 "Invalid page number" },
    {  -184, "DB_EREADONLY",                "Storage is in the read only mode" },
    {  -185, "DB_ENOPAGESPACE",             "Storage run out of disk space" },
    {  -186, "DB_EPASSWORD",                "Access password is not valid or access is denied" },
    {  -187, "DB_EJOURNALWRITE",            "Unable to write journal file" },
    {  -188, "DB_EINCOMPATIBLE",            "Incompatible storage format" },
    {  -190, "DB_ENOTX",                    "No transaction is active" },
    {  -191, "DB_ETXSTATE",                 "The operation cannot be performed in the current transaction state" },
    {  -192, "DB_ETXACTIVE",                "Transaction is already running" },
    {  -193, "DB_ESAVEPOINTEXIST",          "Savepoint already exists" },
    {  -194, "DB_ENOSAVEPOINT",             "Specified savepoint does not exist" },
    {  -195, "DB_ETXKIND",                  "Operation cannot be performed for such transaction type (for example, local commit cannot be done on prepared transaction)" },
    {  -196, "DB_EDTMXID",                  "Invalid DTM xid provided" },
    {  -197, "DB_EBADTX",                   "Transaction cannot continue and must abort" },
    {  -200, "DB_ENOLOCK",                  "There are no available lock cells" },
    {  -201, "DB_EMAXLOCKERS",              "There are too many lockers" },
    {  -202, "DB_EINVLOCK",                 "Unlock request for a lock that has not been obtained" },
    {  -203, "DB_EWAITINTR",                "Lock waiting interrupted" },
    {  -204, "DB_ENOLOCKOBJ",               "There are no available lock object cells" },
    {  -210, "DB_EUSER",                    "User generated error" },
    {  -220, "DB_EAPIVER",                  "API not initialized or feature not available in this version of the API" },
    {  -250, "DB_EEVALUATION",              "Operation not permitted in evaluation version" },
    {  -300, "DB_EIPCPROTO",                "No compatible protocol negotiated  " },
    {  -301, "DB_EIPCINVAL",                "Invalid parameter in IPC message" },
    {  -302, "DB_EIPCINVALMESSAGETAG",      "Invalid parameter in IPC message" },
    {  -303, "DB_EIPCINVALPROCTAG",         "Invalid procedure tag in IPC CALL message" },
    {  -304, "DB_EIPCSTORAGEALREADYOPENED", "Storage already opened" },
    {  -305, "DB_EIPCSTORAGENOTOPENED",     "Storage not opened " },
    {  -306, "DB_EIPCINVALHANDLE",          "Invalid object's handle" },
    {  -307, "DB_EIPCNULLPARAM",            "Complex parameter is NULL" },
    {  -308, "DB_EIPCTOOFEWPARAMS",         "Too few parameters in message " },
    {  -309, "DB_EIPCINVALMESSAGESIZE",     "Invalid message size" },
    {  -310, "DB_EIPCINVALMESSAGEFORMAT",   "Invalid message format" },
    {  -311, "DB_EIPCINVALURL",             "Invalid URL format" },
    {  -312, "DB_EIPCINVALURLSCHEMA",       "Invalid URL schema " },
    {  -313, "DB_EIPCENVSTARTUP",           "IPC environment startup error" },
    {  -314, "DB_EIPCACCESS",               "Disabled operations" },
    {  -315, "DB_EIPCMAXCONNECTIONS",       "Maximum connections is reached  " },
    {  -316, "DB_EIPCALREADYSTARTED",       "Server is already running" },
    {  -317, "DB_EIPCNOLISTENER",           "No listeners" },
    {  -318, "DB_EIPCDISCONNECT",           "Connection has been dropped" },
    {  -408, "DB_EINVALIDNUMBER",           "Invalid number" },

};

static const dbs_error_info_t error_unknown = { 0, "DB_?", "Unknown error" };

#define ERRORS_SIZE (sizeof(errors)/sizeof(errors[0]))


dbs_error_info_t dbs_get_error_info(int error_code)
{
    int start = 0;
    int end = ERRORS_SIZE;

    /* Binary search: errors[] is sorted by code in descending order */
    while (start < end) {
      int i = start + ((end - start) / 2);
      if (errors[i].code > error_code) {
        start = i + 1;
      }
      else {
        end = i;
      }
    }
    if (start < ERRORS_SIZE && errors[start].code == error_code) {
      return errors[start];
    }
    else {
      /* Error code not found. */
      dbs_error_info_t error_unknown_result;
      error_unknown_result = error_unknown;
      error_unknown_result.code = error_code;
      return error_unknown_result;
    }
}

const dbs_error_info_t *
dbs_find_error(const char * name)
{
    int start = 0;
    int end = ERRORS_SIZE;

    while (start < end) {
        if (strcmp(errors[start].name, name) == 0) {
            return &errors[start];
        }

        ++start;
    }

    return NULL;
}

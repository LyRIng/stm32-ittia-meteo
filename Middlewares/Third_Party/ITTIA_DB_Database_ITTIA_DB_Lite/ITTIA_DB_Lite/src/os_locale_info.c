#include "ittia/os/os_locale_info.h"
#include "ittia/os/os_error.h"
#include "ittia/os/std/string.h"

#ifdef OS_WIN32
#   include "ittia/os/std/windows.h"
#   define strcasecmp   stricmp
#endif

#ifdef HAVE_NL_LANGINFO
#   include <langinfo.h>
#endif

#include "ittia/os/os_nls.h"

DBDLL_API int
db_locale_info(const char * locale_name, const char *** ref)
{
    static int init_locale = 1;
    static int encoding;
    static const char * sys_local_names  [LOCAL_MAX + 1];
    static const char * std_local_names  [LOCAL_MAX + 1];

    if (init_locale) {

        /* init std locale */

#define SET_ITEM(target, item) std_local_names[target] = item

        SET_ITEM( LOCAL_SHORT_DAY_1, "Sun" );
        SET_ITEM( LOCAL_SHORT_DAY_2, "Mon");
        SET_ITEM( LOCAL_SHORT_DAY_3, "Tue" );
        SET_ITEM( LOCAL_SHORT_DAY_4, "Wed");
        SET_ITEM( LOCAL_SHORT_DAY_5, "Thu" );
        SET_ITEM( LOCAL_SHORT_DAY_6, "Fri" );
        SET_ITEM( LOCAL_SHORT_DAY_7, "Sat" );

        SET_ITEM( LOCAL_LONG_DAY_1, "Sunday" );
        SET_ITEM( LOCAL_LONG_DAY_2, "Monday" );
        SET_ITEM( LOCAL_LONG_DAY_3, "Tuesday" );
        SET_ITEM( LOCAL_LONG_DAY_4, "Wednesday" );
        SET_ITEM( LOCAL_LONG_DAY_5, "Thursday" );
        SET_ITEM( LOCAL_LONG_DAY_6, "Friday" );
        SET_ITEM( LOCAL_LONG_DAY_7, "Saturday" );

        SET_ITEM( LOCAL_SHORT_MONTH_1, "Jan");
        SET_ITEM( LOCAL_SHORT_MONTH_2, "Feb");
        SET_ITEM( LOCAL_SHORT_MONTH_3, "Mar");
        SET_ITEM( LOCAL_SHORT_MONTH_4, "Apr");
        SET_ITEM( LOCAL_SHORT_MONTH_5, "May");
        SET_ITEM( LOCAL_SHORT_MONTH_6, "Jun");
        SET_ITEM( LOCAL_SHORT_MONTH_7, "Jul");
        SET_ITEM( LOCAL_SHORT_MONTH_8, "Aug");
        SET_ITEM( LOCAL_SHORT_MONTH_9, "Sep");
        SET_ITEM( LOCAL_SHORT_MONTH_10, "Oct");
        SET_ITEM( LOCAL_SHORT_MONTH_11, "Nov");
        SET_ITEM( LOCAL_SHORT_MONTH_12, "Dec");

        SET_ITEM( LOCAL_LONG_MONTH_1, "January");
        SET_ITEM( LOCAL_LONG_MONTH_2, "February");
        SET_ITEM( LOCAL_LONG_MONTH_3, "March");
        SET_ITEM( LOCAL_LONG_MONTH_4, "April");
        SET_ITEM( LOCAL_LONG_MONTH_5, "May");
        SET_ITEM( LOCAL_LONG_MONTH_6, "June");
        SET_ITEM( LOCAL_LONG_MONTH_7, "July");
        SET_ITEM( LOCAL_LONG_MONTH_8, "August");
        SET_ITEM( LOCAL_LONG_MONTH_9, "September");
        SET_ITEM( LOCAL_LONG_MONTH_10, "October");
        SET_ITEM( LOCAL_LONG_MONTH_11, "November");
        SET_ITEM( LOCAL_LONG_MONTH_12, "December");

        SET_ITEM( LOCAL_DATE_SEPARATOR, "-" );
        SET_ITEM( LOCAL_TIME_SEPARATOR, ":" );

        SET_ITEM( LOCAL_AM, "AM");
        SET_ITEM( LOCAL_PM, "PM");

        SET_ITEM( LOCAL_FRACTION_SEPARATOR, ".");
#undef SET_ITEM

        /* init user locale */
#define SET_ITEM(target, item) sys_local_names[target] = item

#ifdef OS_WIN32
        static char buf[512];

        char * p = buf;
        size_t size = sizeof(buf);
        int n;
        int pos = 0;

        encoding = DB_NLS_ANSI;

#define GET_ITEM(target, item) \
        do { \
            SET_ITEM(target, p + pos); \
            n = GetLocaleInfoA( LOCALE_USER_DEFAULT, item, p + pos, size - pos); \
            pos += n; \
            p[pos] = 0; \
            ++pos; \
        } while (0)

        GET_ITEM( LOCAL_SHORT_DAY_1, LOCALE_SABBREVDAYNAME7 );
        GET_ITEM( LOCAL_SHORT_DAY_2, LOCALE_SABBREVDAYNAME1 );
        GET_ITEM( LOCAL_SHORT_DAY_3, LOCALE_SABBREVDAYNAME2 );
        GET_ITEM( LOCAL_SHORT_DAY_4, LOCALE_SABBREVDAYNAME3 );
        GET_ITEM( LOCAL_SHORT_DAY_5, LOCALE_SABBREVDAYNAME4 );
        GET_ITEM( LOCAL_SHORT_DAY_6, LOCALE_SABBREVDAYNAME5 );
        GET_ITEM( LOCAL_SHORT_DAY_7, LOCALE_SABBREVDAYNAME6 );

        GET_ITEM( LOCAL_LONG_DAY_1, LOCALE_SDAYNAME7 );
        GET_ITEM( LOCAL_LONG_DAY_2, LOCALE_SDAYNAME1 );
        GET_ITEM( LOCAL_LONG_DAY_3, LOCALE_SDAYNAME2 );
        GET_ITEM( LOCAL_LONG_DAY_4, LOCALE_SDAYNAME3 );
        GET_ITEM( LOCAL_LONG_DAY_5, LOCALE_SDAYNAME4 );
        GET_ITEM( LOCAL_LONG_DAY_6, LOCALE_SDAYNAME5 );
        GET_ITEM( LOCAL_LONG_DAY_7, LOCALE_SDAYNAME6 );

        GET_ITEM( LOCAL_SHORT_MONTH_1,  LOCALE_SABBREVMONTHNAME1);
        GET_ITEM( LOCAL_SHORT_MONTH_2,  LOCALE_SABBREVMONTHNAME2);
        GET_ITEM( LOCAL_SHORT_MONTH_3,  LOCALE_SABBREVMONTHNAME3);
        GET_ITEM( LOCAL_SHORT_MONTH_4,  LOCALE_SABBREVMONTHNAME4);
        GET_ITEM( LOCAL_SHORT_MONTH_5,  LOCALE_SABBREVMONTHNAME5);
        GET_ITEM( LOCAL_SHORT_MONTH_6,  LOCALE_SABBREVMONTHNAME6);
        GET_ITEM( LOCAL_SHORT_MONTH_7,  LOCALE_SABBREVMONTHNAME7);
        GET_ITEM( LOCAL_SHORT_MONTH_8,  LOCALE_SABBREVMONTHNAME8);
        GET_ITEM( LOCAL_SHORT_MONTH_9,  LOCALE_SABBREVMONTHNAME9);
        GET_ITEM( LOCAL_SHORT_MONTH_10, LOCALE_SABBREVMONTHNAME10);
        GET_ITEM( LOCAL_SHORT_MONTH_11, LOCALE_SABBREVMONTHNAME11);
        GET_ITEM( LOCAL_SHORT_MONTH_12, LOCALE_SABBREVMONTHNAME12);

        GET_ITEM( LOCAL_LONG_MONTH_1,   LOCALE_SMONTHNAME1);
        GET_ITEM( LOCAL_LONG_MONTH_2,   LOCALE_SMONTHNAME2);
        GET_ITEM( LOCAL_LONG_MONTH_3,   LOCALE_SMONTHNAME3);
        GET_ITEM( LOCAL_LONG_MONTH_4,   LOCALE_SMONTHNAME4);
        GET_ITEM( LOCAL_LONG_MONTH_5,   LOCALE_SMONTHNAME5);
        GET_ITEM( LOCAL_LONG_MONTH_6,   LOCALE_SMONTHNAME6);
        GET_ITEM( LOCAL_LONG_MONTH_7,   LOCALE_SMONTHNAME7);
        GET_ITEM( LOCAL_LONG_MONTH_8,   LOCALE_SMONTHNAME8);
        GET_ITEM( LOCAL_LONG_MONTH_9,   LOCALE_SMONTHNAME9);
        GET_ITEM( LOCAL_LONG_MONTH_10,  LOCALE_SMONTHNAME10);
        GET_ITEM( LOCAL_LONG_MONTH_11,  LOCALE_SMONTHNAME11);
        GET_ITEM( LOCAL_LONG_MONTH_12,  LOCALE_SMONTHNAME12);

        GET_ITEM( LOCAL_DATE_SEPARATOR, LOCALE_SDATE );
        GET_ITEM( LOCAL_TIME_SEPARATOR, LOCALE_STIME );

        GET_ITEM( LOCAL_AM, LOCALE_S1159 );
        GET_ITEM( LOCAL_PM, LOCALE_S2359 );

        GET_ITEM( LOCAL_FRACTION_SEPARATOR, LOCALE_SDECIMAL );

#undef GET_ITEM

#elif defined(HAVE_NL_LANGINFO)
#define GET_ITEM(target, item) SET_ITEM(target, nl_langinfo(item))

        GET_ITEM( LOCAL_SHORT_DAY_1, ABDAY_1 );
        GET_ITEM( LOCAL_SHORT_DAY_2, ABDAY_2 );
        GET_ITEM( LOCAL_SHORT_DAY_3, ABDAY_3 );
        GET_ITEM( LOCAL_SHORT_DAY_4, ABDAY_4 );
        GET_ITEM( LOCAL_SHORT_DAY_5, ABDAY_5 );
        GET_ITEM( LOCAL_SHORT_DAY_6, ABDAY_6 );
        GET_ITEM( LOCAL_SHORT_DAY_7, ABDAY_7 );

        GET_ITEM( LOCAL_LONG_DAY_1, DAY_1 );
        GET_ITEM( LOCAL_LONG_DAY_2, DAY_2 );
        GET_ITEM( LOCAL_LONG_DAY_3, DAY_3 );
        GET_ITEM( LOCAL_LONG_DAY_4, DAY_4 );
        GET_ITEM( LOCAL_LONG_DAY_5, DAY_5 );
        GET_ITEM( LOCAL_LONG_DAY_6, DAY_6 );
        GET_ITEM( LOCAL_LONG_DAY_7, DAY_7 );

        GET_ITEM( LOCAL_SHORT_MONTH_1,  ABMON_1);
        GET_ITEM( LOCAL_SHORT_MONTH_2,  ABMON_2);
        GET_ITEM( LOCAL_SHORT_MONTH_3,  ABMON_3);
        GET_ITEM( LOCAL_SHORT_MONTH_4,  ABMON_4);
        GET_ITEM( LOCAL_SHORT_MONTH_5,  ABMON_5);
        GET_ITEM( LOCAL_SHORT_MONTH_6,  ABMON_6);
        GET_ITEM( LOCAL_SHORT_MONTH_7,  ABMON_7);
        GET_ITEM( LOCAL_SHORT_MONTH_8,  ABMON_8);
        GET_ITEM( LOCAL_SHORT_MONTH_9,  ABMON_9);
        GET_ITEM( LOCAL_SHORT_MONTH_10, ABMON_10);
        GET_ITEM( LOCAL_SHORT_MONTH_11, ABMON_11);
        GET_ITEM( LOCAL_SHORT_MONTH_12, ABMON_12);

        GET_ITEM( LOCAL_LONG_MONTH_1,   MON_1);
        GET_ITEM( LOCAL_LONG_MONTH_2,   MON_2);
        GET_ITEM( LOCAL_LONG_MONTH_3,   MON_3);
        GET_ITEM( LOCAL_LONG_MONTH_4,   MON_4);
        GET_ITEM( LOCAL_LONG_MONTH_5,   MON_5);
        GET_ITEM( LOCAL_LONG_MONTH_6,   MON_6);
        GET_ITEM( LOCAL_LONG_MONTH_7,   MON_7);
        GET_ITEM( LOCAL_LONG_MONTH_8,   MON_8);
        GET_ITEM( LOCAL_LONG_MONTH_9,   MON_9);
        GET_ITEM( LOCAL_LONG_MONTH_10,  MON_10);
        GET_ITEM( LOCAL_LONG_MONTH_11,  MON_11);
        GET_ITEM( LOCAL_LONG_MONTH_12,  MON_12);

        GET_ITEM( LOCAL_DATE_SEPARATOR, "-" );
        GET_ITEM( LOCAL_TIME_SEPARATOR, ":" );

        GET_ITEM( LOCAL_AM, AM_STR);
        GET_ITEM( LOCAL_PM, PM_STR);

        SET_ITEM( LOCAL_FRACTION_SEPARATOR, ".");

#undef GET_ITEM
#else
        
        memcpy( sys_local_names, std_local_names, sizeof(std_local_names) );
#if 0
#define GET_ITEM(target, item) SET_ITEM(target, item)

        GET_ITEM( LOCAL_SHORT_DAY_1, "Sun" );
        GET_ITEM( LOCAL_SHORT_DAY_2, "Mon");
        GET_ITEM( LOCAL_SHORT_DAY_3, "Tue" );
        GET_ITEM( LOCAL_SHORT_DAY_4, "Wed");
        GET_ITEM( LOCAL_SHORT_DAY_5, "Thu" );
        GET_ITEM( LOCAL_SHORT_DAY_6, "Fri" );
        GET_ITEM( LOCAL_SHORT_DAY_7, "Sat" );

        GET_ITEM( LOCAL_LONG_DAY_1, "Sunday" );
        GET_ITEM( LOCAL_LONG_DAY_2, "Monday" );
        GET_ITEM( LOCAL_LONG_DAY_3, "Tuesday" );
        GET_ITEM( LOCAL_LONG_DAY_4, "Wednesday" );
        GET_ITEM( LOCAL_LONG_DAY_5, "Thursday" );
        GET_ITEM( LOCAL_LONG_DAY_6, "Friday" );
        GET_ITEM( LOCAL_LONG_DAY_7, "Saturday" );

        GET_ITEM( LOCAL_SHORT_MONTH_1, "Jan");
        GET_ITEM( LOCAL_SHORT_MONTH_2, "Feb");
        GET_ITEM( LOCAL_SHORT_MONTH_3, "Mar");
        GET_ITEM( LOCAL_SHORT_MONTH_4, "Apr");
        GET_ITEM( LOCAL_SHORT_MONTH_5, "May");
        GET_ITEM( LOCAL_SHORT_MONTH_6, "Jun");
        GET_ITEM( LOCAL_SHORT_MONTH_7, "Jul");
        GET_ITEM( LOCAL_SHORT_MONTH_8, "Avg");
        GET_ITEM( LOCAL_SHORT_MONTH_9, "Sep");
        GET_ITEM( LOCAL_SHORT_MONTH_10, "Oct");
        GET_ITEM( LOCAL_SHORT_MONTH_11, "Nov");
        GET_ITEM( LOCAL_SHORT_MONTH_12, "Dec");

        GET_ITEM( LOCAL_LONG_MONTH_1, "January");
        GET_ITEM( LOCAL_LONG_MONTH_2, "February");
        GET_ITEM( LOCAL_LONG_MONTH_3, "March");
        GET_ITEM( LOCAL_LONG_MONTH_4, "April");
        GET_ITEM( LOCAL_LONG_MONTH_5, "May");
        GET_ITEM( LOCAL_LONG_MONTH_6, "June");
        GET_ITEM( LOCAL_LONG_MONTH_7, "July");
        GET_ITEM( LOCAL_LONG_MONTH_8, "August");
        GET_ITEM( LOCAL_LONG_MONTH_9, "September");
        GET_ITEM( LOCAL_LONG_MONTH_10, "October");
        GET_ITEM( LOCAL_LONG_MONTH_11, "November");
        GET_ITEM( LOCAL_LONG_MONTH_12, "December");

        GET_ITEM( LOCAL_DATE_SEPARATOR, "-" );
        GET_ITEM( LOCAL_TIME_SEPARATOR, ":" );

        GET_ITEM( LOCAL_AM, "AM");
        GET_ITEM( LOCAL_PM, "PM");

        SET_ITEM( LOCAL_FRACTION_SEPARATOR, ".");

#undef GET_ITEM
#endif

#endif

#undef SET_ITEM
        init_locale = 0;
    }

    if (locale_name == NULL)
        *ref = std_local_names;
    else if (*locale_name == 0)
        *ref = sys_local_names;

    else
        return DB_EINVLOCALE;

    return encoding;
}

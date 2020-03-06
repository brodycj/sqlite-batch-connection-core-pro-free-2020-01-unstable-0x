/**
 * SQLite connection core - low-level SQLite connection C library API
 */

#include "sqlite-connection-core.h"

#include <stdbool.h>

#include <stddef.h>

#include <math.h>

#include "sqlite3.h"

#define MAX_SCC_RECORD_COUNT 5000

#define FIRST_RECORD_ID 1

bool is_initialized = false;

// use sqlite3_mutex ref:
// https://www.sqlite.org/c3ref/mutex_alloc.html
sqlite3_mutex * _open_mutex = NULL;

#define START_OPEN_MUTEX()  do {    \
  sqlite3_mutex_enter(_open_mutex); \
} while (false)

#define END_OPEN_MUTEX()    do {    \
  sqlite3_mutex_leave(_open_mutex); \
} while (false)


typedef struct scc_record_s {
  sqlite3 * db;
  sqlite3_mutex * _st_mutex;
  sqlite3_stmt * _st;
} * scc_record_ref;

#define START_REC_ST_MUTEX(r)   do {    \
  sqlite3_mutex_enter(r->_st_mutex);    \
} while (false)

#define END_REC_ST_MUTEX(r) do {        \
  sqlite3_mutex_leave(r->_st_mutex);    \
} while (false)

static struct scc_record_s scc_record_list[MAX_SCC_RECORD_COUNT] = { NULL };

static int _scc_record_count = FIRST_RECORD_ID;

static int get_open_connection_id()
{
  START_OPEN_MUTEX();

  int connection_id = -1;

  if (_scc_record_count < MAX_SCC_RECORD_COUNT) {
    connection_id = _scc_record_count;
    ++_scc_record_count;
  }

  END_OPEN_MUTEX();

  return connection_id;
}

/**
 * NOT expected to be thread-safe, should be run in the main thread
 */
void scc_init()
{
  if (is_initialized) return;

  _open_mutex = sqlite3_mutex_alloc(SQLITE_MUTEX_RECURSIVE);
  is_initialized = true;
}

int scc_open_connection(const char * filename, int flags)
{
  int connection_id = -1;

  if (!is_initialized) {
    // BOGUS
    return -1;
  } else if ((connection_id = get_open_connection_id()) == -1) {
    // BOGUS
    return -1;
  } else {
    sqlite3 * db = NULL;

    const int open_result = sqlite3_open_v2(filename, &db, flags, NULL);

    if (open_result != SQLITE_OK) {
      return -open_result;
    } else {
      scc_record_list[connection_id].db = db;
      scc_record_list[connection_id]._st_mutex =
        sqlite3_mutex_alloc(SQLITE_MUTEX_RECURSIVE);
      scc_record_list[connection_id]._st = NULL;
      return connection_id;
    }
  }
}

int scc_begin_statement(int connection_id, const char * statement)
{
  if (connection_id < 0) {
    return 21; // SQLite abuse
  } else {
    scc_record_ref r = &scc_record_list[connection_id];
    sqlite3_stmt * st;
    int rc;

    START_REC_ST_MUTEX(r);
    st = r->_st;
    if (st != NULL) {
      rc = 21; // SQLite abuse
    } else {
      sqlite3_stmt * st;
      rc = sqlite3_prepare_v2(r->db, statement, -1, &st, NULL);
      r->_st = st;
    }
    END_REC_ST_MUTEX(r);
    return rc;
  }
}

int scc_bind_text(int connection_id, int index, const char * text)
{
  if (connection_id < 0) {
    return 21; // SQLite abuse
  } else {
    scc_record_ref r = &scc_record_list[connection_id];
    sqlite3_stmt * st;
    int rc;

    START_REC_ST_MUTEX(r);
    st = r->_st;
    if (st == NULL) {
      rc = 21; // SQLite abuse
    } else {
      rc = sqlite3_bind_text(st, index, text, -1, SQLITE_TRANSIENT);
    }
    END_REC_ST_MUTEX(r);
    return rc;
  }
}

int scc_bind_double(int connection_id, int index, double value)
{
  if (connection_id < 0) {
    return 21; // SQLite abuse
  } else {
    scc_record_ref r = &scc_record_list[connection_id];
    sqlite3_stmt * st;
    int rc;

    START_REC_ST_MUTEX(r);
    st = r->_st;
    if (st == NULL) {
      rc = 21; // SQLite abuse
    } else {
      rc = sqlite3_bind_double(st, index, value);
    }
    END_REC_ST_MUTEX(r);
    return rc;
  }
}

int scc_bind_long(int connection_id, int index, long value)
{
  if (connection_id < 0) {
    return 21; // SQLite abuse
  } else {
    scc_record_ref r = &scc_record_list[connection_id];
    sqlite3_stmt * st;
    int rc;

    START_REC_ST_MUTEX(r);
    st = r->_st;
    if (st == NULL) {
      rc = 21; // SQLite abuse
    } else {
      rc = sqlite3_bind_int64(st, index, value);
    }
    END_REC_ST_MUTEX(r);
    return rc;
  }
}

int scc_bind_null(int connection_id, int index)
{
  if (connection_id < 0) {
    return 21; // SQLite abuse
  } else {
    scc_record_ref r = &scc_record_list[connection_id];
    sqlite3_stmt * st;
    int rc;

    START_REC_ST_MUTEX(r);
    st = r->_st;
    if (st == NULL) {
      rc = 21; // SQLite abuse
    } else {
      rc = sqlite3_bind_null(st, index);
    }
    END_REC_ST_MUTEX(r);
    return rc;
  }
}

int scc_step(int connection_id)
{
  if (connection_id < 0) {
    return 21; // SQLite abuse
  } else {
    scc_record_ref r = &scc_record_list[connection_id];
    sqlite3_stmt * st;
    int rc;

    START_REC_ST_MUTEX(r);
    st = r->_st;
    if (st == NULL) {
      rc = 21; // SQLite abuse
    } else {
      rc = sqlite3_step(st);
    }
    END_REC_ST_MUTEX(r);
    return rc;
  }
}

const char * scc_get_last_error_message(int connection_id)
{
  if (connection_id < 0) {
    // BOGUS
    return "invalid connection id";
  } else {
    scc_record_ref r = &scc_record_list[connection_id];
    return sqlite3_errmsg(r->db);
  }
}

int scc_get_column_count(int connection_id)
{
  if (connection_id < 0) {
    // BOGUS
    return -1;
  } else {
    scc_record_ref r = &scc_record_list[connection_id];
    sqlite3_stmt * st;
    int rc;

    START_REC_ST_MUTEX(r);
    st = r->_st;
    if (st == NULL) {
      // BOGUS
      rc = -1;
    } else {
      rc = sqlite3_column_count(st);
    }
    END_REC_ST_MUTEX(r);
    return rc;
  }
}

const char * scc_get_column_name(int connection_id, int column)
{
  if (connection_id < 0) {
    // BOGUS
    return NULL;
  } else {
    scc_record_ref r = &scc_record_list[connection_id];
    sqlite3_stmt * st;
    const char * value;

    START_REC_ST_MUTEX(r);
    st = r->_st;
    if (st == NULL) {
      value = "";
    } else {
      value = sqlite3_column_name(st, column);
    }
    END_REC_ST_MUTEX(r);
    return value;
  }
}

int scc_get_column_type(int connection_id, int column)
{
  if (connection_id < 0) {
    // BOGUS
    return -1;
  } else {
    scc_record_ref r = &scc_record_list[connection_id];
    sqlite3_stmt * st;
    int value;

    START_REC_ST_MUTEX(r);
    st = r->_st;
    if (st == NULL) {
      // BOGUS
      value = -1;
    } else {
      value = sqlite3_column_type(st, column);
    }
    END_REC_ST_MUTEX(r);
    return value;
  }
}

const char * scc_get_column_text(int connection_id, int column)
{
  if (connection_id < 0) {
    // BOGUS
    return NULL;
  } else {
    scc_record_ref r = &scc_record_list[connection_id];
    sqlite3_stmt * st;
    const char * value;

    START_REC_ST_MUTEX(r);
    st = r->_st;
    if (st == NULL) {
      value = NULL;
    } else {
      value = (const char *)sqlite3_column_text(st, column);
      // TBD quick workaround solution to avoid a possible crash:
      if (value == NULL) value = "";
    }
    END_REC_ST_MUTEX(r);
    return value;
  }
}

double scc_get_column_double(int connection_id, int column)
{
  if (connection_id < 0) {
    // BOGUS
    return NAN;
  } else {
    scc_record_ref r = &scc_record_list[connection_id];
    sqlite3_stmt * st;
    double value;

    START_REC_ST_MUTEX(r);
    st = r->_st;
    if (st == NULL) {
      // BOGUS
      value = NAN;
    } else {
      value = sqlite3_column_double(st, column);
    }
    END_REC_ST_MUTEX(r);
    return value;
  }
}

long scc_get_column_long(int connection_id, int column)
{
  if (connection_id < 0) {
    // BOGUS
    return 0;
  } else {
    scc_record_ref r = &scc_record_list[connection_id];
    sqlite3_stmt * st;
    int rc;

    START_REC_ST_MUTEX(r);
    st = r->_st;
    if (st == NULL) {
      // BOGUS
      rc = 0;
    } else {
      rc = sqlite3_column_int64(st, column);
    }
    END_REC_ST_MUTEX(r);
    return rc;
  }
}

int scc_end_statement(int connection_id)
{
  if (connection_id < 0) {
    return 21; // SQLite abuse
  } else {
    scc_record_ref r = &scc_record_list[connection_id];
    sqlite3_stmt * st;
    int rc;

    START_REC_ST_MUTEX(r);
    st = r->_st;
    if (st == NULL) {
      rc = 21; // SQLite abuse
    } else {
      rc = sqlite3_finalize(st);
      r->_st = NULL;
    }
    END_REC_ST_MUTEX(r);
    return rc;
  }
}

int scc_get_total_changes(int connection_id)
{
  if (connection_id < 0) {
    // BOGUS
    return 0;
  } else {
    scc_record_ref r = &scc_record_list[connection_id];
    return sqlite3_total_changes(r->db);
  }
}

int scc_get_last_insert_rowid(int connection_id)
{
  if (connection_id < 0) {
    // BOGUS
    return 0;
  } else {
    scc_record_ref r = &scc_record_list[connection_id];
    return sqlite3_last_insert_rowid(r->db);
  }
}

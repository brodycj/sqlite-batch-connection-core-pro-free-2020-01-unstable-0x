#include "sqlite-connection-core.h"

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#define TEST_ASSERT_ALWAYS(x) \
  do { \
    if (!(x)) { \
      fprintf(stderr, "TEST_ASSERT_ALWAYS FAILURE on line %d\n", __LINE__); \
      exit(1); \
    } \
  } while(0)

#define TEST_ASSERT_INT_EQUALS(x, y) \
  do { \
    const int a = (x); \
    const int b = (y); \
    if ((a) != (b)) { \
      fprintf(stderr, "TEST_ASSERT_INT_EQUALS FAILURE on line %d: %d != %d\n", \
        __LINE__, (a), (b)); \
      exit(1); \
    } \
  } while(0)

#define TEST_ASSERT_STRING_EQUALS(x, y) \
  do { \
    const char * a = (x); \
    const char * b = (y); \
    if (strcmp((a), (b)) != 0) { \
      fprintf(stderr, "TEST_ASSERT_STRING_EQUALS FAILURE on line %d: %s != %s\n", \
        __LINE__, (a), (b)); \
      exit(1); \
    } \
  } while(0)

#define TEST_ASSERT_DOUBLE_EQUALS(x, y) \
  do { \
    const double a = (x); \
    const double b = (y); \
    if ((a) != (b)) { \
      fprintf(stderr, \
        "TEST_ASSERT_DOUBLE_EQUALS FAILURE on line %d: %lf != %lf\n", \
        __LINE__, (a), (b)); \
      exit(1); \
    } \
  } while(0)

#define TEST_ASSERT_LONG_EQUALS(x, y) \
  do { \
    const long long a = (x); \
    const long long b = (y); \
    if ((a) != (b)) { \
      fprintf(stderr, \
        "TEST_ASSERT_LONG_EQUALS FAILURE on line %d: %lld != %lld\n", \
        __LINE__, (a), (b)); \
      exit(1); \
    } \
  } while(0)

static void test_init() {
  scc_init();
}

static int test_open_connection(const char * test, const char * name, int flags)
{
  printf("testing %s\n", test);

  return scc_open_connection(name, flags);
}

static int test_open_memory_connection(const char * test) {
  return test_open_connection(test, ":memory:", 2);
}

static int test_open_file_connection(const char * test, const char * name) {
  // SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
  // ref: https://www.sqlite.org/c3ref/open.html
  const int flags = 6;

  return test_open_connection(test, name, flags);
}

static void test_01() {
  const int connection_id = test_open_memory_connection(__func__);

  TEST_ASSERT_ALWAYS(connection_id > 0);

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_begin_statement(connection_id, "SELECT 1")
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_end_statement(connection_id)
    );

  TEST_ASSERT_INT_EQUALS(
    21, // SQLite API abuse - as reported by this library (not from SQLite3)
    scc_end_statement(connection_id)
    );
}

static void test_02() {
  const int connection_id = test_open_memory_connection(__func__);

  TEST_ASSERT_ALWAYS(connection_id > 0);

  // syntax error
  TEST_ASSERT_INT_EQUALS(1, // SQLite ERROR
    scc_begin_statement(connection_id, "SLCT 1")
    );

  TEST_ASSERT_STRING_EQUALS(
    "near \"SLCT\": syntax error",
    scc_get_last_error_message(connection_id)
    );

  TEST_ASSERT_INT_EQUALS(
    21, // SQLite API abuse - as reported by this library (not from SQLite3)
    scc_end_statement(connection_id)
    );
}

static void test_03() {
  const int connection_id = test_open_memory_connection(__func__);

  TEST_ASSERT_ALWAYS(connection_id > 0);

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_begin_statement(connection_id, "SELECT UPPER('Test') AS myResult")
    );

  TEST_ASSERT_INT_EQUALS(100, // SQLite rows
    scc_step(connection_id)
    );

  TEST_ASSERT_INT_EQUALS(1,
    scc_get_column_count(connection_id)
    );

  TEST_ASSERT_STRING_EQUALS("myResult",
    scc_get_column_name(connection_id, 0)
    );

  TEST_ASSERT_INT_EQUALS(
    SCC_COLUMN_TYPE_TEXT,
    scc_get_column_type(connection_id, 0)
    );

  TEST_ASSERT_STRING_EQUALS("TEST",
    scc_get_column_text(connection_id, 0)
    );

  TEST_ASSERT_INT_EQUALS(101, // SQLite done
    scc_step(connection_id)
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_end_statement(connection_id)
    );
}

static void test_04() {
  const int connection_id = test_open_memory_connection(__func__);

  TEST_ASSERT_ALWAYS(connection_id > 0);

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_begin_statement(connection_id, "SELECT UPPER(?) AS myResult")
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_bind_text(connection_id, 1, "Test")
    );

  TEST_ASSERT_INT_EQUALS(100, // SQLite rows
    scc_step(connection_id)
    );

  TEST_ASSERT_INT_EQUALS(1,
    scc_get_column_count(connection_id)
    );

  TEST_ASSERT_STRING_EQUALS("myResult",
    scc_get_column_name(connection_id, 0)
    );

  TEST_ASSERT_STRING_EQUALS("TEST",
    scc_get_column_text(connection_id, 0)
    );

  TEST_ASSERT_INT_EQUALS(101, // SQLite done
    scc_step(connection_id)
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_end_statement(connection_id)
    );
}

static void test_05() {
  const int connection_id = test_open_memory_connection(__func__);

  TEST_ASSERT_ALWAYS(connection_id > 0);

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_begin_statement(connection_id, "SELECT UPPER(?),?")
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_bind_text(connection_id, 1, "Test")
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_bind_text(connection_id, 2, "abc")
    );

  TEST_ASSERT_INT_EQUALS(100, // SQLite rows
    scc_step(connection_id)
    );

  TEST_ASSERT_INT_EQUALS(2,
    scc_get_column_count(connection_id)
    );

  TEST_ASSERT_STRING_EQUALS("UPPER(?)",
    scc_get_column_name(connection_id, 0)
    );

  TEST_ASSERT_INT_EQUALS(
    SCC_COLUMN_TYPE_TEXT,
    scc_get_column_type(connection_id, 0)
    );

  TEST_ASSERT_STRING_EQUALS("TEST",
    scc_get_column_text(connection_id, 0)
    );

  TEST_ASSERT_STRING_EQUALS("?",
    scc_get_column_name(connection_id, 1)
    );

  TEST_ASSERT_INT_EQUALS(
    SCC_COLUMN_TYPE_TEXT,
    scc_get_column_type(connection_id, 1)
    );

  TEST_ASSERT_STRING_EQUALS("abc",
    scc_get_column_text(connection_id, 1)
    );

  TEST_ASSERT_INT_EQUALS(101, // SQLite done
    scc_step(connection_id)
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_end_statement(connection_id)
    );
}

static void test_06() {
  const int connection_id = test_open_memory_connection(__func__);

  TEST_ASSERT_ALWAYS(connection_id > 0);

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_begin_statement(connection_id,
      "SELECT 10.0 * ? AS myResult, -? as result2"
      )
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_bind_double(connection_id, 1, 1234567.890123)
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_bind_double(connection_id, 2, 7.89)
    );

  TEST_ASSERT_INT_EQUALS(100, // SQLite rows
    scc_step(connection_id)
    );

  TEST_ASSERT_INT_EQUALS(2,
    scc_get_column_count(connection_id)
    );

  TEST_ASSERT_STRING_EQUALS("myResult",
    scc_get_column_name(connection_id, 0)
    );

  TEST_ASSERT_INT_EQUALS(
    SCC_COLUMN_TYPE_FLOAT,
    scc_get_column_type(connection_id, 0)
    );

  TEST_ASSERT_DOUBLE_EQUALS(
    12345678.90123,
    scc_get_column_double(connection_id, 0)
    );

  TEST_ASSERT_STRING_EQUALS("result2",
    scc_get_column_name(connection_id, 1)
    );

  TEST_ASSERT_INT_EQUALS(
    SCC_COLUMN_TYPE_FLOAT,
    scc_get_column_type(connection_id, 1)
    );

  TEST_ASSERT_DOUBLE_EQUALS(
    -7.89,
    scc_get_column_double(connection_id, 1)
    );

  TEST_ASSERT_INT_EQUALS(101, // SQLite done
    scc_step(connection_id)
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_end_statement(connection_id)
    );
}

static void test_07() {
  const int connection_id = test_open_memory_connection(__func__);

  TEST_ASSERT_ALWAYS(connection_id > 0);

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_begin_statement(connection_id,
      "SELECT 10 * ? AS myResult, -? AS result2")
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_bind_long(connection_id, 1, 1234)
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_bind_long(connection_id, 2, 1234567890123456789)
    );

  TEST_ASSERT_INT_EQUALS(100, // SQLite rows
    scc_step(connection_id)
    );

  TEST_ASSERT_INT_EQUALS(2,
    scc_get_column_count(connection_id)
    );

  TEST_ASSERT_STRING_EQUALS("myResult",
    scc_get_column_name(connection_id, 0)
    );

  TEST_ASSERT_INT_EQUALS(
    SCC_COLUMN_TYPE_INTEGER,
    scc_get_column_type(connection_id, 0)
    );

  TEST_ASSERT_LONG_EQUALS(
    12340,
    scc_get_column_double(connection_id, 0)
    );

  TEST_ASSERT_STRING_EQUALS("result2",
    scc_get_column_name(connection_id, 1)
    );

  TEST_ASSERT_INT_EQUALS(
    SCC_COLUMN_TYPE_INTEGER,
    scc_get_column_type(connection_id, 1)
    );

  TEST_ASSERT_LONG_EQUALS(
    -1234567890123456789,
    scc_get_column_long(connection_id, 1)
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_end_statement(connection_id)
    );
}

static void test_08() {
  const int connection_id = test_open_memory_connection(__func__);

  TEST_ASSERT_ALWAYS(connection_id > 0);

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_begin_statement(connection_id, "SELECT UPPER(?), ?")
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_bind_text(connection_id, 1, "Test")
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_bind_null(connection_id, 2)
    );

  TEST_ASSERT_INT_EQUALS(100, // SQLite rows
    scc_step(connection_id)
    );

  TEST_ASSERT_INT_EQUALS(2,
    scc_get_column_count(connection_id)
    );

  TEST_ASSERT_STRING_EQUALS("UPPER(?)",
    scc_get_column_name(connection_id, 0)
    );

  TEST_ASSERT_INT_EQUALS(
    SCC_COLUMN_TYPE_TEXT,
    scc_get_column_type(connection_id, 0)
    );

  TEST_ASSERT_STRING_EQUALS(
    "TEST",
    scc_get_column_text(connection_id, 0)
    );

  TEST_ASSERT_STRING_EQUALS("?",
    scc_get_column_name(connection_id, 1)
    );

  TEST_ASSERT_INT_EQUALS(
    SCC_COLUMN_TYPE_NULL,
    scc_get_column_type(connection_id, 1)
    );

  // ensure this does not crash:
  TEST_ASSERT_STRING_EQUALS(
    "",
    scc_get_column_text(connection_id, 1)
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_end_statement(connection_id)
    );
}

static void test_11() {
  const int connection_id = test_open_file_connection(__func__, "test_11.db");

  TEST_ASSERT_ALWAYS(connection_id > 0);

  TEST_ASSERT_INT_EQUALS(0, scc_get_total_changes(connection_id));
  TEST_ASSERT_INT_EQUALS(0, scc_get_last_insert_rowid(connection_id));

  {
    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_begin_statement(connection_id,
        "SELECT UPPER('Test') AS myResult")
      );

    TEST_ASSERT_INT_EQUALS(100, // SQLite rows
      scc_step(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(1,
      scc_get_column_count(connection_id)
      );

    TEST_ASSERT_STRING_EQUALS("myResult",
      scc_get_column_name(connection_id, 0)
      );

    TEST_ASSERT_STRING_EQUALS(
      "TEST",
      scc_get_column_text(connection_id, 0)
      );

    TEST_ASSERT_INT_EQUALS(0, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(0, scc_get_last_insert_rowid(connection_id));

    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_end_statement(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(0, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(0, scc_get_last_insert_rowid(connection_id));
  }

  {
    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_begin_statement(connection_id, "CREATE TABLE Testing (data)")
      );

    TEST_ASSERT_INT_EQUALS(101, // SQLite done
      scc_step(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_end_statement(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(0, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(0, scc_get_last_insert_rowid(connection_id));
  }

  {
    // zero (0) rows expected

    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_begin_statement(connection_id, "SELECT * FROM Testing")
      );

    TEST_ASSERT_INT_EQUALS(101, // SQLite done
      scc_step(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(0, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(0, scc_get_last_insert_rowid(connection_id));

    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_end_statement(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(0, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(0, scc_get_last_insert_rowid(connection_id));
  }

  {
    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_begin_statement(connection_id,
        "INSERT INTO Testing VALUES ('test data')"
        )
      );

    TEST_ASSERT_INT_EQUALS(101, // SQLite done
      scc_step(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(1, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(1, scc_get_last_insert_rowid(connection_id));

    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_end_statement(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(1, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(1, scc_get_last_insert_rowid(connection_id));
  }

  {
    // 1 row expected

    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_begin_statement(connection_id, "SELECT * FROM Testing")
      );

    TEST_ASSERT_INT_EQUALS(100, // SQLite rows
      scc_step(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(1,
      scc_get_column_count(connection_id)
      );

    TEST_ASSERT_STRING_EQUALS("data",
      scc_get_column_name(connection_id, 0)
      );

    TEST_ASSERT_INT_EQUALS(
      SCC_COLUMN_TYPE_TEXT,
      scc_get_column_type(connection_id, 0)
      );

    TEST_ASSERT_STRING_EQUALS(
      "test data",
      scc_get_column_text(connection_id, 0)
      );

    TEST_ASSERT_INT_EQUALS(1, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(1, scc_get_last_insert_rowid(connection_id));

    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_end_statement(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(1, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(1, scc_get_last_insert_rowid(connection_id));
  }
}

static void test_12() {
  const int connection_id = test_open_file_connection(__func__, "test_12.db");

  TEST_ASSERT_ALWAYS(connection_id > 0);

  TEST_ASSERT_INT_EQUALS(0, scc_get_total_changes(connection_id));
  TEST_ASSERT_INT_EQUALS(0, scc_get_last_insert_rowid(connection_id));

  {
    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_begin_statement(connection_id, "CREATE TABLE Testing (data)")
      );

    TEST_ASSERT_INT_EQUALS(101, // SQLite done
      scc_step(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_end_statement(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(0, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(0, scc_get_last_insert_rowid(connection_id));
  }

  {
    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_begin_statement(connection_id,
        "INSERT INTO Testing VALUES ('test data')"
        )
      );

    TEST_ASSERT_INT_EQUALS(101, // SQLite done
      scc_step(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(1, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(1, scc_get_last_insert_rowid(connection_id));

    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_end_statement(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(1, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(1, scc_get_last_insert_rowid(connection_id));
  }

  {
    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_begin_statement(connection_id, "DELETE FROM Testing")
      );

    TEST_ASSERT_INT_EQUALS(101, // SQLite done
      scc_step(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(2, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(1, scc_get_last_insert_rowid(connection_id));

    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_end_statement(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(2, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(1, scc_get_last_insert_rowid(connection_id));
  }

  {
    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_begin_statement(connection_id,
        "INSERT INTO Testing VALUES ('test 2')"
        )
      );

    TEST_ASSERT_INT_EQUALS(101, // SQLite done
      scc_step(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(3, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(1, scc_get_last_insert_rowid(connection_id));

    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_end_statement(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(3, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(1, scc_get_last_insert_rowid(connection_id));
  }

  {
    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_begin_statement(connection_id,
        "INSERT INTO Testing VALUES ('test 3')"
        )
      );

    TEST_ASSERT_INT_EQUALS(101, // SQLite done
      scc_step(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(4, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(2, scc_get_last_insert_rowid(connection_id));

    TEST_ASSERT_INT_EQUALS(0, // SQLite OK
      scc_end_statement(connection_id)
      );

    TEST_ASSERT_INT_EQUALS(4, scc_get_total_changes(connection_id));
    TEST_ASSERT_INT_EQUALS(2, scc_get_last_insert_rowid(connection_id));
  }
}

static void test_21() {
  // INCORRECT call with incorrect flags:
  const int connection_id = test_open_connection(__func__, "dummy.db", 0);
  // EXPECTED to indicate an error:
  TEST_ASSERT_ALWAYS(connection_id < 0);
}

int main(int argc, char ** argv) {
  test_init();

  test_01();
  test_02();
  test_03();
  test_04();
  test_05();
  test_06();
  test_07();
  test_08();

  test_11();
  test_12();

  test_21();
}

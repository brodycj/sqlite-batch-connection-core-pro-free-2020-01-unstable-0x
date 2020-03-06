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

static int get_memory_test_connection(const char * test_func) {
  printf("running %s\n", test_func);

  scc_init();

  return scc_open_connection(":memory:", 2);
}

static void test01() {
  const int connection_id = get_memory_test_connection(__func__);

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

static void test02() {
  const int connection_id = get_memory_test_connection(__func__);

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

static void test03() {
  const int connection_id = get_memory_test_connection(__func__);

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

static void test04() {
  const int connection_id = get_memory_test_connection(__func__);

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

static void test05() {
  const int connection_id = get_memory_test_connection(__func__);

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

static void test06() {
  const int connection_id = get_memory_test_connection(__func__);

  TEST_ASSERT_ALWAYS(connection_id > 0);

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_begin_statement(connection_id,
      "SELECT 10.0 * ? AS myResult, -? as result2"
      )
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_bind_double(connection_id, 1, 123.456)
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
    1234.56,
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

static void test07() {
  const int connection_id = get_memory_test_connection(__func__);

  TEST_ASSERT_ALWAYS(connection_id > 0);

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_begin_statement(connection_id,
      "SELECT 10 * ? AS myResult, -? AS result2")
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_bind_long(connection_id, 1, 1234)
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_bind_long(connection_id, 2, 567)
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
    -567,
    scc_get_column_double(connection_id, 1)
    );

  TEST_ASSERT_INT_EQUALS(0, // SQLite OK
    scc_end_statement(connection_id)
    );
}

static void test08() {
  const int connection_id = get_memory_test_connection(__func__);

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

static void test10() {
  const int connection_id = get_memory_test_connection(__func__);

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

static void test11() {
  const int connection_id = get_memory_test_connection(__func__);

  TEST_ASSERT_ALWAYS(connection_id > 0);

  TEST_ASSERT_ALWAYS(scc_get_total_changes(connection_id) == 0);
  TEST_ASSERT_ALWAYS(scc_get_last_insert_rowid(connection_id) == 0);

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

int main(int argc, char ** argv) {
  test01();
  test02();
  test03();
  test04();
  test05();
  test06();
  test07();
  test08();
  test10();
  test11();
}

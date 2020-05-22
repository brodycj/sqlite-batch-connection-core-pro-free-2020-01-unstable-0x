import io.sqlc.SCCoreGlue;

class SCCoreGlueTest {
  static void assertTrue(boolean test) {
    if (!test) {
      System.out.println("test failure, throwing an exception");
      throw new RuntimeException("failed test assertion");
    }
  }

  static void assertEquals(final int a, final int b) {
    if (a != b) {
      System.out.println("assertEquals failure: " + a + " != " + b);
      System.out.println("test failure, throwing an exception");
      throw new RuntimeException("failed test assertion");
    }
  }

  static void assertEquals(final String a, final String b) {
    if (!a.equals(b)) {
      System.out.println("assertEquals failure: " + a + " != " + b);
      System.out.println("test failure, throwing an exception");
      throw new RuntimeException("failed test assertion");
    }
  }


  static void assertEquals(final double a, final double b) {
    if (a != b) {
      System.out.println("assertEquals failure: " + a + " != " + b);
      System.out.println("test failure, throwing an exception");
      throw new RuntimeException("failed test assertion");
    }
  }

  static int
  testOpenConnection(final String test, final String name, final int flags) {
    System.out.println(test);

    return SCCoreGlue.scc_open_connection(name, flags);
  }

  static int testOpenMemoryConnection(final String test) {
    return testOpenConnection(test, ":memory:", 2);
  }

  static int testOpenFileConnection(final String test, final String name) {
    return testOpenConnection(test, name, 6);
  }

  static void test1() {
    final int connection = testOpenMemoryConnection("test 1");

    assertTrue(connection > 0);

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_begin_statement(connection,
        "SELECT UPPER('Test') as myResult"
        )
      );

    assertEquals(100, // SQLite rows
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(1,
      SCCoreGlue.scc_get_column_count(connection)
      );

    assertEquals(
      "myResult",
      SCCoreGlue.scc_get_column_name(connection, 0)
      );

    assertEquals(
      SCCoreGlue.SCC_COLUMN_TYPE_TEXT,
      SCCoreGlue.scc_get_column_type(connection, 0)
      );

    assertEquals(
      "TEST",
      SCCoreGlue.scc_get_column_text(connection, 0)
      );

    assertEquals(101, // SQLite done
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_end_statement(connection)
      );
  }

  static void test2() {
    final int connection = testOpenMemoryConnection("test 2");

    assertTrue(connection > 0);

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_begin_statement(connection,
        "SELECT UPPER(?) AS myResult, ? as result2"
        )
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_bind_text(connection, 1, "Text")
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_bind_text(connection, 2, "abc")
      );

    assertEquals(100, // SQLite rows
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(2,
      SCCoreGlue.scc_get_column_count(connection)
      );

    assertEquals(
      "myResult",
      SCCoreGlue.scc_get_column_name(connection, 0)
      );

    assertEquals(
      SCCoreGlue.SCC_COLUMN_TYPE_TEXT,
      SCCoreGlue.scc_get_column_type(connection, 0)
      );

    assertEquals(
      "TEXT",
      SCCoreGlue.scc_get_column_text(connection, 0)
      );

    assertEquals(
      "result2",
      SCCoreGlue.scc_get_column_name(connection, 1)
      );

    assertEquals(
      SCCoreGlue.SCC_COLUMN_TYPE_TEXT,
      SCCoreGlue.scc_get_column_type(connection, 1)
      );

    assertEquals(
      "abc",
      SCCoreGlue.scc_get_column_text(connection, 1)
      );

    assertEquals(101, // SQLite done
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_end_statement(connection)
      );
  }

  static void test3() {
    final int connection = testOpenMemoryConnection("test 3");

    assertTrue(connection > 0);

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_begin_statement(connection,
        "SELECT -? AS myResult, (? / 10.0) as result2"
        )
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_bind_double(connection, 1, 123.45)
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_bind_double(connection, 2, 67.89)
      );

    assertEquals(100, // SQLite rows
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(2,
      SCCoreGlue.scc_get_column_count(connection)
      );

    assertEquals(
      "myResult",
      SCCoreGlue.scc_get_column_name(connection, 0)
      );

    assertEquals(
      SCCoreGlue.SCC_COLUMN_TYPE_FLOAT,
      SCCoreGlue.scc_get_column_type(connection, 0)
      );

    assertEquals(
      "-123.45",
      SCCoreGlue.scc_get_column_text(connection, 0)
      );

    assertEquals(
      -123.45,
      SCCoreGlue.scc_get_column_double(connection, 0)
      );

    assertEquals(
      "result2",
      SCCoreGlue.scc_get_column_name(connection, 1)
      );

    assertEquals(
      SCCoreGlue.SCC_COLUMN_TYPE_FLOAT,
      SCCoreGlue.scc_get_column_type(connection, 1)
      );

    assertEquals(
      "6.789",
      SCCoreGlue.scc_get_column_text(connection, 1)
      );

    assertEquals(
      6.789,
      SCCoreGlue.scc_get_column_double(connection, 1)
      );

    assertEquals(101, // SQLite done
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_end_statement(connection)
      );
  }

  static void test4() {
    final int connection = testOpenMemoryConnection("test 4");

    assertTrue(connection > 0);

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_begin_statement(connection,
        "SELECT UPPER(?), LOWER(?), ?, -?, (10 * ?)"
        )
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_bind_text(connection, 1, "Text")
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_bind_text(connection, 2, "ABC")
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_bind_null(connection, 3)
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_bind_double(connection, 4, 12345678.90123)
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_bind_long(connection, 5, 123456789012345678L)
      );

    assertEquals(100, // SQLite rows
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(5,
      SCCoreGlue.scc_get_column_count(connection)
      );

    final String columnName = SCCoreGlue.scc_get_column_name(connection, 0);
    assertEquals(columnName, "UPPER(?)");

    final int columnType = SCCoreGlue.scc_get_column_type(connection, 0);
    assertEquals(columnType, SCCoreGlue.SCC_COLUMN_TYPE_TEXT);

    final String columnText = SCCoreGlue.scc_get_column_text(connection, 0);
    assertEquals(columnText, "TEXT");

    final String columnName2 = SCCoreGlue.scc_get_column_name(connection, 1);
    assertEquals(columnName2, "LOWER(?)");

    final int columnType2 = SCCoreGlue.scc_get_column_type(connection, 1);
    assertEquals(columnType2, SCCoreGlue.SCC_COLUMN_TYPE_TEXT);

    final String columnText2 = SCCoreGlue.scc_get_column_text(connection, 1);
    assertEquals(columnText2, "abc");

    final String columnName3 = SCCoreGlue.scc_get_column_name(connection, 2);
    assertEquals(columnName3, "?");

    final int columnType3 = SCCoreGlue.scc_get_column_type(connection, 2);
    assertEquals(columnType3, SCCoreGlue.SCC_COLUMN_TYPE_NULL);

    final String columnText3 = SCCoreGlue.scc_get_column_text(connection, 2);
    assertEquals(columnText3, "");

    final String columnName4 = SCCoreGlue.scc_get_column_name(connection, 3);
    assertEquals(columnName4, "-?");

    final int columnType4 = SCCoreGlue.scc_get_column_type(connection, 3);
    assertEquals(columnType4, SCCoreGlue.SCC_COLUMN_TYPE_FLOAT);

    final String columnText4 = SCCoreGlue.scc_get_column_text(connection, 3);
    assertEquals(columnText4, "-12345678.90123");

    final double columnValue4 = SCCoreGlue.scc_get_column_double(connection, 3);
    assertEquals(columnValue4, -12345678.90123);

    final String columnName5 = SCCoreGlue.scc_get_column_name(connection, 4);
    assertEquals(columnName5, "(10 * ?)");

    final int columnType5 = SCCoreGlue.scc_get_column_type(connection, 4);
    assertEquals(columnType5, SCCoreGlue.SCC_COLUMN_TYPE_INTEGER);

    final String columnText5 = SCCoreGlue.scc_get_column_text(connection, 4);
    assertEquals(columnText5, "1234567890123456780");

    final long columnValue5 = SCCoreGlue.scc_get_column_long(connection, 4);
    assertEquals(columnValue5, 1234567890123456780L);

    assertEquals(101, // SQLite done
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_end_statement(connection)
      );
  }

  static void test5() {
    final int connection = testOpenMemoryConnection("test 5");

    assertTrue(connection > 0);

    // syntax error:
    assertEquals(1, // SQLite ERROR
      SCCoreGlue.scc_begin_statement(connection,"SLCT 1")
      );

    assertEquals(
      "near \"SLCT\": syntax error",
      SCCoreGlue.scc_get_last_error_message(connection)
      );

    assertEquals(21, // SQLite API abuse
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(
      21, // SQLite API abuse - as reported by this library (not from SQLite3)
      SCCoreGlue.scc_end_statement(connection)
      );
  }

  static void test6() {
    final int connection = testOpenMemoryConnection("test 6");

    assertTrue(connection > 0);

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_begin_statement(connection, "SELECT UPPER(?)")
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_bind_text(connection, 1, "Test")
      );

    assertEquals(25, // SQLite range error
      SCCoreGlue.scc_bind_text(connection, 2, "abc")
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_end_statement(connection)
      );
  }

  static void test11() {
    final int connection = testOpenFileConnection("test 11", "test11.db");

    assertTrue(connection > 0);

    assertEquals(0, SCCoreGlue.scc_get_total_changes(connection));
    assertEquals(0, SCCoreGlue.scc_get_last_insert_rowid(connection));

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_begin_statement(connection,
        "SELECT UPPER('Test') as myResult"
        )
      );

    assertEquals(100, // SQLite rows
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(1,
      SCCoreGlue.scc_get_column_count(connection)
      );

    assertEquals(
      "myResult",
      SCCoreGlue.scc_get_column_name(connection, 0)
      );

    assertEquals(
      SCCoreGlue.SCC_COLUMN_TYPE_TEXT,
      SCCoreGlue.scc_get_column_type(connection, 0)
      );

    assertEquals(
      "TEST",
      SCCoreGlue.scc_get_column_text(connection, 0)
      );

    assertEquals(0, SCCoreGlue.scc_get_total_changes(connection));
    assertEquals(0, SCCoreGlue.scc_get_last_insert_rowid(connection));

    assertEquals(101, // SQLite done
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_end_statement(connection)
      );

    assertEquals(0, SCCoreGlue.scc_get_total_changes(connection));
    assertEquals(0, SCCoreGlue.scc_get_last_insert_rowid(connection));

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_begin_statement(connection,
        "CREATE TABLE Testing (data)"
        )
      );

    assertEquals(101, // SQLite done
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_end_statement(connection)
      );

    assertEquals(0, SCCoreGlue.scc_get_total_changes(connection));
    assertEquals(0, SCCoreGlue.scc_get_last_insert_rowid(connection));

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_begin_statement(connection,
        "INSERT INTO Testing VALUES ('test data')"
        )
      );

    assertEquals(101, // SQLite done
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(1, SCCoreGlue.scc_get_total_changes(connection));
    assertEquals(1, SCCoreGlue.scc_get_last_insert_rowid(connection));

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_end_statement(connection)
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_begin_statement(connection,
        "DELETE FROM Testing"
        )
      );

    assertEquals(101, // SQLite done
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(2, SCCoreGlue.scc_get_total_changes(connection));
    assertEquals(1, SCCoreGlue.scc_get_last_insert_rowid(connection));

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_end_statement(connection)
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_begin_statement(connection,
        "INSERT INTO Testing VALUES ('test 2')"
        )
      );

    assertEquals(101, // SQLite done
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(3, SCCoreGlue.scc_get_total_changes(connection));
    assertEquals(1, SCCoreGlue.scc_get_last_insert_rowid(connection));

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_end_statement(connection)
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_begin_statement(connection,
        "INSERT INTO Testing VALUES ('test 3')"
        )
      );

    assertEquals(101, // SQLite done
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(4, SCCoreGlue.scc_get_total_changes(connection));
    assertEquals(2, SCCoreGlue.scc_get_last_insert_rowid(connection));

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_end_statement(connection)
      );
  }

  static void test21() {
    // INCORRECT call with incorrect flags:
    final int connection = testOpenConnection("test 21", "dummy.db", 0);
    // EXPECTED to indicate an error:
    assertTrue(connection < 0);
  }

  public static void main(String [] args) {
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test11();
    test21();
  }

  static {
    System.loadLibrary("sqlc-connection-core-glue");
    SCCoreGlue.scc_init();
  }
}

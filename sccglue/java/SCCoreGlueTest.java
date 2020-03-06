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

  static void test1() {
    System.out.println("test 1");

    final int connection = SCCoreGlue.scc_open_connection(":memory:", 2);
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
    System.out.println("test 2");

    final int connection = SCCoreGlue.scc_open_connection(":memory:", 2);
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
    System.out.println("test 3");

    final int connection = SCCoreGlue.scc_open_connection(":memory:", 2);
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
    System.out.println("test 4");

    final int connection = SCCoreGlue.scc_open_connection(":memory:", 2);
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
      SCCoreGlue.scc_bind_double(connection, 4, 12.34)
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_bind_long(connection, 5, 5678)
      );

    assertEquals(100, // SQLite rows
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(5,
      SCCoreGlue.scc_get_column_count(connection)
      );

    final String columnName = SCCoreGlue.scc_get_column_name(connection, 0);
    assertTrue(columnName.equals("UPPER(?)"));

    final int columnType = SCCoreGlue.scc_get_column_type(connection, 0);
    assertTrue(columnType == SCCoreGlue.SCC_COLUMN_TYPE_TEXT);

    final String columnText = SCCoreGlue.scc_get_column_text(connection, 0);
    assertTrue(columnText.equals("TEXT"));

    final String columnName2 = SCCoreGlue.scc_get_column_name(connection, 1);
    assertTrue(columnName2.equals("LOWER(?)"));

    final int columnType2 = SCCoreGlue.scc_get_column_type(connection, 1);
    assertTrue(columnType2 == SCCoreGlue.SCC_COLUMN_TYPE_TEXT);

    final String columnText2 = SCCoreGlue.scc_get_column_text(connection, 1);
    assertTrue(columnText2.equals("abc"));

    final String columnName3 = SCCoreGlue.scc_get_column_name(connection, 2);
    assertTrue(columnName3.equals("?"));

    final int columnType3 = SCCoreGlue.scc_get_column_type(connection, 2);
    assertTrue(columnType3 == SCCoreGlue.SCC_COLUMN_TYPE_NULL);

    final String columnText3 = SCCoreGlue.scc_get_column_text(connection, 2);
    assertTrue(columnText3.equals(""));

    final String columnName4 = SCCoreGlue.scc_get_column_name(connection, 3);
    assertTrue(columnName4.equals("-?"));

    final int columnType4 = SCCoreGlue.scc_get_column_type(connection, 3);
    assertTrue(columnType4 == SCCoreGlue.SCC_COLUMN_TYPE_FLOAT);

    final String columnText4 = SCCoreGlue.scc_get_column_text(connection, 3);
    assertTrue(columnText4.equals("-12.34"));

    final double columnValue4 = SCCoreGlue.scc_get_column_double(connection, 3);
    assertTrue(columnValue4 == -12.34);

    final String columnName5 = SCCoreGlue.scc_get_column_name(connection, 4);
    assertTrue(columnName5.equals("(10 * ?)"));

    final int columnType5 = SCCoreGlue.scc_get_column_type(connection, 4);
    assertTrue(columnType5 == SCCoreGlue.SCC_COLUMN_TYPE_INTEGER);

    final String columnText5 = SCCoreGlue.scc_get_column_text(connection, 4);
    assertTrue(columnText5.equals("56780"));

    final long columnValue5 = SCCoreGlue.scc_get_column_long(connection, 4);
    assertTrue(columnValue5 == 56780);

    assertEquals(101, // SQLite done
      SCCoreGlue.scc_step(connection)
      );

    assertEquals(0, // SQLite OK
      SCCoreGlue.scc_end_statement(connection)
      );
  }

  static void test5() {
    System.out.println("test 5");

    final int connection = SCCoreGlue.scc_open_connection(":memory:", 2);
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
    System.out.println("test 6");

    final int connection = SCCoreGlue.scc_open_connection(":memory:", 2);
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

  static void test7() {
    System.out.println("test 7");

    final int connection = SCCoreGlue.scc_open_connection(":memory:", 2);
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

  public static void main(String [] args) {
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
  }

  static {
    System.loadLibrary("sqlite-connection-core-glue");
    SCCoreGlue.scc_init();
  }
}

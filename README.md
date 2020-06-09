# SQLite batch connection core 2020-01

**Author:** Christopher J. Brody <mailto:chris.brody+brodybits@gmail.com>

**License:** MIT with commercial license option available

**IMPORTANT UPGRADE NOTICE:** It is highly recommended to avoid breaking schema changes, database file name changes, and database directory path changes. Upgrades need to account for any old schema versions and database file paths that may still be in use. It is possible for users to upgrade at any time, even after many years.

**IMPORTANT CORRUPTION NOTICE 1:** SQLite database corruption is possible if accessed from multiple libraries, for example using both this library and built-in `android.sqlite.database` on Android ref:
- <https://ericsink.com/entries/multiple_sqlite_problem.html>
- <https://www.sqlite.org/faq.html#q5>
- <https://github.com/xpbrew/cordova-sqlite-storage/issues/626>

**IMPORTANT CORRUPTION NOTICE 2:** It is **highly** recommended to use `-DSQLITE_DEFAULT_SYNCHRONOUS=3` build setting to be extra-durable against crashes and power loss ref:
- <https://github.com/xpbrew/cordova-sqlite-storage-help/issues/34>
- <https://github.com/xpbrew/cordova-sqlite-storage/issues/736>
- <http://sqlite.1065341.n5.nabble.com/Is-WAL-mode-more-robust-against-corruption-td99624.html>

## About

SQLite batch processing for Android and iOS, including low-level modules for C, C++, Objective-C, and Java

intended for easy integration into plugin packages for Apache Cordova and React Native

includes simple SQLite connection library modules for C, C++, Objective-C, and Java

with demonstration of use in an extremely simple Cordova plugin for mobile apps in JavaScript

designed to be thread-safe

with support available here: <https://github.com/brodybits/ask-me-anything/issues>

## Contents

- `android` - `io.sqlc.SQLiteBatchCore` Java module, requires JAR built from `sccglue`
- `ios` - `SQLiteBatchCore` Objective-C module, requires the `sqlite-connection-core.c` C module
- `sqlite-connection-core.h` - main low-level C API header
- `sqlite-connection-core.c` - main low-level C library source module
- `ctest` - test of main low-level C library
- `sccglue` - low-level Java JNI API wrapper, generated with help from GlueGen from jogamp.org, with JNI test for macOS included
- `cordova-demo` - extremely simple Cordova demo app for testing, reformatted by `prettier-standard`, includes Cordova demo plugin:
  - `cordova-sqlite-demo-plugin` - extremely simple Cordova plugin that can open a SQLite database, execute a set of batch statements with parameters, and send the results to the Cordova JavaScript app, now with background processing
- with some other top-level Makefile artifacts included

## Major features

- The API is designed to work with an integer connection ID that is easy to use from higher-level library packages for frameworks such as Apache Cordova and React Native.
- Reports the last insert row ID and total number of changes which are needed to help match some of the capabilities of the *deprecated draft* Web SQL specification.
- Includes some extremely rudimentary error handling.

## Some known limitations

- When using the low-level C or Java API, the required `scc_init()` API initialization function which is NOT thread-safe **must** be called from the main thread upon startup. (This is automatically done by the batch processing classes for Android and iOS.)
- When using the low-level Java API, `System.loadLibrary()` **must** be used to load the JNI implementation before calling any API functions including `scc_init()`, as shown in the Java API sample below. (This is automatically done by the `io.sqlc.SQLiteBatchCore` class on Android.)
- Not tested with `Infinity`, `-Infinity`, or `NaN` values.
- 64-bit integer values are not supported by the SQLiteBatchCore classes for Android and iOS. The Cordova demo is tested with integer values up to 52-bits (signed).
- For Apache Cordova, a helper plugin such as `cordova-sqlite-storage-file` (recommended) or `cordova-plugin-file` (recommended for external filesystem access on Android) should be used to resolve an absolute database file path before opening it. Similar helper components would be recommended on React Native.
- not able to close database connection and release internal resources
- hard limit of 1000 open SQLite database connections, which can be changed by defining `SCC_MAXIMUM_CONNECTIONS` to configure the hard limit when building
- The API was not designed to support parallel database access through the same database connection. For parallel database access it is recommended to open multiple SQLite connections to the same database file name.
- A limited number of historical SQLite features are disabled since the `SQLITE_DBCONFIG_DEFENSIVE` option is enabled (unless `NO_SCC_DBCONFIG_DEFENSIVE` is defined when building) ref: <https://www.sqlite.org/c3ref/c_dbconfig_defensive.html#sqlitedbconfigdefensive>
- Background threading would need to be done in a higher-level component, which should be straightforward on both Android and iOS, as now demonstrated in the Cordova demo plugin.
- The `sqlite-connection-core.h` API header file and Java interface class have very limited documentation comments.
- Formal documentation of the API is missing here.
- Missing both demonstration and formal documentation of SQLiteBatchCore class for Android and iOS, which is demonstrated in `cordova-sqlite-demo-plugin` subdirectory of `cordova-demo`.

## Samples

### Low-level API samples

**Low-level C sample:**

```c
#include "sqlite-connection-core.h"

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

static void demo() {
  const int connection_id = scc_open_connection(":memory:", 2);

  int result_check;

  if (connection_id < 0) {
    fprintf(stderr, "could not open connection");
    exit(1);
  }

  result_check = scc_begin_statement(connection_id,
    "SELECT UPPER(?) AS result1, -? as result2");
  if (result_check != 0) {
    fprintf(stderr, "could not prepare statement");
    exit(1);
  }

  result_check = scc_bind_text(connection_id, 1, "Test");
  if (result_check != 0) {
    fprintf(stderr, "could not bind text");
    exit(1);
  }

  result_check = scc_bind_double(connection_id, 2, 123.456789);
  if (result_check != 0) {
    fprintf(stderr, "could not bind double");
    exit(1);
  }

  // should get rows:
  while (scc_step(connection_id) == 100) {
    int column_index;
    const int column_count = scc_get_column_count(connection_id);

    printf("column count: %d\n", column_count);

    for (column_index = 0; column_index < column_count; ++column_index) {
      int column_type;

      printf("column index: %d\n", column_index);

      column_type = scc_get_column_type(connection_id, column_index);

      printf("  column type: %d\n", column_index);

      if (column_type == SCC_COLUMN_TYPE_FLOAT ||
          column_type == SCC_COLUMN_TYPE_INTEGER) {
        double doubleValue = scc_get_column_double(connection_id, column_index);
        printf("  double column value: %lf\n", doubleValue);
      } else {
        const char * textValue =
          scc_get_column_text(connection_id, column_index);
        printf("  text value: %s\n", textValue);
      }
    }
  }

  scc_end_statement(connection_id);
}

int main(int argc, char ** argv) {
  scc_init();
  demo();
}
```

**Low-level Java sample:**

```java
import io.sqlc.SCCoreGlue;

class SQLiteDemo {
  public static void main(String [] args) {
    final int connection_id = SCCoreGlue.scc_open_connection(":memory:", 2);

    if (connection_id < 0) {
      throw new RuntimeException("could not open connection");
    }

    int resultCheck;

    resultCheck = SCCoreGlue.scc_begin_statement(connection_id,
      "SELECT UPPER(?) AS result1, -? as result2");
    if (resultCheck != 0) {
      throw new RuntimeException("could not prepare statement");
    }

    resultCheck = SCCoreGlue.scc_bind_text(connection_id, 1, "Test");
    if (resultCheck != 0) {
      throw new RuntimeException("could not bind text");
    }

    resultCheck = SCCoreGlue.scc_bind_double(connection_id, 2, 123.456789);
    if (resultCheck != 0) {
      throw new RuntimeException("could not bind double");
    }

    // should get rows:
    while (SCCoreGlue.scc_step(connection_id) == 100) {
      final int column_count = SCCoreGlue.scc_get_column_count(connection_id);

      System.out.println("column count: " + column_count);

      for (int column_index = 0; column_index < column_count; ++column_index) {
        System.out.println("column index: " + column_index);

        int column_type =
          SCCoreGlue.scc_get_column_type(connection_id, column_index);

        System.out.println("  column type: " + column_index);

        if (column_type == SCCoreGlue.SCC_COLUMN_TYPE_FLOAT ||
            column_type == SCCoreGlue.SCC_COLUMN_TYPE_INTEGER) {
          double doubleValue =
            SCCoreGlue.scc_get_column_double(connection_id, column_index);
          System.out.println("  double column value: " + doubleValue);
        } else {
          String textValue =
            SCCoreGlue.scc_get_column_text(connection_id, column_index);
          System.out.println("  text value: " + textValue);
        }
      }
    }

    SCCoreGlue.scc_end_statement(connection_id);
  }

  static {
    System.loadLibrary("sqlc-connection-core-glue");
    SCCoreGlue.scc_init();
  }
}
```

**Low-level API sample output:**

The C and Java samples above would both show the following output:

```
column count: 2
column index: 0
  column type: 0
  text value: TEST
column index: 1
  column type: 1
  double column value: -123.456789
```

### Apache Cordova demo app

Demonstrates accessing both memory database and database files from an Apache Cordova application, now with background processing, with help from the `cordova-sqlite-storage-file` and `cordova-plugin-file` plugins.

#### index.html

```html
<html>
  <head>
    <link rel="stylesheet" href="body.css" />
  </head>

  <body>
    <font face="Arial" />
    <h1>Cordova demo app</h1>
    <div id="messages" />
  </body>

  <script src="cordova.js"></script>
  <script src="app.js"></script>
</html>
```

#### body.css

needed for the HTML body to show up on macOS ("osx")

```css
body {
  background-color: #fff;
}
```

#### app.js

(reformatted by `prettier-standard`)

```js
document.addEventListener('deviceready', onReady)

// based on some JavaScript code generated by generate-cordova-package
function log (text) {
  // log into the `messages` div:
  document.getElementById('messages').appendChild(document.createTextNode(text))
  document.getElementById('messages').appendChild(document.createElement('br'))
  // and to the console
  console.log(text)
}

const DATABASE_FILE_NAME = 'demo.db'

// SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
// ref: https://www.sqlite.org/c3ref/open.html
const OPEN_DATABASE_FILE_FLAGS = 6

function openMemoryDatabaseConnection (openCallback, errorCallback) {
  window.sqliteBatchConnection.openDatabaseConnection(
    { fullName: ':memory:', flags: 2 },
    openCallback,
    errorCallback
  )
}

function openFileDatabaseConnection (name, openCallback, errorCallback) {
  window.sqliteStorageFile.resolveAbsolutePath(
    {
      name: name,
      // TEMPORARY & DEPRECATED value, as needed for iOS & macOS ("osx"):
      location: 2
    },
    function (path) {
      log('database file path: ' + path)

      window.sqliteBatchConnection.openDatabaseConnection(
        { fullName: path, flags: OPEN_DATABASE_FILE_FLAGS },
        openCallback,
        errorCallback
      )
    }
  )
}

function openCacheFileDatabaseConnection (name, openCallback, errorCallback) {
  window.resolveLocalFileSystemURL(
    // portable across Android, iOS, & macOS ("osx"):
    cordova.file.cacheDirectory,
    function (entry) {
      const dataDirectoryUrl = entry.toURL()

      log('data directory url: ' + dataDirectoryUrl)

      // hacky, working solution:
      const path = dataDirectoryUrl.substring(7) + name

      log('database cache file path: ' + path)

      window.sqliteBatchConnection.openDatabaseConnection(
        { fullName: path, flags: OPEN_DATABASE_FILE_FLAGS },
        openCallback,
        errorCallback
      )
    }
  )
}

function onReady () {
  log('deviceready event received')
  startMemoryDatabaseDemo()
}

function startMemoryDatabaseDemo () {
  openMemoryDatabaseConnection(
    function (id) {
      log('memory database connection id: ' + id)

      window.sqliteBatchConnection.executeBatch(
        id,
        [['SELECT UPPER(?)', ['Text']]],
        function (results) {
          log(JSON.stringify(results))
          startFileDatabaseDemo()
        }
      )
    },
    function (error) {
      log('UNEXPECTED OPEN MEMORY DATABASE ERROR: ' + error)
    }
  )
}

function startFileDatabaseDemo () {
  openFileDatabaseConnection(
    DATABASE_FILE_NAME,
    openDatabaseFileCallback,
    function (e) {
      log('UNEXPECTED OPEN ERROR: ' + e)
    }
  )
}

function openDatabaseFileCallback (connectionId) {
  log('open connection id: ' + connectionId)

  // ERROR TEST - file name with incorrect flags:
  window.sqliteBatchConnection.openDatabaseConnection(
    { fullName: 'dummy.db', flags: 0 },
    function (_ignored) {
      log('FAILURE - unexpected open success callback received')
    },
    function (e) {
      log('OK - received error callback as expected for incorrect open call')

      // CONTINUE with batch demo, with the correct connectionId:
      batchDemo(connectionId)
    }
  )
}

function batchDemo (connectionId) {
  log('starting batch demo for connection id: ' + connectionId)
  window.sqliteBatchConnection.executeBatch(
    connectionId,
    [
      [
        'SELECT ?, -?, LOWER(?), UPPER(?)',
        [null, 1234567.890123, 'ABC', 'Text']
      ],
      ['SELECT -?', [1234567890123456]], // should fit into 52 bits (signed)
      ['SLCT 1', []],
      ['SELECT ?', ['OK', 'out of bounds parameter']],
      ['DROP TABLE IF EXISTS Testing', []],
      ['CREATE TABLE Testing (data NOT NULL)', []],
      ["INSERT INTO Testing VALUES ('test data')", []],
      ['INSERT INTO Testing VALUES (null)', []],
      ['DELETE FROM Testing', []],
      ["INSERT INTO Testing VALUES ('test data 2')", []],
      ["INSERT INTO Testing VALUES ('test data 3')", []],
      ['SELECT * FROM Testing', []],
      ["SELECT 'xyz'", []]
    ],
    batchCallback
  )
}

function batchCallback (batchResults) {
  // show batch results in JSON string format (on all platforms)
  log('received batch results')
  log(JSON.stringify(batchResults))

  startReaderDemo()
}

function startReaderDemo () {
  openFileDatabaseConnection(
    DATABASE_FILE_NAME,
    function (id) {
      log('read from another connection id: ' + id)

      window.sqliteBatchConnection.executeBatch(
        id,
        [['SELECT * FROM Testing', []]],
        function (res) {
          log(JSON.stringify(res))
          startCacheFileDemo()
        }
      )
    },
    function (error) {
      log('UNEXPECTED OPEN ERROR: ' + error)
    }
  )
}

function startCacheFileDemo () {
  openCacheFileDatabaseConnection(
    DATABASE_FILE_NAME,
    function (id) {
      log('cache file database connection id: ' + id)

      window.sqliteBatchConnection.executeBatch(
        id,
        [
          ['DROP TABLE IF EXISTS Testing', []],
          ['CREATE TABLE Testing (data NOT NULL)', []],
          ["INSERT INTO Testing VALUES ('test data')", []],
          ['SELECT * FROM Testing', []]
        ],
        function (results) {
          log(JSON.stringify(results))
        }
      )
    },
    function (error) {
      log('UNEXPECTED OPEN ERROR: ' + error)
    }
  )
}
```

#### expected Cordova batch results

results from memory database demo:

```json
[{ "status": 0, "columns": ["UPPER(?)"], "rows": [["TEXT"]] }]
```

first file resut set in JSON string format (reformatted by `prettier-standard`):

```json
[
  {
    "status": 0,
    "columns": ["?", "-?", "LOWER(?)", "UPPER(?)"],
    "rows": [[null, -1234567.890123, "abc", "TEXT"]]
  },
  { "status": 0, "columns": ["-?"], "rows": [[-1234567890123456]] },
  { "status": 1, "message": "near \"SLCT\": syntax error" },
  { "status": 1, "message": "column index out of range" },
  { "status": 0, "totalChanges": 0, "rowsAffected": 0, "lastInsertRowId": 0 },
  { "status": 0, "totalChanges": 0, "rowsAffected": 0, "lastInsertRowId": 0 },
  { "status": 0, "totalChanges": 1, "rowsAffected": 1, "lastInsertRowId": 1 },
  { "status": 1, "message": "NOT NULL constraint failed: Testing.data" },
  { "status": 0, "totalChanges": 2, "rowsAffected": 1, "lastInsertRowId": 1 },
  { "status": 0, "totalChanges": 3, "rowsAffected": 1, "lastInsertRowId": 1 },
  { "status": 0, "totalChanges": 4, "rowsAffected": 1, "lastInsertRowId": 2 },
  {
    "status": 0,
    "columns": ["data"],
    "rows": [["test data 2"], ["test data 3"]]
  },
  { "status": 0, "columns": ["'xyz'"], "rows": [["xyz"]] }
]
```

NOTE: `lastInsertRowId` is only relevant for a statement with non-zero `rowsAffected` in the result.

second file result set (in JSON string format, reformatted by `prettier-standard`):

```json
[
  {
    "status": 0,
    "columns": ["data"],
    "rows": [["test data 2"], ["test data 3"]]
  }
]
```
results from cache database file demo:


```json
[
  { "status": 0, "totalChanges": 0, "rowsAffected": 0, "lastInsertRowId": 0 },
  { "status": 0, "totalChanges": 0, "rowsAffected": 0, "lastInsertRowId": 0 },
  { "status": 0, "totalChanges": 1, "rowsAffected": 1, "lastInsertRowId": 1 },
  { "status": 0, "columns": ["data"], "rows": [["test data"]] }
]
```

## Testing

### C test

- `cd ctest`
- `make test`

### Java JNI test

intended for testing on macOS only:

- `cd sccglue`
- `make test`

### Running Cordova demo

prerequisites:

- *recommended:* use macOS to build and run on all Cordova platforms
- install Apache Cordova using npm (`npm i -g cordova`)
- install Android SDK and NDK
- *recommended:* install Xcode

how:

- `(cd sccglue && make ndkbuild)`
- `cd cordova-demo`
- `make prepare-app`
- recommended: do `cordova plugin ls` to check that the demo plugin was added
- `(cordova platform add osx && cordova run osx)` to build and run on Cordova "osx" (macOS) platform
- `(cordova platform add android && cordova run android)`
- `(cordova platform add ios && cordova run ios)`

## build notes

- Makefiles are designed to fetch and extract recent SQLite amalgamation as needed to build test programs, JAR library with NDK build, and Cordova demo.

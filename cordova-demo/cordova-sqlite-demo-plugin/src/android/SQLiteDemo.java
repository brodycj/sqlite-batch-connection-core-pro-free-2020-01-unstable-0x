package com.demo;

import io.sqlc.SCCoreGlue;

import org.apache.cordova.*;

import org.json.*;

public class SQLiteDemo extends CordovaPlugin {
  @Override
  public boolean execute(String method, JSONArray data, CallbackContext cbc) {
    switch(method) {
      case "openDatabaseConnection":
        openDatabaseConnection(data, cbc);
        break;
      case "executeBatch":
        executeBatch(data, cbc);
        break;
      default:
        return false;
    }
    return true;
  }

  static private void
  openDatabaseConnection(JSONArray args, CallbackContext cbc) {
    try {
      final JSONObject options = args.getJSONObject(0);

      final String fullName = options.getString("fullName");

      final int flags = options.getInt("flags");

      final int mydbc = SCCoreGlue.scc_open_connection(fullName, flags);

      if (mydbc < 0) {
        cbc.error("open error: " + -mydbc);
      } else {
        cbc.success(mydbc);
      }
    } catch(Exception e) {
      // NOT EXPECTED - internal error:
      cbc.error(e.toString());
    }
  }

  static private void executeBatch(JSONArray args, CallbackContext cbc) {
    try {
      final int mydbc = args.getInt(0);

      JSONArray data = args.getJSONArray(1);

      JSONArray results = new JSONArray();
      final int count = data.length();

      for (int i=0; i<count; ++i) {
        int previousTotalChanges = SCCoreGlue.scc_get_total_changes(mydbc);

        JSONArray entry = data.getJSONArray(i);

        String s = entry.getString(0);

        if (SCCoreGlue.scc_begin_statement(mydbc, s) != 0) {
          JSONObject result = new JSONObject();
          result.put("status", 1); // REPORT SQLite ERROR 1
          result.put("message", SCCoreGlue.scc_get_last_error_message(mydbc));
          results.put(result);
        } else {
          JSONArray bind = entry.getJSONArray(1);

          final int bindCount = bind.length();

          int bindResult = 0; // SQLite OK

          for (int j = 0; j < bindCount; ++j) {
            final Object o = bind.get(j);

            if (o instanceof Number) {
              bindResult =
                SCCoreGlue.scc_bind_double(mydbc, 1 + j, bind.optDouble(j));
            } else if (o instanceof String) {
              bindResult =
                SCCoreGlue.scc_bind_text(mydbc, 1 + j, o.toString());
            } else {
              bindResult =
                SCCoreGlue.scc_bind_null(mydbc, 1 + j);
            }
          }

          if (bindResult != 0) {
            JSONObject result = new JSONObject();
            result.put("status", 1); // REPORT SQLite ERROR 1
            result.put("message", SCCoreGlue.scc_get_last_error_message(mydbc));
            results.put(result);
            SCCoreGlue.scc_end_statement(mydbc);
            continue;
          }

          int stepResult = SCCoreGlue.scc_step(mydbc);

          if (stepResult == 100) {
            final int columnCount = SCCoreGlue.scc_get_column_count(mydbc);

            JSONArray columns = new JSONArray();

            for (int j=0; j < columnCount; ++j) {
              columns.put(SCCoreGlue.scc_get_column_name(mydbc, j));
            }

            JSONArray rows = new JSONArray();

            do {
              JSONArray row = new JSONArray();

              for (int col=0; col < columnCount; ++col) {
                final int type = SCCoreGlue.scc_get_column_type(mydbc, col);

                if (type == SCCoreGlue.SCC_COLUMN_TYPE_INTEGER ||
                    type == SCCoreGlue.SCC_COLUMN_TYPE_FLOAT) {
                  row.put(SCCoreGlue.scc_get_column_double(mydbc, col));
                } else if (type == SCCoreGlue.SCC_COLUMN_TYPE_NULL) {
                  row.put(JSONObject.NULL);
                } else {
                  row.put(SCCoreGlue.scc_get_column_text(mydbc, col));
                }
              }

              rows.put(row);

              stepResult = SCCoreGlue.scc_step(mydbc);
            } while (stepResult == 100);

            JSONObject result = new JSONObject();
            result.put("status", 0); // REPORT SQLite OK
            result.put("columns", columns);
            result.put("rows", rows);
            results.put(result);
            SCCoreGlue.scc_end_statement(mydbc);
          } else if (stepResult == 101) {
            int totalChanges = SCCoreGlue.scc_get_total_changes(mydbc);
            int rowsAffected = totalChanges - previousTotalChanges;

            JSONObject result = new JSONObject();
            // same order as iOS & macOS ("osx"):
            result.put("status", 0); // REPORT SQLite OK
            result.put("totalChanges", totalChanges);
            result.put("rowsAffected", rowsAffected);
            result.put("lastInsertRowId",
              SCCoreGlue.scc_get_last_insert_rowid(mydbc));
            results.put(result);
            SCCoreGlue.scc_end_statement(mydbc);
          } else {
            JSONObject result = new JSONObject();
            result.put("status", 1); // REPORT SQLite ERROR 1
            result.put("message", SCCoreGlue.scc_get_last_error_message(mydbc));
            results.put(result);
            SCCoreGlue.scc_end_statement(mydbc);
          }
        }
      }

      // send results to JavaScript side (...)
      cbc.success(results);
    } catch(Exception e) {
      // NOT EXPECTED - internal error:
      cbc.error(e.toString());
    }
  }

  static {
    System.loadLibrary("sqlite-connection-core-glue");
    SCCoreGlue.scc_init();
  }
}

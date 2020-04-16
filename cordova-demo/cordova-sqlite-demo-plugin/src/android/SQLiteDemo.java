package com.demo;

import io.sqlc.SCCoreGlue;

import org.apache.cordova.*;

import org.json.*;

public class SQLiteDemo extends CordovaPlugin {
  @Override
  public boolean execute(String a, JSONArray data, CallbackContext cbc) {
    switch(a) {
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

  static private void openDatabaseConnection(JSONArray args, CallbackContext cbc) {
    try {
      final String filename = args.getString(0);
      final int flags = args.getInt(1);

      final int mydbc = SCCoreGlue.scc_open_connection(filename, flags);

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
        JSONArray sa = data.getJSONArray(i);

        String s = sa.getString(0);

        if (SCCoreGlue.scc_begin_statement(mydbc, s) != 0) {
          JSONObject result = new JSONObject();
          result.put("status", 1); // REPORT SQLite
          result.put("message", SCCoreGlue.scc_get_last_error_message(mydbc));
          results.put(result);
        } else {
          JSONArray aa = sa.getJSONArray(1);

          final int aalength = aa.length();

          int br = 0; // SQLite OK

          for (int ai = 0; ai < aalength; ++ai) {
            final Object o = aa.get(ai);

            if (o instanceof Integer || o instanceof Long) {
              br = SCCoreGlue.scc_bind_long(mydbc, 1 + ai, aa.optLong(ai));
            } else if (o instanceof Number) {
              br = SCCoreGlue.scc_bind_double(mydbc, 1 + ai, aa.optDouble(ai));
            } else if (o instanceof String) {
              br = SCCoreGlue.scc_bind_text(mydbc, 1 + ai, o.toString());
            } else {
              br = SCCoreGlue.scc_bind_null(mydbc, 1 + ai);
            }
          }

          if (br != 0) {
            JSONObject result = new JSONObject();
            result.put("status", 1); // REPORT SQLite
            result.put("message", SCCoreGlue.scc_get_last_error_message(mydbc));
            results.put(result);
            SCCoreGlue.scc_end_statement(mydbc);
            continue;
          }

          final int sr = SCCoreGlue.scc_step(mydbc);

          if (sr == 100) {
            final int cc = SCCoreGlue.scc_get_column_count(mydbc);

            JSONArray columns = new JSONArray();

            for (int ci=0; ci < cc; ++ci) {
              columns.put(SCCoreGlue.scc_get_column_name(mydbc, ci));
            }

            JSONArray rows = new JSONArray();

            int sr2;

            do {
              JSONArray row = new JSONArray();

              for (int c=0; c < cc; ++c) {
                final int ct = SCCoreGlue.scc_get_column_type(mydbc, c);

                if (ct == SCCoreGlue.SCC_COLUMN_TYPE_INTEGER) {
                  row.put(SCCoreGlue.scc_get_column_long(mydbc, c));
                } else if (ct == SCCoreGlue.SCC_COLUMN_TYPE_FLOAT) {
                  row.put(SCCoreGlue.scc_get_column_double(mydbc, c));
                } else if (ct == SCCoreGlue.SCC_COLUMN_TYPE_NULL) {
                  row.put(JSONObject.NULL);
                } else {
                  row.put(SCCoreGlue.scc_get_column_text(mydbc, c));
                }
              }

              rows.put(row);

              sr2 = SCCoreGlue.scc_step(mydbc);
            } while (sr2 == 100);

            JSONObject result = new JSONObject();
            result.put("status", 0); // REPORT SQLite OK
            result.put("columns", columns);
            result.put("rows", rows);
            results.put(result);
            SCCoreGlue.scc_end_statement(mydbc);
          } else if (sr == 101) {
            JSONObject result = new JSONObject();
            result.put("status", 0); // REPORT SQLite OK
            result.put("total_changes", SCCoreGlue.scc_get_total_changes(mydbc));
            result.put("last_insert_rowid", SCCoreGlue.scc_get_last_insert_rowid(mydbc));
            results.put(result);
            SCCoreGlue.scc_end_statement(mydbc);
          } else {
            JSONObject result = new JSONObject();
            result.put("status", 1); // REPORT SQLite
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

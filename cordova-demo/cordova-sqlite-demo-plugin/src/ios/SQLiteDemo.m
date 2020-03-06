#import <Cordova/CDVPlugin.h>

#include "sqlite-connection-core.h"

@interface SQLiteDemo : CDVPlugin

- (void) openDatabaseConnection: (CDVInvokedUrlCommand *) commandInfo;

- (void) executeBatch: (CDVInvokedUrlCommand *) commandInfo;

@end

@implementation SQLiteDemo

- (void) pluginInitialize
{
  scc_init();
}

- (void) openDatabaseConnection: (CDVInvokedUrlCommand *) commandInfo
{
  NSArray * _args = commandInfo.arguments;

  const char * filename = [(NSString *)[_args objectAtIndex: 0] cString];

  const int flags = [(NSNumber *)[_args objectAtIndex: 1] intValue];

  const int connection_id = scc_open_connection(filename, flags);

  CDVPluginResult * testResult =
    [CDVPluginResult resultWithStatus: CDVCommandStatus_OK
			 messageAsInt: connection_id];

  [self.commandDelegate sendPluginResult: testResult
                              callbackId: commandInfo.callbackId];
}

- (void) executeBatch: (CDVInvokedUrlCommand *) commandInfo
{
  NSArray * _args = commandInfo.arguments;

  const int connection_id = [(NSNumber *)[_args objectAtIndex: 0] intValue];

  NSArray * data = [_args objectAtIndex: 1];

  NSMutableArray * ra = [NSMutableArray arrayWithCapacity: 0];

  for (int ii=0; ii < [data count]; ++ii) {
    NSArray * saa = [data objectAtIndex:ii];

    NSString * st = [saa objectAtIndex: 0];

    NSArray * aa = [saa objectAtIndex: 1];

    const int rc1 = scc_begin_statement(connection_id, [st cString]);

    int bindResult = 0;

    for (int i=0; i < [aa count]; ++i) {
      NSObject * bindValue = [aa objectAtIndex: i];

      if (bindValue == nil) {
        bindResult = scc_bind_null(connection_id, 1 + i);
      } else if ([bindValue isKindOfClass: [NSNumber class]]) {
        // TBD UIWebView vs WKWebView
        if ([(NSNumber *)bindValue objCType][0] == 'q') {
          bindResult = scc_bind_long(connection_id, 1 + i, [(NSNumber *)bindValue longValue]);
        } else {
          bindResult = scc_bind_double(connection_id, 1 + i, [(NSNumber *)bindValue doubleValue]);
        }
      } else if ([bindValue isKindOfClass: [NSString class]]) {
        bindResult = scc_bind_text(connection_id, 1 + i, [(NSString *)bindValue cString]);
      } else {
        bindResult = scc_bind_null(connection_id, 1 + i);
      }
    }

    if (rc1 != 0 || bindResult != 0) {
      [ra addObject: @{
        @"status": @1,
        @"message": [NSString stringWithUTF8String:
          scc_get_last_error_message(connection_id)]
      }];

      scc_end_statement(connection_id);

      continue;
    }

    int rc2 = scc_step(connection_id);

    if (rc2 == 100) {
      NSMutableArray * rra = [NSMutableArray arrayWithCapacity: 0];

      int nextrc;

      do {
        NSMutableDictionary * rro =
          [NSMutableDictionary dictionaryWithCapacity: 0];

        const int cc = scc_get_column_count(connection_id);

        for (int c = 0; c < cc; ++c) {
          NSString * columnNameAsString =
            [NSString stringWithUTF8String: scc_get_column_name(connection_id, c)];

          const int columnType = scc_get_column_type(connection_id, c);

          if (columnType == SCC_COLUMN_TYPE_NULL) {
            [rro setValue: [NSNull null] forKey: columnNameAsString];
          } else if (columnType == SCC_COLUMN_TYPE_INTEGER) {
            NSNumber * columnNumberValue =
              [NSNumber numberWithLongLong: scc_get_column_long(connection_id, c)];
            [rro setValue: columnNumberValue forKey: columnNameAsString];
          } else if (columnType == SCC_COLUMN_TYPE_FLOAT) {
            NSNumber * columnNumberValue =
              [NSNumber numberWithDouble: scc_get_column_double(connection_id, c)];
            [rro setValue: columnNumberValue forKey: columnNameAsString];
          } else {
            NSString * columnStringValue =
              [NSString stringWithUTF8String: scc_get_column_text(connection_id, c)];
            [rro setValue: columnStringValue forKey: columnNameAsString];
          }
        }

        [rra addObject: rro];

        nextrc = scc_step(connection_id);
      } while(nextrc == 100);

      [ra addObject: @{@"status":@0, @"rows": rra}];
    } else if (rc2 == 101) {
      [ra addObject: @{
        @"status": @0,
        @"total_changes": [NSNumber numberWithInteger:
          scc_get_total_changes(connection_id)],
        @"last_insert_rowid": [NSNumber numberWithInteger:
          scc_get_last_insert_rowid(connection_id)]
      }];
    } else {
      [ra addObject: @{
        @"status": @1,
        @"message": [NSString stringWithUTF8String:
          scc_get_last_error_message(connection_id)]
      }];
    }

    scc_end_statement(connection_id);
  }

  CDVPluginResult * testResult =
    [CDVPluginResult resultWithStatus: CDVCommandStatus_OK
                       messageAsArray: ra];

  [self.commandDelegate sendPluginResult: testResult
                              callbackId: commandInfo.callbackId];
}

@end

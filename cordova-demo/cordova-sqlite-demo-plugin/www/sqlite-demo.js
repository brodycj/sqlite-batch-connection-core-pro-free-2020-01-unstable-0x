function openDatabaseConnection (options, cb, errorCallback) {
  cordova.exec(cb, errorCallback, 'SQLiteDemo', 'openDatabaseConnection', [
    options
  ])
}

function executeBatch (connectionId, batchList, cb) {
  cordova.exec(cb, null, 'SQLiteDemo', 'executeBatch', [
    connectionId,
    batchList
  ])
}

window.openDatabaseConnection = openDatabaseConnection

window.executeBatch = executeBatch

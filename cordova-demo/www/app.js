document.addEventListener('deviceready', onReady)

function onReady () {
  window.openDatabaseConnection(':memory:', 2, openCallback)
}

function openCallback (connectionId) {
  console.log('got connectionId: ' + connectionId)
  window.executeBatch(
    connectionId,
    [
      ['SELECT ?, -?, LOWER(?), UPPER(?)', [null, 123.456789, 'ABC', 'Text']],
      ['SLCT 1', []],
      ['SELECT ?', ['OK', 'out of bounds parameter']],
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
  console.log('received batch results')
  console.log(JSON.stringify(batchResults))
  window.alert('received batch results: ' + JSON.stringify(batchResults))
}

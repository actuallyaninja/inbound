Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready!');
});

Pebble.addEventListener('showConfiguration', function() {
  var url = 'http://firstwebappcsharp.azurewebsites.net/';

  //console.log('Showing configuration page: ' + url);

  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));

  console.log('Configuration page returned: ' + JSON.stringify(configData));

    Pebble.sendAppMessage({
      slantDirection: configData.slantDirection
    }, function() {
      console.log('Send successful!');
    }, function() {
      console.log('Send failed!');
    });
  //}
});
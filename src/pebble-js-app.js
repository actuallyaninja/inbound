Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready!');
});

Pebble.addEventListener('showConfiguration', function() {
  //var url = 'http://firstwebappcsharp.azurewebsites.net/';
  var url = 'http://actuallyaninja.github.io/djangles-config/?platform=';
  
  if(Pebble.getActiveWatchInfo) {
    var myPlatform = Pebble.getActiveWatchInfo().platform; 
    url = url + myPlatform;
  }
  
  //console.log('Showing configuration page: ' + url);
  console.log('Showing JS configuration page.');

  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));

  console.log('Configuration page returned: ' + JSON.stringify(configData));

    Pebble.sendAppMessage({
      slantDirectionNumber: configData.slantDirectionNumber,
      //backgroundImageSelection: configData.backgroundImageSelection,
      bgPatternRadioButton: configData.bgPatternRadioButton
    }, function() {
      console.log('Send successful!');
    }, function() {
      console.log('Send failed!');
    });
  //}
});
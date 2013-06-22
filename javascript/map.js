function panBy(map, key, x, y) {
  var cap = (key >= 65 && key <= 90);
  var mult = 2;
  x = x * (cap ? mult : 1);
  y = y * (cap ? mult : 1);

  console.log("panBy(" + key + ", " + x + ", " + y + ")");
  console.log("  height: " + $(document).height());
  console.log("  width : " + $(document).width());

  var remX = remaining(x, $(document).width());
  var remY = remaining(y, $(document).height());
  x = x - remX;
  y = y - remY;
  
  console.log("  x     : " + x);
  console.log("  remX  : " + remX);
  console.log("  y     : " + y);
  console.log("  remY  : " + remY);

  panLater(map, remX, remY);
  map.panBy(x, y);
}

function panLater(map, x, y) {
  if(x != 0 || y != 0) {
    console.log("panLater(" + x + ", " + y + ")");
    var idle = google.maps.event.addListener(map, 'idle', function() {
      google.maps.event.removeListener(idle);
      panBy(map, 0, x, y);
    });
  }
}

function remaining(val, limit) {
  if(Math.abs(val) < limit) { return 0; }

  return (val < 0)
    ? val + limit - 1
    : val - limit + 1;
}

function initialize() {
  var mapOptions = {
    center: new google.maps.LatLng(-34.397, 150.644),
    zoom: 8,
    mapTypeId: google.maps.MapTypeId.ROADMAP
  };
  var map = new google.maps.Map(document.getElementById("map-canvas"), mapOptions);

  function logEvent(event) {
    google.maps.event.addListener(map, event, function() {
      console.log(event);
    });
  }

  logEvent('idle');

  $(window).keypress(function(e) {
    var dist = 800;

    /* w */ if(e.keyCode == 119 || e.keyCode == 87)      { panBy(map, e.keyCode, 0, -1 * dist); }
    /* a */ else if(e.keyCode == 97  || e.keyCode == 65) { panBy(map, e.keyCode, -1 * dist, 0); }
    /* s */ else if(e.keyCode == 115 || e.keyCode == 83) { panBy(map, e.keyCode, 0, dist); }
    /* d */ else if(e.keyCode == 100 || e.keyCode == 68) { panBy(map, e.keyCode, dist, 0); }
    /* q */ else if(e.keyCode == 113 || e.keyCode == 81) { panBy(map, e.keyCode, -1 * dist, -1 * dist); }
    

    /* z */ else if(e.keyCode == 122)                    { map.setZoom(map.getZoom() + 1); }
    /* x */ else if(e.keyCode == 120)                    { map.setZoom(map.getZoom() - 1); }
  });
}
google.maps.event.addDomListener(window, 'load', initialize);

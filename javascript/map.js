var DIRECTION_HISTORY_NEEDED = 2;
var MOVEMENT_LIMIT           = 1000;

var map;

function panBy(x, y) {
  if(x == 0 && y == 0)
    return;

  console.log("panBy(" + x + ", " + y + ")");
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
  if(Math.abs(val) < limit)
    return 0;

  return (val < 0)
    ? val + limit - 1
    : val - limit + 1;
}

var xDirectionHistory = [];
var yDirectionHistory = [];
var up    = 1;
var down  = 2;
var left  = 3;
var right = 4;

function dirToString(dir) {
  if(dir === null)
    return null;

  var s;

  if(dir.d == up)    s = "up";
  if(dir.d == down)  s = "down";
  if(dir.d == left)  s = "left";
  if(dir.d == right) s = "right";

  if(dir.confident)
    s += " (confident)";
  else
    s += " (unsure)";

  return s;
}

function historyToString(list) {
  var newList = [];
  for(var i=0; i<list.length; i++) {
    newList.push(dirToString(list[i]));
  }

  return newList.join(", ");
}

var lastFingerX = 0;
var lastFingerY = 0;
var lastHandX   = 0;
var lastHandY   = 0;

function lastNDirectionsAre(dir, list) {
  for(var i=list.length - 1; i>=0; i--) {
    if(list[i].d != dir)
      return false;
  }

  return true;
}

function pushDirection(dir, list) {
  if(dir === null)
    return;

  if(list.length > DIRECTION_HISTORY_NEEDED - 1)
    list.splice(0, 1);

  list.push(dir);
}

function currentXDirection(fingerCount, difference) {
  if(Math.abs(difference) > MOVEMENT_LIMIT || difference == 0)
    return null;

  if(difference > 0)
    return (lastNDirectionsAre(left, xDirectionHistory) ? {d : left, confident : true} : {d : left, confident : false});
  else
    return (lastNDirectionsAre(right, xDirectionHistory) ? {d : right, confident : true} : {d : right, confident : false});
}

function currentYDirection(fingerCount, difference) {
  if(Math.abs(difference) > MOVEMENT_LIMIT || difference == 0)
    return null;

  if(difference > 0)
    return (lastNDirectionsAre(down, yDirectionHistory) && fingerCount == 1 ? {d : down, confident : true} : {d : down, confident : false});
  else
    return (lastNDirectionsAre(up, yDirectionHistory) && fingerCount > 1 ? {d : up, confident : true} : {d : up, confident : false});
}

function logState(differenceX, differenceY, xDirection, yDirection) {
  console.log("  differenceX: " + differenceX);
  console.log("  differenceY: " + differenceY);
  console.log("  xDirection:  " + dirToString(xDirection));
  console.log("  yDirection:  " + dirToString(yDirection));

  console.log("  xDirectionHistory: " + historyToString(xDirectionHistory));
  console.log("  yDirectionHistory: " + historyToString(yDirectionHistory));
}

function updateMap(fingerCount, fingerX, fingerY, handX, handY) {
  console.log("updateMap(" + fingerCount + ", " + fingerX + ", " + fingerY + ", " + handX + ", " + handY + ")");

  var differenceX = fingerX - lastFingerX;
  var differenceY = fingerY - lastFingerY;
  var xDirection = currentXDirection(fingerCount, differenceX);
  var yDirection = currentYDirection(fingerCount, differenceY);

  logState(differenceX, differenceY, xDirection, yDirection);

  panBy(xDirection != null && xDirection.confident ? differenceX : 0, yDirection != null && yDirection.confident ? -differenceY : 0);

  pushDirection(xDirection, xDirectionHistory);
  pushDirection(yDirection, yDirectionHistory);

  lastFingerX = fingerX;
  lastFingerY = fingerY;
  lastHandX   = handX;
  lastHandY   = handY;
}

function initialize() {
  var sanfrancisco = new google.maps.LatLng(37.7577, -122.4376);
  var mapOptions = {
    center: sanfrancisco,
    zoom: 13,
    mapTypeId: google.maps.MapTypeId.ROADMAP
  };
  map = new google.maps.Map(document.getElementById("map-canvas"), mapOptions);

  function logEvent(event) {
    google.maps.event.addListener(map, event, function() {
      console.log(event);
    });
  }

  logEvent('idle');

  $(window).keypress(function(e) {
    var dist = 200;

    /* w */ if(e.keyCode == 119 || e.keyCode == 87)      { panBy(0, -1 * dist); }
    /* a */ else if(e.keyCode == 97  || e.keyCode == 65) { panBy(-1 * dist, 0); }
    /* s */ else if(e.keyCode == 115 || e.keyCode == 83) { panBy(0, dist); }
    /* d */ else if(e.keyCode == 100 || e.keyCode == 68) { panBy(dist, 0); }
    /* q */ else if(e.keyCode == 113 || e.keyCode == 81) { panBy(-1 * dist, -1 * dist); }


    /* z */ else if(e.keyCode == 122)                    { map.setZoom(map.getZoom() + 1); }
    /* x */ else if(e.keyCode == 120)                    { map.setZoom(map.getZoom() - 1); }

    /* M */ else if(e.keyCode == 77)                     { driveGestureEvents(); }
  });

  document.getElementById("pluginObj").addEventHandler("testEvent", updateMap);
}
//google.maps.event.addDomListener(window, 'load', initialize);
$(document).ready(initialize);

function driveGestureEvents() {
  console.log("starting gesture simulator");

  var pos = [
    {c : 1, fx : 0,   fy : 0},
    {c : 1, fx : 10,  fy : 10},
    {c : 1, fx : 30,  fy : 30},
    {c : 1, fx : 50,  fy : 50},
    {c : 1, fx : 70,  fy : 70},
    {c : 1, fx : 80,  fy : 80},
    {c : 1, fx : 100, fy : 100},
    {c : 1, fx : 120, fy : 120},
    {c : 1, fx : 300, fy : 300},
    {c : 1, fx : 500, fy : 500}
  ];

  // for(var i=0; i<pos.length; i++) {
  //   (function() {
  //     var el = pos[i];
  //     setTimeout(function() {
  //       updateMap(el.c, el.fx, el.fy, el.fx, el.fy);
  //     }, 1000);
  //   })();
  // }

  // updateMap(1, 0, 0);
  // updateMap(1, 40, 40);
  // updateMap(1, 200, 200);
  
  // updateMap(2, 150, 150);
  // updateMap(2, 100, 100);
  // updateMap(2, 50, 50);
  // updateMap(2, 20, 20);
  // updateMap(2, 20, 20);
  // updateMap(2, 20, 20);

  // updateMap(1, 40, 40);
  // updateMap(1, 80, 80);
  // updateMap(1, 100, 100);
  // updateMap(1, 200, 200);

  // return;

  var i=0;
  var fx = 0;
  var fy = 0;
  var fingercount = 1;
  var direction = 1;

  var func = function() {
    updateMap(fingercount, fx, fy, fx, fy);

    if(fx > 500) {
      direction = -1;
      fingercount = 2;
    }
    else if (fx < 0) {
      direction = 1;
      fingercount = 1;
    }

    fx = fx + (40 * direction);
    fy = fy + (40 * direction);

    var ms = 60;

    if(i++ < 10000 / ms)
      setTimeout(func, ms);
  }

  func();
}

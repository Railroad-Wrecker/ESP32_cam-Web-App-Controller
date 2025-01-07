window.onload = document.getElementById("streaming").src =
  "http://192.168.4.1:81/stream";
var button = document.querySelector(".button");

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener("load", onLoad);
// Initialize WebSocket
function onOpen(event) {
  console.log("Connection opened");
}
function onClose(event) {
  console.log("Connection closed");
  setTimeout(initWebSocket, 2000);
}
function initWebSocket() {
  console.log("Trying to open a WebSocket connection...");
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}

function onMessage(event) {
  console.log(`Received a notification from ${event.origin}`);
  console.log(event);
}

// Initialize buttons for control
function initButtons() {
  var buttons = document.querySelectorAll(".button");
  buttons.forEach((button) => {
    button.addEventListener("mousedown", function (event) {
      websocket.send(event.target.id);
    });
    button.addEventListener("mouseup", function (event) {
      websocket.send("stop");
    });
    button.addEventListener("touchstart", function (event) {
      websocket.send(event.target.id);
    });
    button.addEventListener("touchend", function (event) {
      websocket.send("stop");
    });
  });
}

function onLoad(event) {
  initWebSocket();
  initButtons();
}

// Function to send commands via WebSocket
function buttonpressed(x) {
  websocket.send(x);
}

// Function to set speed
function setSpeed() {
  const speedInput = document.getElementById("speed");
  speedValue = parseInt(speedInput.value);

  if (isNaN(speedValue) || speedValue < 0 || speedValue > 280) {
    alert("Please enter a valid speed between 0 and 280 RPM.");
    return;
  }

  websocket.send(`setSpeed:${speedValue}`);
  alert(`Speed set to ${speedValue} RPM`);
  console.log(`Speed set to ${speedValue} RPM`);
}

// Key mapping for controls
var keyActions = {
  w: function () {
    buttonpressed("forward");
  },
  a: function () {
    buttonpressed("left");
  },
  s: function () {
    buttonpressed("backward");
  },
  d: function () {
    buttonpressed("right");
  },
  q: function () {
    buttonpressed("rotateL");
  },
  e: function () {
    buttonpressed("rotateR");
  },
  f: function () {
    buttonpressed("stop");
  },
  1: function () {
    buttonpressed("path1");
  },
  2: function () {
    buttonpressed("path2");
  },
  3: function () {
    buttonpressed("path3");
  },
};

var keys = {};

document.addEventListener("keydown", function (event) {
  var key = event.key.toLowerCase();
  if (keyActions[key] && !keys[key]) {
    keys[key] = true; // Mark key as pressed
    handleMultipleKeyPress();
    console.log("Key pressed:", key);
  }
});

document.addEventListener("keyup", function (event) {
  var key = event.key.toLowerCase();
  if (keys[key]) {
    keys[key] = false; // Mark key as released
    websocket.send("stop");
  }
});

function handleMultipleKeyPress() {
  if (keys["w"] && keys["d"]) {
    buttonpressed("diag_for_right");
  } else if (keys["w"] && keys["a"]) {
    buttonpressed("diag_for_left");
  } else if (keys["s"] && keys["a"]) {
    buttonpressed("diag_back_left");
  } else if (keys["s"] && keys["d"]) {
    buttonpressed("diag_back_right");
  } else if (keys["w"]) {
    buttonpressed("forward");
  } else if (keys["a"]) {
    buttonpressed("left");
  } else if (keys["s"]) {
    buttonpressed("backward");
  } else if (keys["d"]) {
    buttonpressed("right");
  } else if (keys["q"]) {
    buttonpressed("rotateL");
  } else if (keys["e"]) {
    buttonpressed("rotateR");
  } else if (keys["f"]) {
    buttonpressed("stop");
  } else if (keys["1"]) {
    buttonpressed("path1");
  } else if (keys["2"]) {
    buttonpressed("path2");
  } else if (keys["3"]) {
    buttonpressed("path3");
  }
}
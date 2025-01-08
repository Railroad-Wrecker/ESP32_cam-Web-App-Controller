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
    keyActions[key](); // Trigger the action
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

document.getElementById("path1Button").addEventListener("click", function () {
  buttonpressed("path1");
});

document.getElementById("path2Button").addEventListener("click", function () {
  buttonpressed("path2");
});

document.getElementById("path3Button").addEventListener("click", function () {
  buttonpressed("path3");
});

var actionStatus = {
  forward: false,
  backward: false,
  left: false,
  right: false,
  diag_for_right: false,
  diag_for_left: false,
  diag_back_right: false,
  diag_back_left: false,
  rotateL: false,
  rotateR: false,
  stop: true,
  path1: false,
  path2: false,
  path3: false
};

function isActionFinished(action) {
  // Check if the action is finished
  return actionStatus[action];
}

websocket.onmessage = function(event) {
  var message = event.data;
  var action = message.replace("_finished", "");
  if (actionStatus.hasOwnProperty(action)) {
    actionStatus[action] = true;
  }
};

function handleMultipleKeyPress() {
  const actions = [
    { keys: ["w", "d"], action: "diag_for_right" },
    { keys: ["w", "a"], action: "diag_for_left" },
    { keys: ["s", "a"], action: "diag_back_left" },
    { keys: ["s", "d"], action: "diag_back_right" },
    { keys: ["w"], action: "forward" },
    { keys: ["a"], action: "left" },
    { keys: ["s"], action: "backward" },
    { keys: ["d"], action: "right" },
    { keys: ["q"], action: "rotateL" },
    { keys: ["e"], action: "rotateR" },
    { keys: ["f"], action: "stop" },
    { keys: ["1"], action: "path1" },
    { keys: ["2"], action: "path2" },
    { keys: ["3"], action: "path3" }  
  ];

  for (const { keys: actionKeys, action } of actions) {
    if (actionKeys.every(key => keys[key])) {
      buttonpressed(action, isActionFinished);
      break;
    }
  }
}

/*
    p5.js MQTT Client example
    This example uses p5.js: https://p5js.org/
    and the Eclipse Paho MQTT client library: https://www.eclipse.org/paho/clients/js/
    to create an MQTT client that sends and receives MQTT messages.
    The client is set up for use on the shiftr.io test MQTT broker (https://shiftr.io/try),
    but has also been tested on https://test.mosquitto.org
    created 12 June 2020
    modified 20 Aug 2020
    by Tom Igoe
*/

// MQTT client details:
let broker = {

  hostname: 'test-orbitals.cloud.shiftr.io',
  port: 443
};
// MQTT client:
let client;
// client credentials:
let creds = {
  clientID: 'p5Client', // choose whatever name you want
  userName: 'test-orbitals', // unique Key from token
  password: 'wykBmo6ZdeO4WZ4p' // unique Secret from token
}
// topics to subscribe to when you connect:
let topic_item = 'Item';
let topic_state = 'State';

// a pushbutton to send messages
let sendButton;
let localDiv;
let remoteDiv;
let comDiv;
let inpt;

let leds = [];

function setup() {
  createCanvas(windowWidth, windowHeight);

  for (let i = 0; i < 10; i++) {
    leds[i] = new Led(40, i * 50 + 150, color(40, 180, 20), false);
  }
  // Create an MQTT client:
  client = new Paho.MQTT.Client(broker.hostname, Number(broker.port), creds.clientID);
  // set callback handlers for the client:
  client.onConnectionLost = onConnectionLost;
  client.onMessageArrived = onMessageArrived;
  // connect to the MQTT broker:
  client.connect({
    onSuccess: onConnect, // callback function for when you connect
    userName: creds.userName, // username
    password: creds.password, // password
    useSSL: true // use SSL
  });

  // create a div for local messages:
  localDiv = createDiv('local messages will go here');
  localDiv.position(20, 50);
  localDiv.style('color', '#fff');
  // create a div for the response:
  remoteDiv = createDiv('waiting for messages');
  remoteDiv.position(20, 80);
  remoteDiv.style('color', '#fff');
  // create a div for communication:
  comDiv = createDiv('communication');
  comDiv.position(20, 110);
  comDiv.style('color', '#fff');


  for (let i = 0; i < leds.length; i++) {
      leds[i].name = "Item" + (i + 1);
    }
}

function draw() {
  background(50);
  for (let i = 0; i < leds.length; i++) {
    leds[i].show();
    leds[i].detect();
  }
}

function mousePressed() {
  for (let i = 0; i < leds.length; i++) {
    if(leds[i].detect()){
      leds[i].sendMsg();
    }
  }
}

// called when the client connects
function onConnect() {
  localDiv.html('client is connected');
  client.subscribe(topic_item);
  console.log("connected")
}

// called when the client loses its connection
function onConnectionLost(response) {
  if (response.errorCode !== 0) {
    localDiv.html('onConnectionLost:' + response.errorMessage);
  }
}

// called when a message arrives
function onMessageArrived(message) {
  remoteDiv.html('I got a message:' + message.payloadString);
  let incoming = split(trim(message.payloadString), ",");
  console.log(incoming);

  for (let i = 0; i < leds.length; i++) {
    leds[i].receiveMsg(incoming[i]);
  }
}


class Drawer {
  constructor(x, y, fill) {
    this.x = x;
    this.y = y;
    this.infill = fill;
    this.size = 30;
    this.onOff = false;
    this.number = "";
    this.openDoor = false;
  }
}

class Motor {
  constructor(x, y, fill) {
    this.x = x;
    this.y = y;
    this.infill = fill;
    this.size = 30;
    this.onOff = false;
  }
}

class Button {
  constructor(x, y, fill) {
    this.name = "";
    this.x = x;
    this.y = y;
    this.infill = fill;
    this.size = 30;
    this.pressed = false;
  }
}

class Error {
  constructor(x, y, fill) {
    this.x = x;
    this.y = y;
    this.infill = fill;
    this.size = 30;
    this.onOff = false;
    this.message = "";
  }
}

class Light {
  constructor(x, y, fill) {
    this.x = x;
    this.y = y;
    this.infill = fill;
    this.size = 30;
    this.onOff = false;
  }
}

class State {
  constructor(x, y, fill) {
    this.x = x;
    this.y = y;
    this.infill = fill;
    this.size = 30;
    this.state = "";
  }
}

class Led {
  constructor(x, y, fil, onOff) {
    this.x = x;
    this.y = y;
    this.infill = fil;
    this.size = 30;
    this.onOff = onOff;
    this.name = "";
  }
  sendMsg() {
    if (client.isConnected()) {
      this.onOff = !this.onOff;

      let msg = "";
      for(let i = 0; i < leds.length; i++){
        msg = msg + String(!leds[i].onOff) + ",";
      }
      // start an MQTT message:
      let message;
      message = new Paho.MQTT.Message(msg);
      // choose the destination topic:
      message.destinationName = topic_item;
      // send it:
      client.send(message);
      // print what you sent:
      localDiv.html('I sent: ' + message.payloadString);
    }

  }

  show() {
    if (this.onOff === "true") {
      fill(this.infill);
    } else if (this.onOff === "false") {
      fill(120);
    }
    else{
      fill(200, 50, 20);
    } 
    circle(this.x, this.y, this.size);
  }

  detect() {
    if (dist(mouseX, mouseY, this.x, this.y) <= this.size) {
      //fill(this.inFill);
      return true;

    } else {
    //  fill(120);
      return false;
    }
  }

  receiveMsg(onOff) {
    this.onOff = onOff;
  }
}
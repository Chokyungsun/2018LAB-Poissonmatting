var gravity = 10;
var mass =30;
var positionY = 100;
var velocityY = 0;
var timeStep = 0.03;
function setup() {
  createCanvas(windowWidth,windowHeight);
}

function draw() {
  //background
  background(255);
  drawShape();
}

function drawShape(){
  var forceY = mass * gravity;
  var accY = forceY/mass;
  velocityY = velocityY + accY*timeStep;
  positionY = positionY + velocityY *timeStep;
  background(255);
  ellipse(200, positionY, 20,20);
}


/*
Plot in real-time the gamma radiation level being logged to the serial
port. Because it's way more fun than reading a terminal.

Configure the serial port to which the Arduino is connected. By default,
we select the first port, but that may not be the good one.

by Yoan Tournade <yoan@ytotech.com>
*/

import processing.serial.*;

Serial port;
final int selectedPort = 0;
float lastY = 0.0;
float lastX = 0.0;
float x = 0.0;
float y = 0.0;
int startWindowX = 0;
int endWindowX = 500;

void setup () {
  size(600, 400);
  /* List all the available serial ports.
   * Check the listed serial ports in your machine
   * and use the correct index number for the selectedPort variable. */
  for(String portName: Serial.list())
    println(portName);
  // Initialize the serial port.
  port = new Serial(this, Serial.list()[selectedPort], 9600);
  // A onNewLine() will be generated when a newline character is received.
  port.bufferUntil('\n');
  // Set inital background.
  background(0);
  noLoop();
}

void serialEvent(Serial p) {
  onNewLine(p.readString());
  redraw();
}

void onNewLine(final String line) {
  print(line);
  // Parse the incoming CSV-formatted string.
  String[] parts = line.split(",");
  // We skip the first line containing the keys.
  if(parts[0].equals("time(ms)"))
    return;
  long time = Long.parseLong(parts[0]);
  float uSvh = Float.parseFloat(parts[3]);
  float uSvhError = Float.parseFloat(parts[4]);
  /* Keeps all the values and redraw all the graph for each new value?
   * Actually this is too complicated with Processing: easier and better use
   * the Python script with matplotlib. */
  // Map to the screen width and height.
  x = map(time / 1000, startWindowX, endWindowX, 0, width);
  y = map(uSvh, 0, 1, 0, height);
  // At the edge of the window, clear the screen and go back to the beginning.
  if (x >= width) {
    x = 0;
    lastX = 0;
    startWindowX = endWindowX;
    endWindowX = endWindowX + 500;
    background(0);
  }
}

void draw () {
  // Let's draw!
  stroke(127,34,255);
  line(lastX, lastY, x, height - y);
  lastY = height - y;
  lastX = x;
}

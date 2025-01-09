/*
// D3.js setup
const width = 800, height = 600;
const velocityNumbers = [];
// WebSocket setup
const ws2 = new WebSocket('ws://192.168.137.77/ws'); // Replace with your ESP32 IP address
ws2.onmessage = function(event) {
    console.log("Data received from ESP32:", event.data); // Log received data
    const velocityNumbers = JSON.parse(event.data);
}


const svg = d3.select("#trajectory_chart").append("svg")
    .attr("width", width)
    .attr("height", height);

const path = svg.append("path")
    .attr("fill", "none")
    .attr("stroke", "steelblue")
    .attr("stroke-width", 2);

let data = [];
let x = 400, y = 300, phi = 0; // Initial position
let xMin = x, xMax = x, yMin = y, yMax = y; // Track bounds

// Function to update bounds and adjust viewBox
function updateViewBox() {
    // Expand bounds slightly for padding
    const padding = 50;
    xMin -= padding; xMax += padding;
    yMin -= padding; yMax += padding;

    const viewBoxWidth = xMax - xMin;
    const viewBoxHeight = yMax - yMin;

    // Update SVG viewBox
    svg.attr("viewBox", `${xMin} ${yMin} ${viewBoxWidth} ${viewBoxHeight}`);
}

// Function to generate random velocities and integrate them to update positions
function updatePosition() {
    const deltaTime = 1; // Time step in seconds

    // AGV inverse kinematics
    //const v1 = 100, v2 = -50, v3 = 100, v4 = -50;
    const x_dot = 0.25 * (velocityNumbers[1] + velocityNumbers[2] + velocityNumbers[3] + velocityNumbers[4]); // x_dot
    const y_dot = 0.25 * (velocityNumbers[2] + velocityNumbers[4] - velocityNumbers[1] - velocityNumbers[3]); // y_dot

    x += x_dot * deltaTime;
    y += y_dot * deltaTime;

    // Update data array
    data.push({x: x, y: y});
    if (data.length > 100) data.shift(); // Keep last 100 points

    // Update bounds
    xMin = Math.min(xMin, x);
    xMax = Math.max(xMax, x);
    yMin = Math.min(yMin, y);
    yMax = Math.max(yMax, y);

    // Update path
    path.attr("d", d3.line()
    .x(d => d.x)
    .y(d => d.y)(data));

    // Update the viewBox to zoom out as needed
    updateViewBox();
}

// Function to simulate receiving data and update the plot
function simulateAGV() {
    updatePosition();
    setTimeout(simulateAGV, 1000); // Update every second
}

// Start the simulation
simulateAGV();
*/
/*
// D3.js setup
const width = 800, height = 600;
const svg = d3.select("#trajectory_chart").append("svg")
    .attr("width", width)
    .attr("height", height);

// Path to represent the trail
const trail = svg.append("path")
    .attr("fill", "none")
    .attr("stroke", "steelblue")
    .attr("stroke-width", 2);

// Polygon to represent the car as a triangle
const triangleSize = 20; // Size of the triangle
const car = svg.append("polygon")
    .attr("points", `0,${-triangleSize} ${triangleSize / 2},${triangleSize} ${-triangleSize / 2},${triangleSize}`)
    .attr("fill", "blue")
    .attr("stroke", "black")
    .attr("stroke-width", 1);

// Initial position and data
let data = [];
let x = 400, y = 300, phi = 0; // Initial position and orientation
let xMin = x, xMax = x, yMin = y, yMax = y; // Track bounds

// Function to update bounds and adjust viewBox
function updateViewBox() {
    const padding = 50;
    xMin = Math.min(xMin, x - padding);
    xMax = Math.max(xMax, x + padding);
    yMin = Math.min(yMin, y - padding);
    yMax = Math.max(yMax, y + padding);

    const viewBoxWidth = xMax - xMin;
    const viewBoxHeight = yMax - yMin;

    svg.attr("viewBox", `${xMin} ${yMin} ${viewBoxWidth} ${viewBoxHeight}`);
}

// Function to generate random velocities and integrate them to update positions
function updatePosition() {
    const deltaTime = 1; // Time step in seconds

    // AGV inverse kinematics
    const v1 = 100, v2 = -50, v3 = 100, v4 = -50;
    const x_dot = 0.25 * (v1 + v2 + v3 + v4); // x_dot
    const y_dot = 0.25 * (v2 + v4 - v1 - v3); // y_dot

    x += x_dot * deltaTime;
    y += y_dot * deltaTime;

    // Update data array
    data.push({x: x, y: y});
    if (data.length > 100) data.shift(); // Keep last 100 points

    // Update bounds
    updateViewBox();
}

// Function to update the trail and car position
function draw() {
    // Update the trail
    trail.attr("d", d3.line()
        .x(d => d.x)
        .y(d => d.y)(data));

    // Update car position and orientation
    const lastPoint = data[data.length - 1];
    car.attr("transform", `translate(${lastPoint.x}, ${lastPoint.y}) rotate(${phi})`);
}

// Function to simulate receiving data and update the plot
function simulateAGV() {
    updatePosition();
    draw();
    setTimeout(simulateAGV, 1000); // Update every second
}

// Start the simulation
simulateAGV();
*/


// Use this code for square
// D3.js setup for visualization
const width = 800, height = 600;
const svg = d3.select("#trajectory_chart").append("svg")
    .attr("width", "100%")
    .attr("height", "100%")
    .attr("viewBox", `0 0 ${width} ${height}`)
    .style("background", "#F2F0EF"); // Dark grey background for road

// Lane markers
const laneMarkers = svg.append("g")
    .attr("stroke", "white")
    .attr("stroke-width", 2)
    .attr("stroke-dasharray", "10, 10")
    .attr("opacity", 0.5);

// Create dashed lines for lanes
// for (let i = 200; i <= 600; i += 200) {
//     laneMarkers.append("line")
//         .attr("x1", 0)
//         .attr("y1", i)
//         .attr("x2", width)
//         .attr("y2", i);
// }

// Define a group for all visual elements
const g = svg.append("g");

// Trail with fading effect
const trail = g.append("path")
    .attr("fill", "none")
    .attr("stroke", "steelblue")
    .attr("stroke-width", 3)
    .attr("stroke-linejoin", "round")
    .attr("stroke-linecap", "round")
    .attr("opacity", 0.7);

// Car (square with shadow)
const carSize = 20;
const car = g.append("rect")
    .attr("width", carSize)
    .attr("height", carSize)
    .attr("fill", "#008000")
    //.attr("stroke", "#bf360c")
    .attr("stroke-width", 2)
    .attr("rx", 3) // Rounded corners for aesthetics
    .style("filter", "drop-shadow(2px 4px 6px rgba(0, 0, 0, 0.4))");

// Initial position and data
let data = [];
let x = 400, y = 300, phi = 0;
let currentSide = 0, distanceTraveled = 0;

// Define square path parameters
const sideLength = 200; // Length of each side of the square
const velocities = [];  // Array to store velocity sets
const speed = 100;      // Speed for linear motion

// Function to generate velocities for square path
function generateSquarePath() {
    velocities.push({ v1: speed, v2: speed, v3: speed, v4: speed });   // Right
    velocities.push({ v1: -speed, v2: speed, v3: -speed, v4: speed }); // Down
    velocities.push({ v1: -speed, v2: -speed, v3: -speed, v4: -speed }); // Left
    velocities.push({ v1: speed, v2: -speed, v3: speed, v4: -speed });  // Up
}
generateSquarePath();

// Function to update position
function updatePosition() {
    const deltaTime = 0.1; // Time step in seconds
    const velocity = velocities[currentSide];

    const x_dot = 0.25 * (velocity.v1 + velocity.v2 + velocity.v3 + velocity.v4);
    const y_dot = 0.25 * (velocity.v2 + velocity.v4 - velocity.v1 - velocity.v3);

    x += x_dot * deltaTime;
    y += y_dot * deltaTime;
    distanceTraveled += Math.sqrt(x_dot ** 2 + y_dot ** 2) * deltaTime;

    data.push({ x: x, y: y });
    if (data.length > 100) data.shift();

    if (distanceTraveled >= sideLength) {
        currentSide = (currentSide + 1) % velocities.length;
        distanceTraveled = 0;
    }
}

// Function to draw the trail and car
function draw() {
    trail.attr("d", d3.line().x(d => d.x).y(d => d.y)(data));
    car.attr("transform", `translate(${x - carSize / 2}, ${y - carSize / 2}) rotate(${phi}, ${carSize / 2}, ${carSize / 2})`);
}

// Simulate square path
function simulateSquarePath() {
    updatePosition();
    draw();
    setTimeout(simulateSquarePath, 100);
}

// Start simulation
simulateSquarePath();

// Function to create a chart
function createChart(ctx, label) {
  return new Chart(ctx, {
    type: 'line',
    data: {
      labels: [], // Time labels
      datasets: [{
        label: label,
        data: [],
        borderColor: 'rgba(75, 192, 192, 1)',
        borderWidth: 1,
        fill: false
      }]
    },
    options: {
      responsive: false,
      scales: {
        x: {
          type: 'time',
          time: {
            unit: 'second'
          },
          adapters: {
            date: {
              library: 'luxon',
              format: 'iso'
            }
          }
        },
        y: {
          beginAtZero: false,
          min:-280,
          max: 280
        }
      }
    }
  });
}

  // Initialize charts
  const chart1 = createChart(document.getElementById('chart1').getContext('2d'), 'Velocity Wheel 1 (Bottom Left)');
  const chart2 = createChart(document.getElementById('chart2').getContext('2d'), 'Velocity Wheel 2 (Top Left)');
  const chart3 = createChart(document.getElementById('chart3').getContext('2d'), 'Velocity Wheel 3 (Bottom Right)');
  const chart4 = createChart(document.getElementById('chart4').getContext('2d'), 'Velocity Wheel 4 (Top Right)');
  
  // WebSocket setup
  const ws = new WebSocket('ws://192.168.137.79/ws'); // Replace with your ESP32 IP address
  
  ws.onopen = function() {
    console.log("WebSocket connection established");
  };
  
  ws.onmessage = function(event) {
    console.log("Data received from ESP32:", event.data); // Log received data
    const velocityNumbers = JSON.parse(event.data);
    // console.log(velocityNumbers);
    const now = new Date();
  
    // Function to update chart data
    function updateChart(chart, randomNumber) {
      chart.data.labels.push(now);
      chart.data.datasets[0].data.push(randomNumber);
  
      // Keep only the last 20 data points
      if (chart.data.labels.length > 20) {
        chart.data.labels.shift();
        chart.data.datasets[0].data.shift();
      }
  
      chart.update();
    }
  
    // Update each chart with the same random number
    updateChart(chart1, velocityNumbers[1]);
    updateChart(chart2, velocityNumbers[2]);
    updateChart(chart3, velocityNumbers[3]);
    updateChart(chart4, velocityNumbers[4]);
  };
  
  ws.onerror = function(error) {
    console.error("WebSocket error:", error);
  };
  
  ws.onclose = function() {
    console.log("WebSocket connection closed");

    
  };
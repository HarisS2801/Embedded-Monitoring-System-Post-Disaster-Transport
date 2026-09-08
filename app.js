const ESP32_IP = "10.81.92.237";

const ESP32_API = `http://${ESP32_IP}/data`;
const ESP32_STATUS = `http://${ESP32_IP}/status`;

const temperatureValue = document.getElementById("temperatureValue");
const humidityValue = document.getElementById("humidityValue");
const sensor1Value = document.getElementById("sensor1Value");
const sensor2Value = document.getElementById("sensor2Value");
const warningText = document.getElementById("warningText");
const warningPanel = document.getElementById("warningPanel");
const warningBadge = document.getElementById("warningBadge");
const historyTableBody = document.getElementById("historyTableBody");
const historyMobileList = document.getElementById("historyMobileList");
const deviceConnection = document.getElementById("deviceConnection");
const deviceApi = document.getElementById("deviceApi");
const lastUpdate = document.getElementById("lastUpdate");
const heroStatus = document.getElementById("heroStatus");
const heroSubtext = document.getElementById("heroSubtext");
const refreshBtn = document.getElementById("refreshBtn");
const themeToggleBtn = document.getElementById("themeToggleBtn");
const mobileMenuBtn = document.getElementById("mobileMenuBtn");
const sidebar = document.getElementById("sidebar");

const tempCard = document.getElementById("tempCard");
const humidityCard = document.getElementById("humidityCard");
const sensor1Card = document.getElementById("sensor1Card");
const sensor2Card = document.getElementById("sensor2Card");

const tempAlertText = document.getElementById("tempAlertText");
const humidityAlertText = document.getElementById("humidityAlertText");
const sensor1AlertText = document.getElementById("sensor1AlertText");
const sensor2AlertText = document.getElementById("sensor2AlertText");

const labels = [];
const temperatureData = [];
const humidityData = [];
const sensor1Data = [];
const sensor2Data = [];
const maxPoints = window.innerWidth <= 640 ? 8 : 12;
const historyRows = [];

function getTheme() {
  return document.documentElement.getAttribute("data-theme") || "dark";
}

function applySavedTheme() {
  const savedTheme = localStorage.getItem("dashboard-theme");
  if (savedTheme === "light" || savedTheme === "dark") {
    document.documentElement.setAttribute("data-theme", savedTheme);
  }
}

function hexToRgba(hex, alpha) {
  const cleaned = hex.replace("#", "");
  const bigint = parseInt(cleaned, 16);
  const r = (bigint >> 16) & 255;
  const g = (bigint >> 8) & 255;
  const b = bigint & 255;
  return `rgba(${r}, ${g}, ${b}, ${alpha})`;
}

function getThemePalette() {
  const theme = getTheme();

  if (theme === "light") {
    return {
      text: "#0f172a",
      muted: "#64748b",
      grid: "rgba(15, 23, 42, 0.10)",
      temp: "#f97316",
      hum: "#06b6d4",
      s1: "#8b5cf6",
      s2: "#ef476f"
    };
  }

  return {
    text: "#eef4ff",
    muted: "#94a3b8",
    grid: "rgba(255,255,255,0.06)",
    temp: "#38bdf8",
    hum: "#22d3ee",
    s1: "#60a5fa",
    s2: "#fb7185"
  };
}

function createGradient(ctx, color) {
  const gradient = ctx.createLinearGradient(0, 0, 0, 260);
  gradient.addColorStop(0, hexToRgba(color, 0.35));
  gradient.addColorStop(1, hexToRgba(color, 0.02));
  return gradient;
}

function makeChart(canvasId, label, dataArray, lineColor) {
  const ctx = document.getElementById(canvasId).getContext("2d");
  const palette = getThemePalette();

  return new Chart(ctx, {
    type: "line",
    data: {
      labels,
      datasets: [{
        label,
        data: dataArray,
        borderColor: lineColor,
        backgroundColor: createGradient(ctx, lineColor),
        borderWidth: 2.5,
        tension: 0.38,
        fill: true,
        pointRadius: window.innerWidth <= 640 ? 2 : 3,
        pointHoverRadius: window.innerWidth <= 640 ? 4 : 5,
        pointBackgroundColor: lineColor,
        pointBorderColor: lineColor
      }]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      animation: {
        duration: 900,
        easing: "easeOutQuart"
      },
      interaction: {
        mode: "index",
        intersect: false
      },
      plugins: {
        legend: {
          labels: {
            color: palette.text
          }
        }
      },
      scales: {
        x: {
          ticks: {
            color: palette.muted,
            maxRotation: 0,
            autoSkip: true,
            maxTicksLimit: window.innerWidth <= 640 ? 4 : 8
          },
          grid: {
            color: palette.grid
          }
        },
        y: {
          ticks: {
            color: palette.muted
          },
          grid: {
            color: palette.grid
          }
        }
      }
    }
  });
}

function createCharts() {
  const palette = getThemePalette();

  const temperatureChart = makeChart("temperatureChart", "Temperature (°C)", temperatureData, palette.temp);
  const humidityChart = makeChart("humidityChart", "Humidity (%)", humidityData, palette.hum);

  const waterCtx = document.getElementById("waterChart").getContext("2d");

  const waterChart = new Chart(waterCtx, {
    type: "line",
    data: {
      labels,
      datasets: [
        {
          label: "Sensor 1",
          data: sensor1Data,
          borderColor: palette.s1,
          backgroundColor: createGradient(waterCtx, palette.s1),
          borderWidth: 2.5,
          tension: 0.35,
          fill: true,
          pointRadius: window.innerWidth <= 640 ? 2 : 3,
          pointHoverRadius: window.innerWidth <= 640 ? 4 : 5,
          pointBackgroundColor: palette.s1,
          pointBorderColor: palette.s1
        },
        {
          label: "Sensor 2",
          data: sensor2Data,
          borderColor: palette.s2,
          backgroundColor: createGradient(waterCtx, palette.s2),
          borderWidth: 2.5,
          tension: 0.35,
          fill: true,
          pointRadius: window.innerWidth <= 640 ? 2 : 3,
          pointHoverRadius: window.innerWidth <= 640 ? 4 : 5,
          pointBackgroundColor: palette.s2,
          pointBorderColor: palette.s2
        }
      ]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      animation: {
        duration: 900,
        easing: "easeOutQuart"
      },
      interaction: {
        mode: "index",
        intersect: false
      },
      plugins: {
        legend: {
          labels: {
            color: palette.text
          }
        }
      },
      scales: {
        x: {
          ticks: {
            color: palette.muted,
            maxRotation: 0,
            autoSkip: true,
            maxTicksLimit: window.innerWidth <= 640 ? 4 : 8
          },
          grid: {
            color: palette.grid
          }
        },
        y: {
          ticks: {
            color: palette.muted
          },
          grid: {
            color: palette.grid
          }
        }
      }
    }
  });

  return { temperatureChart, humidityChart, waterChart };
}

let charts = null;

function destroyChartsIfNeeded() {
  if (!charts) return;
  charts.temperatureChart.destroy();
  charts.humidityChart.destroy();
  charts.waterChart.destroy();
}

function rebuildCharts() {
  destroyChartsIfNeeded();
  charts = createCharts();
}

function pushChartData(timeLabel, temp, hum, s1, s2) {
  labels.push(timeLabel);
  temperatureData.push(temp);
  humidityData.push(hum);
  sensor1Data.push(s1);
  sensor2Data.push(s2);

  if (labels.length > maxPoints) {
    labels.shift();
    temperatureData.shift();
    humidityData.shift();
    sensor1Data.shift();
    sensor2Data.shift();
  }

  charts.temperatureChart.update();
  charts.humidityChart.update();
  charts.waterChart.update();
}

function updateWarningUI(temp, hum, s1, s2) {

  let isAlert = false;
  let messages = [];

  // Temperature check
  if (temp > 25) {
    isAlert = true;
    messages.push("Temperature above 25°C");
  }

  // Humidity check
  if (hum < 60 || hum > 70) {
    isAlert = true;
    messages.push("Humidity out of 60–70%");
  }

  // Water sensor check (your existing logic)
  if (s1 > 384 && s2 > 384) {
    isAlert = true;
    messages.push("Water level high");
  } 
  else if (Math.abs(s1 - s2) > 512) {
    isAlert = true;
    messages.push("Water sensor mismatch");
  }

  // ===== FINAL DECISION =====
  if (isAlert) {
    warningPanel.classList.remove("safe");
    warningPanel.classList.add("alert");

    warningBadge.textContent = "ALERT";
    warningText.textContent = "ALERT: " + messages.join(" | ");

    heroStatus.textContent = "Critical environmental attention required";
    heroSubtext.textContent = "One or more monitored values are unsafe.";
  } 
  else {
    warningPanel.classList.remove("alert");
    warningPanel.classList.add("safe");

    warningBadge.textContent = "SAFE";
    warningText.textContent = "NO ALERT - All readings within safe limits";

    heroStatus.textContent = "Environmental conditions are stable";
    heroSubtext.textContent = "All monitored values are within safe range.";
  }
}

function renderHistoryTable() {
  historyTableBody.innerHTML = historyRows.map(row => `
    <tr>
      <td>${row.time}</td>
      <td>${row.temp}</td>
      <td>${row.hum}</td>
      <td>${row.s1}</td>
      <td>${row.s2}</td>
      <td>${row.warning}</td>
    </tr>
  `).join("");
}

function renderMobileHistory() {
  if (!historyMobileList) return;

  historyMobileList.innerHTML = historyRows.map(row => {
    const isAlert = row.warning.includes("ALERT");

    return `
      <div class="history-mobile-item">
        <div class="history-mobile-top">
          <div class="history-mobile-time">${row.time}</div>
          <div class="history-mobile-badge ${isAlert ? "alert" : "safe"}">
            ${isAlert ? "ALERT" : "SAFE"}
          </div>
        </div>

        <div class="history-mobile-grid">
          <div class="history-mobile-field">
            <span>Temperature</span>
            <strong>${row.temp} °C</strong>
          </div>

          <div class="history-mobile-field">
            <span>Humidity</span>
            <strong>${row.hum} %</strong>
          </div>

          <div class="history-mobile-field">
            <span>Sensor 1</span>
            <strong>${row.s1}</strong>
          </div>

          <div class="history-mobile-field">
            <span>Sensor 2</span>
            <strong>${row.s2}</strong>
          </div>
        </div>

        <div class="history-mobile-warning">
          <span>Warning</span>
          <strong>${row.warning}</strong>
        </div>
      </div>
    `;
  }).join("");
}

function addHistoryRow(time, temp, hum, s1, s2, warning) {
  historyRows.unshift({ time, temp, hum, s1, s2, warning });

  if (historyRows.length > 10) {
    historyRows.pop();
  }

  renderHistoryTable();
  renderMobileHistory();
}

async function fetchStatus() {
  try {
    const res = await fetch(ESP32_STATUS);
    const data = await res.json();

    deviceConnection.textContent = "Connected";
    deviceApi.textContent = data.ip || "Available";
  } catch (error) {
    deviceConnection.textContent = "Disconnected";
    deviceApi.textContent = "Unavailable";
  }
}

function updateMetricAlerts(temp, hum, s1, s2) {
  // Temperature: alert if above 25C
  if (temp > 25) {
    tempAlertText.textContent = "Alert: Above 25°C";
    tempAlertText.className = "card-alert alert-text";
    tempCard.classList.add("alert-card");
    tempCard.classList.remove("safe-card");
  } else {
    tempAlertText.textContent = "Normal: Within safe limit";
    tempAlertText.className = "card-alert safe-text";
    tempCard.classList.add("safe-card");
    tempCard.classList.remove("alert-card");
  }

  // Humidity: alert if below 60 or above 70
  if (hum < 60 || hum > 70) {
    humidityAlertText.textContent = "Alert: Below 60% or above 70%";
    humidityAlertText.className = "card-alert alert-text";
    humidityCard.classList.add("alert-card");
    humidityCard.classList.remove("safe-card");
  } else {
    humidityAlertText.textContent = "Normal: Between 60% and 70%";
    humidityAlertText.className = "card-alert safe-text";
    humidityCard.classList.add("safe-card");
    humidityCard.classList.remove("alert-card");
  }

  // Water Sensor 1
  if (s1 > 384) {
    sensor1AlertText.textContent = "Alert: Water level high";
    sensor1AlertText.className = "card-alert alert-text";
    sensor1Card.classList.add("alert-card");
    sensor1Card.classList.remove("safe-card");
  } else {
    sensor1AlertText.textContent = "Normal: Safe reading";
    sensor1AlertText.className = "card-alert safe-text";
    sensor1Card.classList.add("safe-card");
    sensor1Card.classList.remove("alert-card");
  }

  // Water Sensor 2
  if (s2 > 384) {
    sensor2AlertText.textContent = "Alert: Water level high";
    sensor2AlertText.className = "card-alert alert-text";
    sensor2Card.classList.add("alert-card");
    sensor2Card.classList.remove("safe-card");
  } else {
    sensor2AlertText.textContent = "Normal: Safe reading";
    sensor2AlertText.className = "card-alert safe-text";
    sensor2Card.classList.add("safe-card");
    sensor2Card.classList.remove("alert-card");
  }
}

async function fetchLiveData() {
  try {
    const res = await fetch(ESP32_API);
    const data = await res.json();

    const now = new Date();
    const timeLabel = now.toLocaleTimeString();

    temperatureValue.textContent = `${data.temperature} °C`;
    humidityValue.textContent = `${data.humidity} %`;
    sensor1Value.textContent = data.sensor1;
    sensor2Value.textContent = data.sensor2;
    lastUpdate.textContent = timeLabel;

    updateMetricAlerts(data.temperature, data.humidity, data.sensor1, data.sensor2);

    updateWarningUI(
      data.temperature,
      data.humidity,
      data.sensor1,
      data.sensor2
    );

    pushChartData(timeLabel, data.temperature, data.humidity, data.sensor1, data.sensor2);
    addHistoryRow(timeLabel, data.temperature, data.humidity, data.sensor1, data.sensor2, data.warning);
  } catch (error) {
    deviceConnection.textContent = "Disconnected";
    heroStatus.textContent = "Unable to reach ESP32 live API";
    heroSubtext.textContent = "Check hotspot connection, ESP32 IP address, and API availability.";
    console.error("Fetch error:", error);
  }
}

themeToggleBtn.addEventListener("click", () => {
  const current = getTheme();
  const next = current === "dark" ? "light" : "dark";
  document.documentElement.setAttribute("data-theme", next);
  localStorage.setItem("dashboard-theme", next);
  rebuildCharts();
});

mobileMenuBtn.addEventListener("click", () => {
  sidebar.classList.toggle("open");
});

document.addEventListener("click", (e) => {
  if (window.innerWidth > 900) return;
  if (!sidebar.classList.contains("open")) return;

  const clickedInsideSidebar = sidebar.contains(e.target);
  const clickedMenuButton = mobileMenuBtn.contains(e.target);

  if (!clickedInsideSidebar && !clickedMenuButton) {
    sidebar.classList.remove("open");
  }
});

refreshBtn.addEventListener("click", () => {
  fetchStatus();
  fetchLiveData();
});

window.addEventListener("resize", () => {
  rebuildCharts();
});

applySavedTheme();
rebuildCharts();
fetchStatus();
fetchLiveData();

setInterval(fetchStatus, 8000);
setInterval(fetchLiveData, 2000);
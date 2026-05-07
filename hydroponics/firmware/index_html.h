#pragma once

#include <pgmspace.h>

static const char INDEX_HTML[] PROGMEM = R"HTML(<!doctype html>
<html><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Hydroponics</title>
<style>
body{font-family:sans-serif;max-width:760px;margin:1rem auto;padding:0 1rem;color:#222}
h1{font-size:1.4rem;margin:.2rem 0 1rem}
.state{display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:.5rem;margin-bottom:1rem}
.state div{background:#f4f4f4;padding:.5rem .75rem;border-radius:4px}
.state b{display:block;font-size:1.2rem}
form{display:grid;gap:.75rem;margin-bottom:1rem}
fieldset{border:1px solid #ccc;padding:.5rem .75rem;border-radius:4px}
legend{padding:0 .25rem;font-weight:bold}
label{display:block;margin:.25rem 0}
input[type=time]{width:7rem}
button{padding:.5rem 1rem;font-size:1rem}
canvas{max-width:100%;height:260px}
small{color:#666}
</style></head>
<body>
<h1>Hydroponics</h1>
<div class="state">
  <div>Temp<b><span id="t">-</span> &deg;C</b></div>
  <div>Humidity<b><span id="h">-</span> %</b></div>
  <div>Lights<b><span id="lon">-</span></b></div>
  <div>Pump<b><span id="pon">-</span></b></div>
  <div>Fan<b><span id="fon">-</span></b></div>
  <div>Time<b><span id="tm">-</span></b></div>
</div>
<form id="f" method="post" action="/api/schedule"></form>
<canvas id="chart"></canvas>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<script>
const $ = s => document.querySelector(s);
const m2t = m => `${String(Math.floor(m/60)).padStart(2,'0')}:${String(m%60).padStart(2,'0')}`;
const t2m = t => { const [h,mi] = (t||'00:00').split(':').map(Number); return h*60+mi; };

const CHANNELS = [
  {key: 'lights', label: 'Lights'},
  {key: 'pump',   label: 'O2 Pump'},
  {key: 'fan',    label: 'Fan'},
];

function buildForm() {
  const f = $('#f');
  for (const c of CHANNELS) {
    f.insertAdjacentHTML('beforeend', `
      <fieldset>
        <legend>${c.label}</legend>
        <label>Start <input type="time" data-${c.key}="start"></label>
        <label>End <input type="time" data-${c.key}="end"></label>
        <label><input type="checkbox" name="${c.key}_always" value="1" data-${c.key}="always"> Always on</label>
        <input type="hidden" name="${c.key}_start" data-${c.key}="hstart">
        <input type="hidden" name="${c.key}_end"   data-${c.key}="hend">
      </fieldset>`);
  }
  f.insertAdjacentHTML('beforeend', '<button type="submit">Save</button>');
  f.addEventListener('submit', () => {
    for (const c of CHANNELS) {
      f.querySelector(`[data-${c.key}=hstart]`).value = t2m(f.querySelector(`[data-${c.key}=start]`).value);
      f.querySelector(`[data-${c.key}=hend]`).value   = t2m(f.querySelector(`[data-${c.key}=end]`).value);
    }
  });
}
buildForm();

let chart, formInit = false;
async function refresh() {
  const s = await fetch('/api/state').then(r => r.json());
  $('#t').textContent = s.temp_c ?? '-';
  $('#h').textContent = s.humidity ?? '-';
  $('#lon').textContent = s.lights_on ? 'ON' : 'off';
  $('#pon').textContent = s.pump_on ? 'ON' : 'off';
  $('#fon').textContent = s.fan_on ? 'ON' : 'off';
  $('#tm').textContent = s.epoch ? new Date(s.epoch*1000).toLocaleString() : '(no NTP)';
  if (!formInit) {
    formInit = true;
    for (const c of CHANNELS) {
      const cc = s.config[c.key];
      $(`[data-${c.key}=start]`).value = m2t(cc.start_minute);
      $(`[data-${c.key}=end]`).value   = m2t(cc.end_minute);
      $(`[data-${c.key}=always]`).checked = cc.always_on;
    }
  }
}
async function loadChart() {
  const h = await fetch('/api/history').then(r => r.json());
  const labels = h.map(x => new Date(x.t*1000).toLocaleTimeString());
  const data = {
    labels,
    datasets: [
      {label: 'Temp °C',    data: h.map(x => x.tc), borderColor: '#e44'},
      {label: 'Humidity %', data: h.map(x => x.h),  borderColor: '#48c'},
    ]
  };
  if (chart) { chart.data = data; chart.update(); return; }
  chart = new Chart($('#chart'), {
    type: 'line', data,
    options: { animation: false, elements: { point: {radius: 0} } }
  });
}
refresh(); loadChart();
setInterval(refresh, 5000);
setInterval(loadChart, 60000);
</script>
</body></html>
)HTML";

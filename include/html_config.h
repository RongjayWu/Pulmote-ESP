#ifndef HTML_CONFIG_H
#define HTML_CONFIG_H

const char WIFI_CONFIG_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Pulmote WiFi</title>
<style>
body{margin:0;padding:20px;font-family:Arial,sans-serif;background:#667eea;min-height:100vh;display:flex;align-items:center;justify-content:center}
.container{background:white;border-radius:15px;padding:40px;max-width:500px;width:100%;box-shadow:0 10px 40px rgba(0,0,0,0.3)}
h1{color:#333;text-align:center;margin:0 0 10px 0}
.subtitle{text-align:center;color:#666;margin:0 0 30px 0}
.status-box{background:#f5f5f5;border-left:4px solid #667eea;padding:15px;border-radius:5px;margin:0 0 25px 0}
.status-item{display:flex;justify-content:space-between;padding:8px 0;font-size:14px}
.status-label{color:#666;font-weight:500}
.status-value{color:#333;font-weight:bold}
.status-value.ok{color:#4CAF50}
.status-value.fail{color:#f44336}
.form-group{margin-bottom:20px}
label{display:block;margin-bottom:8px;color:#333;font-weight:500;font-size:14px}
input,select{width:100%;padding:12px;border:2px solid #e0e0e0;border-radius:8px;font-size:14px;box-sizing:border-box}
input:focus{outline:none;border-color:#667eea}
.network-list{max-height:150px;overflow-y:auto;margin-bottom:10px;border:1px solid #e0e0e0;border-radius:5px}
.net-item{padding:10px;background:#f9f9f9;border-bottom:1px solid #e0e0e0;cursor:pointer}
.net-item:hover{background:#e8e8e8}
.net-name{font-weight:500}
.net-signal{font-size:12px;color:#999}
button{width:100%;padding:12px;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:white;border:none;border-radius:8px;font-size:16px;font-weight:600;cursor:pointer;margin-top:10px}
button:hover{box-shadow:0 5px 20px rgba(102,126,234,0.4)}
.msg{padding:12px;border-radius:5px;margin:15px 0;display:none;font-size:14px}
.msg.ok{background:#c8e6c9;color:#2e7d32}
.msg.fail{background:#ffcdd2;color:#c62828}
</style>
</head>
<body>
<div class="container">
<h1>Pulmote-ESP</h1>
<p class="subtitle">WiFi Setup</p>

<div class="status-box">
<div class="status-item">
<span class="status-label">Status:</span>
<span id="sta-status" class="status-value fail">Not Connected</span>
</div>
<div class="status-item">
<span class="status-label">Mode:</span>
<span id="ap-status" class="status-value ok">AP Active</span>
</div>
<div class="status-item">
<span class="status-label">Signal:</span>
<span id="sig" class="status-value">-- dBm</span>
</div>
</div>

<div id="msg" class="msg"></div>

<div class="form-group">
<label>Available Networks</label>
<button onclick="scan_wifi()">Scan Networks</button>
<div class="network-list" id="net_list">
<p style="padding:10px;text-align:center;color:#999">Click Scan...</p>
</div>
</div>

<form onsubmit="save_config(event)">
<div class="form-group">
<label>Network Name</label>
<input type="text" id="ssid" placeholder="SSID" required>
</div>

<div class="form-group">
<label>Password</label>
<input type="password" id="pwd" placeholder="Password">
</div>

<button type="submit">Save and Connect</button>
</form>
</div>

<script>
function upd_status(){
fetch("/api/status").then(r=>r.json()).then(d=>{
var s=document.getElementById("sta-status");
s.textContent=d.sta_connected?"Connected":"Not Connected";
s.className=d.sta_connected?"status-value ok":"status-value fail";
document.getElementById("sig").textContent=d.rssi+" dBm";
}).catch(e=>{});
}

function scan_wifi(){
var div=document.getElementById("net_list");
div.innerHTML="<p style=\"padding:10px;text-align:center\">Scanning...</p>";
fetch("/api/scan").then(r=>r.json()).then(n=>{
if(n.length===0){div.innerHTML="<p style=\"padding:10px\">No networks</p>";return;}
div.innerHTML="";
n.forEach(x=>{
var d=document.createElement("div");
d.className="net-item";
d.innerHTML="<div class=\"net-name\">"+x.ssid+"</div><div class=\"net-signal\">"+x.rssi+" dBm</div>";
d.onclick=function(){document.getElementById("ssid").value=x.ssid;document.getElementById("pwd").focus();};
div.appendChild(d);
});
}).catch(e=>{div.innerHTML="<p style=\"padding:10px\">Scan error</p>";});
}

function save_config(e){
e.preventDefault();
var s=document.getElementById("ssid").value;
if(!s){show_msg("Enter SSID","fail");return;}
fetch("/api/save",{method:"POST",headers:{"Content-Type":"application/json"},body:JSON.stringify({ssid:s,password:document.getElementById("pwd").value})}).then(r=>r.json()).then(d=>{
if(d.success){show_msg("Saved! Restarting...","ok");}else{show_msg("Error: "+d.message,"fail");}
}).catch(e=>{show_msg("Request error","fail");});
}

function show_msg(t,c){
var m=document.getElementById("msg");
m.textContent=t;
m.className="msg "+c;
m.style.display="block";
if(c!="fail"){setTimeout(function(){m.style.display="none";},5000);}
}

upd_status();
setInterval(upd_status,3000);
</script>
</body>
</html>
)HTML";

#endif

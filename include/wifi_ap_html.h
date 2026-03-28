// Pulmote AP Mode HTML
#pragma once

const char WIFI_AP_HTML[] = R"HTML(
<html>
<head>
	<title>Pulmote WiFi Config</title>
	<style>
		body { font-family: Arial; text-align: center; margin-top: 40px; }
		.container { display: flex; flex-direction: column; align-items: center; }
		#wifi-list { width: 300px; margin: 20px auto; text-align: left; max-height: 220px; overflow-y: auto; border: 1px solid #eee; }
		.wifi-item { cursor: pointer; padding: 6px; border-bottom: 1px solid #ccc; }
		.wifi-item.selected { background: #e0f7fa; }
		input, button { margin: 8px; padding: 8px; width: 220px; }
	</style>
</head>
<body>
	<div class="container">
		<h2>Pulmote</h2>
		<div>
			<button onclick="scanWifi()">Re-search for nearby networks</button>
		</div>
		<div id="wifi-list"></div>
		<input id="ssid" placeholder="SSID" readonly />
		<input id="password" placeholder="Password" type="password" />
		<button id="connectBtn" onclick="connectWifi()">Connect to this network</button>
	</div>
	<script>
		function scanWifi() {
			fetch('/scan').then(r => {
				if (r.status === 200) return r.json();
				if (r.status === 202) return Promise.resolve({__scanning: true});
				return Promise.reject();
			}).then(listJson => {
				if (listJson && listJson.__scanning) {
					let el = document.getElementById('status');
					if (el) el.innerText = 'Status: scanning...';
					setTimeout(scanWifi, 800);
					return;
				}
				let list = document.getElementById('wifi-list');
				list.innerHTML = '';
				if (!Array.isArray(listJson)) {
					list.innerText = 'Scan returned unexpected data';
					return;
				}
				listJson.forEach(net => {
					let el = document.createElement('div');
					el.className = 'wifi-item';
					el.dataset.ssid = net.ssid || '';
					el.innerText = (net.ssid || '') + (net.rssi !== undefined ? (' (RSSI: ' + net.rssi + ')') : '');
					el.onclick = function() {
						Array.from(document.getElementsByClassName('wifi-item')).forEach(i => i.classList.remove('selected'));
						this.classList.add('selected');
						document.getElementById('ssid').value = this.dataset.ssid;
						document.getElementById('password').focus();
					};
					list.appendChild(el);
				});
			}).catch(() => {
				// 若失敗則顯示錯誤並讓使用者重試
				let el = document.getElementById('status');
				if (el) el.innerText = 'Status: scan failed';
			});
		}
		// 首次載入自動搜尋
		window.onload = function() {
			scanWifi();
		};

		function connectWifi() {
			let ssid = document.getElementById('ssid').value;
			let pwd = document.getElementById('password').value;
			if (!ssid) {
				alert('Please select a WiFi network first');
				return;
			}
			if (!pwd) {
				alert('Please enter the WiFi password');
				return;
			}

			console.log('sending connect request...');
			fetch('/connect', {
				method: 'POST',
				headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
				body: new URLSearchParams({ ssid: ssid, pass: pwd })
			}).then(r => r.text()).then(msg => {
				// 不再以 alert 顯示成功或回應，僅記錄於 console
				console.log('connect response:', msg);
			}).catch(() => {
				// 失敗時僅記錄於 console（僅在欄位為空時才 alert）
				console.log('Failed to send connect request');
			});
		}
	</script>
</body>
</html>
)HTML";

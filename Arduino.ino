#include <EEPROM.h> 
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

#define SSID_ADDR   0
#define PASS_ADDR   100
#define MAX_LEN     100
#define IN1 D0
#define IN2 D1
#define IN3 D2
#define IN4 D3
#define LED_PIN D4
#define ENB D6
#define ENA D5

String ssid     = "Ezma";
String password = "Ezma12345";

DNSServer       dnsServer;

AsyncWebServer server(80);

String          addListFunction = "";
String          ipStr;
String          state;
String          action;

unsigned long previousMillis = 0, prev = 0;
int blinkTimes = 0;
int blinkDelay = 0;
int speed;
bool ledOn = false;
bool isBlinking = false;
bool callF = false;
bool once = false;

bool BlinkOnce = false;
unsigned long blinkStart;

const char index_html_find_wifi[] PROGMEM =  R"rawliteral(
<!DOCTYPE html><html lang=en data-theme=lM><head>
<meta charset=UTF-8><link rel="icon" href="data:;base64,iVBORw0KGgo="><meta name=theme-color content=#9097E6>
<meta name=viewport content=width=device-width, initial-scale=1>
<title>WiFi Connect Page</title><style>
:root{ --bgC:#ffffff; --bdC-B:#EEEEEE; --boxS:8px 8px 16px #bebebe, -8px -8px 16px #ffffff; --boxSl:3px 3px 6px #bebebe, -3px -3px 6px #ffffff; --bgC-I:#ffffff; } 
[data-theme=dM]{ --bgC: #ffffff1f; --bdC-B:#111111; --boxS:none; --boxSl:none; --bgC-I:#393939EE; } *{ font-family:"Roboto",sans-serif;; margin:0; padding:0; color:#9097E6; } 
body{ height:75rem; background-color:var(--bdC-B); } 
h1{ background-color:var(--bgC); margin:5%; padding:3% 5% 3% 5%; border-radius: 35rem; border:.3rem solid #9097E6; width:80%; box-shadow:var(--boxS); display:flex; align-items:center; justify-content:space-between; } 
h4{ color:#9097E6; font-weight:normal; text-align:center; padding-bottom:5px; } 
#b{ background-color:var(--bdC-B); border:none; padding:2%; height:2.5rem; width:2.5rem; border-radius:50%; } 
#dMI{ fill:#9097E6; } 
ul{ width:auto; max-height:30.5rem; overflow:auto; margin-left:0 2.5% 0 2.5%; background-color:var(--bgC); border-radius:3rem; padding:1rem 1rem 1rem 1rem; } 
li{ font-size:1.3rem; list-style:none; background-color:#9097E6; padding:12px 20px 12px 20px; border-radius:2rem; margin:1.5rem; box-shadow:var(--boxSl); transition:transform 0.3s ease; } 
li div{ color:#111111; display:flex; align-items:center; gap:1.1rem; } 
input[type=text], input[type=password], .btn{ box-shadow:1px 1px 4px #00000055,-0px -0px 0px #00000055; padding:.4rem; background-color:var(--bgC-I); font-size:1rem; color:#9097E6aa; border-radius:.5rem; margin:1.5rem 0 .5rem 0; border:none; } 
.hide{ max-height:0; opacity:0; display:none; transition:opacity .5s ease,max-height .5s ease; } 
.hide.show{ display:flex; max-height:10000px; opacity:1; } 
.hide.showA{ display:block; max-height:10000px; opacity:1; } 
.from{ display:none; text-align:center; } 
input[type=password], input[type=text]{ width:90%; color:#9097E6; padding-left:12px; outline:none; transition:box-shadow 0.3s ease; } 
.btn:active{ transform:scale(1.05); box-shadow:none; } 
.btn:active::after{ box-shadow: 2px 2px 4px #484848AD,-0px -0px 0px #484848AD; transform:scale(-1.05); } 
.div{ text-overflow:hidden; overflow:auto; white-space:nowrap; width:min-content; } 
.close{ margin-left:auto; color:#111; float:right; font-size:24px; font-weight:bold; } 
.rotate{ transform:rotate(45deg); } 
.Add{ margin-top:.5rem; background-color:#9097E6; color:var(--bdC-B); margin-left:auto; width:25%; } 
input[type=password]::placeholder, input[type=text]::placeholder{ color:#9097E6aa; font-style:normal; } 
.addN{ height:auto; align-items:end; margin:1rem 1.5rem 1rem 1.5rem; padding:1rem 1.5rem 1rem 1.5rem; border-radius:1rem; background-color:var(--bgC); } 
.addN div{ gap:1.5rem; align-items:center; } 
.addN div div:first-child{ font-weight:normal; font-size:2rem; } 
.spinner { height: 20px; width: 20px; border: 4px solid #9097E6a0; border-top: 4px solid #9097E6; border-radius: 50%; animation: spin .5s linear infinite; margin: 0 auto; } @keyframes spin { to { transform: rotate(360deg); } }
</style></head><body><h1>Wi-Fi<button id=b onclick=tglT()>
<svg id=dMI xmlns=http://www.w3.org/2000/svg height=24px viewBox="0 -960 960 960" width=24px>
<path d="M480-120q-150 0-255-105T120-480q0-150 105-255t255-105q14 0 27.5 1t26.5 3q-41 29-65.5 75.5T444-660q0 90 63 153t153 63q55 0 101-24.5t75-65.5q2 13 3 26.5t1 27.5q0 150-105 255T480-120Zm0-80q88 0 158-48.5T740-375q-20 5-40 8t-40 3q-123 0-209.5-86.5T364-660q0-20 3-40t8-40q-78 32-126.5 102T200-480q0 116 82 198t198 82Zm-10-270Z" />
</svg></button></h1><ul><h4>-- Find Your Wifi --</h4><h4>-- The End --</h4></ul>
<div class=addN onclick=sAI(this)>
<div class="from hide" onclick="event.stopPropagation()">
<h3 style="display:flex;align-items:center;">ADD NETWORK
<span onclick=isAS() style="margin-left:auto;">&times;</span>
</h3><input type=text placeholder=SSID>
<input type=password style="margin-top:.5rem;" placeholder=Password>
<button class="btn Add" onclick=aW(this) type=submit>Add</button>
</div><div class=bar style="display: flex;z-index: 100;">
<div>+</div><h2>Add Network</h2></div></div></div><script>
const d=document,html=d.documentElement;
let ssid,pass;
function sST(el){
sessionStorage.setItem('theme',el);}
function gST(){
let a = sessionStorage.getItem('theme');
return a;}
function sA(el){
html.setAttribute('data-theme', el);}
function gA(){
let a = html.getAttribute('data-theme');
return a;}
function tglT(){
const newTheme = gA() === 'lM' ? 'dM' : 'lM';
sA(newTheme);
sST(newTheme);}
function isAS(){
ssid = '';
pass = '';
window.scrollTo({
top: 0,
behavior: 'smooth'});
d.querySelectorAll('input[type=password]').forEach(el => el.value.trim() && (el.value = ''));
d.querySelectorAll('input[type=text]').forEach(el => el.value.trim() && (el.value = ''));
d.querySelector('.from.hide.showA')?.classList.remove('showA');
d.querySelector('.hide.show')?.classList.remove('show');
d.querySelector('.bar.hide')?.classList.remove('hide');
d.querySelector('.rotate')?.classList.remove('rotate');}
function sI(el){
el.querySelector('.from.hide')?.classList.add('showA');
el.querySelector('.hide')?.classList.add('show');
el.querySelector('.close')?.classList.add('rotate');}
function sCI(el){
isAS();sI(el);
ssid = el.querySelector('.div').innerText;}
function hCI(el){
(el.classList.contains('rotate')) ? isAS(): (isAS(), sI(el.closest('li')));}
function sAI(el){
if(!el.querySelector('.bar.hide')){
isAS();sI(el);
el.querySelector('.bar').classList.add('hide');
el.scrollIntoView({
behavior: 'smooth',
block: 'center'});}}
function makeList(nam,dBm){
let ul = d.querySelector('ul');
[...d.querySelectorAll('ul h4')].find(h4 => h4.textContent.includes('-- The End --'))?.remove();
ul.innerHTML += `<li onclick=sCI(this)>
<div><div>${wLI(dBm)}</div><div class=div>${nam}</div>
<span class=close onclick="event.stopPropagation(); hCI(this)">+</span>
</div><div class=hide onclick="event.stopPropagation()">
<input type=password placeholder=Password>
<button class=btn onclick=cW(this)>Connect</button>
</div></li><div id="loading" class="spinner" style="display:none;"></div>
<h4 style="margin-top:1.5rem;">-- The End -- </h4>  `}
function wLI(dBm){
let p;if(dBm >= -40){
p = `<svg xmlns=http://www.w3.org/2000/svg height=24px viewBox="0 -960 960 960" width=24px fill=#erefe3><path d="M720-320h80v-40q0-17-11.5-28.5T760-400q-17 0-28.5 11.5T720-360v40ZM480-120 0-600q95-97 219.5-148.5T480-800q136 0 260.5 51.5T960-600L859-499q-23-11-48-16t-51-5q-100 0-170 70t-70 170q0 26 5 51t16 48l-61 61Zm200 0q-17 0-28.5-11.5T640-160v-120q0-17 11.5-28.5T680-320v-40q0-33 23.5-56.5T760-440q33 0 56.5 23.5T840-360v40q17 0 28.5 11.5T880-280v120q0 17-11.5 28.5T840-120H680ZM174-540q67-48 145-74t161-26q83 0 161 26t145 74l58-58q-79-60-172-91t-192-31q-99 0-192 31t-172 91l58 58Z"/></svg>`;
}else if(dBm >= -70 && dBm < -40){
p = `<svg xmlns=http://www.w3.org/2000/svg height=24px viewBox="0 -960 960 960" width=24px fill=#erefe3><path d="M480-120 0-600q95-97 219.5-148.5T480-800q137 0 261 51t219 149L859-499q-22-10-45.5-15t-47.5-6l78-78q-79-60-172-91t-192-31q-99 0-192 31t-172 91l172 172q44-26 92.5-40t99.5-14q31 0 61.5 6.5T601-459q-38 35-59.5 81.5T520-279q0 26 5 50.5t16 47.5l-61 61Zm200 0q-17 0-28.5-11.5T640-160v-120q0-17 11.5-28.5T680-320v-40q0-33 23.5-56.5T760-440q33 0 56.5 23.5T840-360v40q17 0 28.5 11.5T880-280v120q0 17-11.5 28.5T840-120H680Zm40-200h80v-40q0-17-11.5-28.5T760-400q-17 0-28.5 11.5T720-360v40Z"/></svg>`;
}else if(dBm < -70){
p = `<svg xmlns=http://www.w3.org/2000/svg height=24px viewBox="0 -960 960 960" width=24px fill=#erefe3><path d="M680-120q-17 0-28.5-11.5T640-160v-120q0-17 11.5-28.5T680-320v-40q0-33 23.5-56.5T760-440q33 0 56.5 23.5T840-360v40q17 0 28.5 11.5T880-280v120q0 17-11.5 28.5T840-120H680Zm40-200h80v-40q0-17-11.5-28.5T760-400q-17 0-28.5 11.5T720-360v40ZM480-120 0-600q95-97 219.5-148.5T480-800q136 0 260.5 51.5T960-600L822-463q-14-14-28-28.5T766-520l78-78q-79-60-172-91t-192-31q-99 0-192 31t-172 91l364 364 40-40 28.5 28.5L577-217l-97 97Zm0-357Z"/></svg>`;}return p;}
function cW(el) {
pass = el.parentElement.querySelector('input[type="password"]').value;
(pass.length < 8) ? isAS(): sent(el);}
function aW(el) {
ssid = el.parentElement.querySelector('input[type="text"]').value;
pass = el.parentElement.querySelector('input[type="password"]').value;
(!ssid || pass.length < 8) ? isAS(): sent(el);}
function sent(el) {
const l = el.closest('li').nextElementSibling;
l.style.display = "block";
const p = `ssid=${encodeURIComponent(ssid)}&pass=${encodeURIComponent(pass)}`;
const x = new XMLHttpRequest();
x.open("POST", "/wifi", true);
x.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
x.onloadend = () => { l.style.display = "none"; alert(x.responseText); }
x.send(p);}
window.onload = function(){
if(gST===null){ sST(gA()); }
(gST()==='dM') ? (sA('dM'), sST('dM')) : (sA('lM'), sST('lM'));
if(d.querySelector('li')==null){
d.querySelector('ul').innerHTML = "<h4>-- No Wifi Found --</h4>";
}if (performance.navigation.type === performance.navigation.TYPE_RELOAD) {
window.location.href = 'http://192.168.4.1/find-load';}};
//No_Network_Available
</script>
</body>
</html>
)rawliteral";

const char index_html_copy[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html lang="en"><head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="icon" href="data:;base64,iVBORw0KGgo="><title>Car Control User Manual</title><style>
:root {--primary: #2c3e50;--accent: #9097E6;--bg: #EEECF1;--text: #34495e;}
* {box-sizing: border-box;margin: 0;padding: 0;-webkit-text-size-adjust: 100%;text-size-adjust: 100%;}
body {font-family: Arial, sans-serif;background: var(--bg);color: var(--text);padding: 20px;box-shadow: inset 0 200px 200px -150px var(--accent);}
header {text-align: center;margin-bottom: 2rem;}
header h1 {font-size: 2.5rem;opacity: .8;color: var(--primary);text-shadow: 3px 5px 7px rgba(0, 0, 0, 0.3);}
header p {margin-top: 0.5rem;font-size: 1rem;font-style: bolder;font-family: "Lucida Console", monospace;color: var(--accent);}
nav {position: relative;z-index: 0;padding: 15px 20px;box-shadow: inset 0 2px 5px rgba(0, 0, 0, 0.3);margin-bottom: 1rem;border-radius: 2rem;text-shadow: 0 2px 2px rgba(0, 0, 0, 0.2);background: transparent;text-align: center;strong {font-size: 1.4rem;color: var(--accent);}}
nav::before {content: "";position: absolute;border-radius: 2rem;inset: 0;z-index: -1;background: #9097E622 url("data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 -960 960 960' fill='%239097E640'><path d='M784-120 532-372q-30 24-69 38t-83 14q-109 0-184.5-75.5T120-580q0-109 75.5-184.5T380-840q109 0 184.5 75.5T640-580q0 44-14 83t-38 69l252 252-56 56ZM380-400q75 0 127.5-52.5T560-580q0-75-52.5-127.5T380-760q-75 0-127.5 52.5T200-580q0 75 52.5 127.5T380-400Z'/></svg>") no-repeat center center / contain;filter: blur(1px);}
nav ul {list-style: none;padding: 0;margin-top: 10px; line-height: 30px; li:last-child {border: none;}}
nav ul li {border-bottom: 1px solid #00000020;}
section {padding: 1.5rem;margin-bottom: 1rem;border-radius: 1rem;box-shadow: 0 3px 5px rgba(0, 0, 0, 0.3);box-shadow: inset 0 2px 5px rgba(0, 0, 0, 0.3);text-shadow: 0 2px 2px rgba(0, 0, 0, 0.2);}
.highlight { animation: fadeHighlight .8s ease; }
@keyframes fadeHighlight { 0% { background-color: var(--bg); opacity: 0.7; } 100% { background-color: auto; opacity: 1; } }
h2 {font-size: 1.5rem;color: var(--primary);}
p {margin-top: 0.75rem;margin-bottom: 1rem;line-height: 1.6;}
.info-box { background: #EBEEFF; padding: 10px; border-radius: 5px; font-size: 1.1rem; font-weight: bold; display: inline-block; color: var(--accent); word-break: break-all; overflow-wrap: break-word; box-shadow: inset 0 .8px 3px rgba(0, 0, 0, .2); }
.copy-btn { display: flex; margin-top: 0.5rem; padding: 0.5rem 1rem; background: var(--accent); color: #fff; border: none; border-radius: 4px; box-shadow: 0 2px 2px rgba(0, 0, 0, 0.3); align-items: center; justify-content: space-between; svg{margin-left: 3px;}}
footer { text-align: center; margin-top: 2rem; color: #7f8c8d; font-size: 0.9rem; } 
.delete {margin: 0 0 1rem 50%;transform: translateX(-50%);background: #FFEBEB;border-radius: 5px;font-size: 1.1rem;font-weight: bold;display: flex;color: #E69090;width: 100%;box-shadow: 0 .8px 3px rgba(0, 0, 0, .2);align-items: center;max-width: 100%;padding: 10px 1.5rem 10px 1.5rem;justify-content: space-between;cursor: pointer; span {  text-overflow: hidden;  overflow: auto;  white-space: nowrap;  width: min-content;  margin: 0 auto 0 10px;  opacity: .8;}*:first-child {  transform: translateY(-2px);}* {  filter: drop-shadow(0 1.5px 1px rgba(0, 0, 0, 0.2));}}  
@media (hover: hover) and (pointer: fine) { a:hover { color: var(--accent); text-decoration: none; } .copy-btn:hover { opacity: 0.8; color: var(--text); box-shadow: none; svg { fill: var(--text); } } } 
@media (hover: none) and (pointer: coarse) { a:active { color: var(--accent); text-decoration: none; } .copy-btn:active { opacity: 0.8; color: var(--text); box-shadow: none;  svg { fill: var(--text); } } } 
.sp { font-family: "Courier New", monospace; border-bottom: 1px solid #7F8C8D3f; font-style: bolder; } 
#troubleshoot ul { padding: 0.75rem 0 0 20px;  li { padding: 4px; } }
#step1 {background: rgba(255, 255, 255, 1) url("data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 -960 960 960' fill='%23EBEEFF90'><path d='M720-120v-120H600v-80h120v-120h80v120h120v80H800v120h-80Zm-240 0L0-600q95-97 219.5-148.5T480-800q136 0 260.5 51.5T960-600L822-463q-14-14-28-28.5T766-520l78-78q-79-60-172-91t-192-31q-99 0-192 31t-172 91l364 364 40-40 28.5 28.5L577-217l-97 97Zm0-357Z'/></svg>") no-repeat center center / 15rem 15rem;}
#step2 {background: rgba(255, 255, 255, 1) url("data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 -960 960 960' fill='%23EBEEFF90'><path d='M200-120q-33 0-56.5-23.5T120-200v-560q0-33 23.5-56.5T200-840h167q11-35 43-57.5t70-22.5q40 0 71.5 22.5T594-840h166q33 0 56.5 23.5T840-760v560q0 33-23.5 56.5T760-120H200Zm0-80h560v-560h-80v120H280v-120h-80v560Zm280-560q17 0 28.5-11.5T520-800q0-17-11.5-28.5T480-840q-17 0-28.5 11.5T440-800q0 17 11.5 28.5T480-760Z'/></svg>") no-repeat center center / 15rem 15rem;}
#step3 {background: rgba(255, 255, 255, 1) url("data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 -960 960 960' fill='%23EBEEFF90'><path d='M480-320 280-520l56-58 104 104v-326h80v326l104-104 56 58-200 200ZM240-160q-33 0-56.5-23.5T160-240v-120h80v120h480v-120h80v120q0 33-23.5 56.5T720-160H240Z'/></svg>") no-repeat center center / contain;}
#troubleshoot {background: rgba(255, 255, 255, 1) url("data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 -960 960 960' fill='%23e3e3e390'><path d='m234-480-12-60q-12-5-22.5-10.5T178-564l-58 18-40-68 46-40q-2-13-2-26t2-26l-46-40 40-68 58 18q11-8 21.5-13.5T222-820l12-60h80l12 60q12 5 22.5 10.5T370-796l58-18 40 68-46 40q2 13 2 26t-2 26l46 40-40 68-58-18q-11 8-21.5 13.5T326-540l-12 60h-80Zm40-120q33 0 56.5-23.5T354-680q0-33-23.5-56.5T274-760q-33 0-56.5 23.5T194-680q0 33 23.5 56.5T274-600ZM592-40l-18-84q-17-6-31.5-14.5T514-158l-80 26-56-96 64-56q-2-18-2-36t2-36l-64-56 56-96 80 26q14-11 28.5-19.5T574-516l18-84h112l18 84q17 6 31.5 14.5T782-482l80-26 56 96-64 56q2 18 2 36t-2 36l64 56-56 96-80-26q-14 11-28.5 19.5T722-124l-18 84H592Zm56-160q50 0 85-35t35-85q0-50-35-85t-85-35q-50 0-85 35t-35 85q0 50 35 85t85 35Z'/></svg>") no-repeat center center / contain;}
#support {background: rgba(255, 255, 255, 1) url("data:image/svg+xml;utf8, <svg xmlns='http://www.w3.org/2000/svg' viewBox='0 -960 960 960' fill='%23e3e3e390'><path d='m480-80-10-120h-10q-142 0-241-99t-99-241q0-142 99-241t241-99q71 0 132.5 26.5t108 73q46.5 46.5 73 108T800-540q0 75-24.5 144t-67 128q-42.5 59-101 107T480-80Zm80-146q71-60 115.5-140.5T720-540q0-109-75.5-184.5T460-800q-109 0-184.5 75.5T200-540q0 109 75.5 184.5T460-280h100v54Zm-101-95q17 0 29-12t12-29q0-17-12-29t-29-12q-17 0-29 12t-12 29q0 17 12 29t29 12Zm-29-127h60q0-30 6-42t38-44q18-18 30-39t12-45q0-51-34.5-76.5T460-720q-44 0-74 24.5T344-636l56 22q5-17 19-33.5t41-16.5q27 0 40.5 15t13.5 33q0 17-10 30.5T480-558q-35 30-42.5 47.5T430-448Zm30-65Z'/></svg>") no-repeat center center / contain;}
a{align-items: center;color: var(--accent);font-weight: 500;transition: color 0.3s ease, text-decoration 0.3s ease;}
li a {display: flex;align-items: center;text-decoration: none;font-weight: 500;transition: color 0.3s ease, text-decoration 0.3s ease;.dot {flex: 1;border-bottom: 3px dotted #9097E6;margin: 0 8px;height: 1px;opacity: .6;}svg {flex-shrink: 0;margin-right: 5px;height: 16px;width: 16px;fill: #9097E6;filter: drop-shadow(0 1.5px 1px rgba(0, 0, 0, 0.4));}}
</style><script defer>
const highlight = (event, selector) => {
event.preventDefault();
const el = document.querySelector(selector);
el.scrollIntoView({ behavior: 'smooth', block: 'center' });
setTimeout(() => {
el.classList.remove('highlight');
void el.offsetWidth;
el.classList.add('highlight');
}, 800);};const sent = () => {
const xhr = new XMLHttpRequest();
xhr.open("GET", "/clear", true);
xhr.onload = function () {
if (xhr.status === 200) {
alert("✔ " + xhr.responseText);} else {
alert("❌ Failed to clear WiFi");
}};xhr.onerror = function () {
alert("❌ Error sending request");};xhr.send();}
</script></head>
<body><header>
<h1>Car Control User Manual</h1>
<p>By following this manual, you can easily use your car's remote control.</p>
</header><nav>
<strong>Table of Contents</strong><ul>
<li><a class="a" href="#step1" onclick="highlight(event,'#step1')"><span>1. Step 1: Connect to Wi-Fi</span></a></li>
<li><a class="a" href="#step2" onclick="highlight(event,'#step2')"><span>2. Step 2: Copy Local IP Address</span></a></li>
<li><a class="a" href="#step3" onclick="highlight(event,'#step3')"><span>3. Step 3: Copy Remote Access Link</span></a></li>
<li><a class="a" href="#troubleshoot" onclick="highlight(event,'#troubleshoot')"><span>4. Troubleshoot</span></a></li>
<li><a class="a" href="#support" onclick="highlight(event,'#support')"><span>5. Contact & Support</span></a></li>
</ul></nav><section id="step1">
<h2>Step 1: Connect to Wi-Fi</h2>
<p><strong>Connect to the following Wi-Fi network for remote access:</strong></p>
<div class="info-box ssi"></div>
<p>If you are not connected to this network, the next step will not work.</p>
</section><section id="step2">
<h2>Step 2: Copy Local IP Address</h2>
<p><strong>The local IP address of the car is given below. Copy it and keep it:</strong></p>
<div class="info-box" id="ipAddress"></div><br>
<button id="copyIPBtn" class="copy-btn">Local IP</button>
<p class="sp">After copying the local IP,</p>
<p>Paste it to enable remote access. You'll get the remote access link in the next step. Without this, remote control won't work.</p>
</section><section id="step3">
<h2>Step 3: Copy Remote Access Download Link</h2>
<p><strong>Copy the control panel link by clicking the button below:</strong></p>
<div class="info-box" id="Dlink"></div><br>
<button id="copyLinkBtn" class="copy-btn">Remote Access Download Link </button>
<p>If you paste the link and open it in your browser, You will be taken to the download page for the remote control panel. From there, you will download the file as a zip.</p>
</section><div class="delete" onclick="sent()"><svg xmlns="http://www.w3.org/2000/svg" height="24px" viewBox="0 -960 960 960" width="24px" fill="#E69090">
<path d="M480-120q-42 0-71-29t-29-71q0-42 29-71t71-29q42 0 71 29t29 71q0 42-29 71t-71 29ZM254-346l-84-86q59-59 138.5-93.5T480-560q92 0 171.5 35T790-430l-84 84q-44-44-102-69t-124-25q-66 0-124 25t-102 69ZM84-516 0-600q92-94 215-147t265-53q142 0 265 53t215 147l-84 84q-77-77-178.5-120.5T480-680q-116 0-217.5 43.5T84-516Z" />
</svg> <span class="ssi"> </span> 
<svg xmlns="http://www.w3.org/2000/svg" height="24px" viewBox="0 -960 960 960" width="24px" fill="#E69090"><path d="M280-120q-33 0-56.5-23.5T200-200v-520h-40v-80h200v-40h240v40h200v80h-40v520q0 33-23.5 56.5T680-120H280Zm400-600H280v520h400v-520ZM360-280h80v-360h-80v360Zm160 0h80v-360h-80v360ZM280-720v520-520Z" />
</svg></div><section id="troubleshoot">
<h2>Troubleshooting</h2><ul>
<li><span style="color: var(--accent);">Wi-Fi connection problem:</span> Reboot the router and try again.</li>
<li><span style="color: var(--accent);">IP not copying:</span> Check the browser's Clipboard permissions.</li>
<li><span style="color: var(--accent);">Link not working:</span> Make sure the IP is pasted correctly and the devices are on the same network.</li>
</ul></section><section id="support">
<h2>Contact & Support</h2>
<p><strong>Contact us if you have any problems or questions:</strong></p>
<p><strong style="color: var(--accent);">Phone:</strong> <a href="tel:+8801872605055">+8801872605055</a></p>
<p><strong style="color: var(--accent);">Email:</strong> <a href="mailto:marufhossainkeyas@gmail.com">marufhossainkeyas@gmail.com</a></p>
</section><footer>&copy; 2025 <span style="color: var(--accent);">Car Control System</span>. All rights reserved.</footer><script>
let ip = 'http://LOCAL_IP', ssi = 'SSID_NAME', remoteLink = 'https://github.com/marufhossainkeyas11/remote-control.git';
document.getElementById('ipAddress').innerHTML = ip;
document.getElementById('Dlink').innerHTML = remoteLink;
document.querySelectorAll('.ssi').forEach(item => item.innerHTML = ssi )
document.querySelectorAll('.a').forEach(item=> item.innerHTML += `<span class="dot"></span><svg xmlns="http://www.w3.org/2000/svg" viewBox="0 -960 960 960"><path d="m480-360 160-160H320l160 160Zm0 280q-83 0-156-31.5T197-197q-54-54-85.5-127T80-480q0-83 31.5-156T197-763q54-54 127-85.5T480-880q83 0 156 31.5T763-763q54 54 85.5 127T880-480q0 83-31.5 156T763-197q-54 54-127 85.5T480-80Zm0-80q134 0 227-93t93-227q0-134-93-227t-227-93q-134 0-227 93t-93 227q0 134 93 227t227 93Zm0-320Z" /></svg>`)
document.querySelectorAll('.copy-btn').forEach(item=> item.innerHTML += `<svg xmlns="http://www.w3.org/2000/svg" height=".7rem" viewBox="0 -960 960 960" width=".7rem" fill="#fff"><path d="M360-240q-33 0-56.5-23.5T280-320v-480q0-33 23.5-56.5T360-880h360q33 0 56.5 23.5T800-800v480q0 33-23.5 56.5T720-240H360Zm0-80h360v-480H360v480ZM200-80q-33 0-56.5-23.5T120-160v-560h80v560h440v80H200Zm160-240v-480 480Z" /></svg>`)
document.getElementById('copyIPBtn').addEventListener('click', () => {
if (navigator.clipboard && window.isSecureContext) {
navigator.clipboard.writeText(id)
.then(() => null)
.catch(err => alert("Copy failed: " + err));
} else {
let input = document.createElement("input");
input.value = ip;
document.body.appendChild(input);
input.select();
document.execCommand("copy");
document.body.removeChild(input);}});
document.getElementById('copyLinkBtn').addEventListener('click', () => {
if (navigator.clipboard && window.isSecureContext) {
navigator.clipboard.writeText(remoteLink)
.then(() => null)
.catch(err => alert("Copy failed: " + err));
} else {
let input = document.createElement("input");
input.value = remoteLink;
document.body.appendChild(input);
input.select();
document.execCommand("copy");
document.body.removeChild(input);}});
</script></body></html>
)rawliteral";

const char index_html_find_load[] PROGMEM = R"rawliteral(
<html><head><link rel="icon" href="data:;base64,iVBORw0KGgo="><style>body {margin: 0;background: #fff;display: flex;
align-items: center;justify-content: center;
height: 100vh;}div{width: 30px;height: 30px;border: 4px solid #ccc;
border-top: 4px solid #6666ff;border-radius: 50%;
animation: s 0.8s linear infinite;}@keyframes s {
0% { transform: rotate(0deg);}
100% { transform: rotate(360deg);}}</style><script>
setTimeout(() => {window.location.href = 'http://192.168.4.1/find';}, 3000);
</script></head><body><div></div></body></html>
)rawliteral";

void saveWiFi(const char* ssid, const char* pass) {
  for (int i = 0; i < MAX_LEN; i++) {
    EEPROM.write(SSID_ADDR + i, i < strlen(ssid) ? ssid[i] : 0);
    EEPROM.write(PASS_ADDR + i, i < strlen(pass) ? pass[i] : 0);
  }
  EEPROM.commit();
}
void loadWiFi(char* ssid, char* pass) {
  for (int i = 0; i < MAX_LEN; i++) {
    ssid[i] = EEPROM.read(SSID_ADDR + i);
    pass[i] = EEPROM.read(PASS_ADDR + i);
  }
  ssid[MAX_LEN - 1] = '\0';
  pass[MAX_LEN - 1] = '\0';
}
void runMotorPercent(char motor , bool forward) {
  int in1, in2;

  if (motor == 'R') {
    in1 = IN1;
    in2 = IN2;
  } else if (motor == 'L') {
    in1 = IN3;
    in2 = IN4;
  } else return;
  digitalWrite(in1, forward ? HIGH : LOW);
  digitalWrite(in2, forward ? LOW : HIGH);
}
void setSpeed(int speedPercent) { 
  speedPercent = constrain(speedPercent, 0, 100);
  int pwmValue = map(speedPercent, 0, 100, 0, 1023);
  if ((action.endsWith("-stop") || action == "right" || action == "left") && !once) {
    once=false;
    return;
  }
  analogWrite(ENA, pwmValue);
  analogWrite(ENB, pwmValue);
}
void left() {
  runMotorPercent('L', false); 
  runMotorPercent('R', true); 
  once = true;
  setSpeed(42);
}
void right() {
  runMotorPercent('R', false); 
  runMotorPercent('L', true); 
  once = true;
  setSpeed(42);
}
void forward() {
  runMotorPercent('R', true); 
  runMotorPercent('L', true); 
  setSpeed(speed);
}
void backward() {
  runMotorPercent('R', false); 
  runMotorPercent('L', false); 
  setSpeed(speed);
}
void stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  once = true;
  setSpeed(0);
}

void scanWiFiNetworks() {
  static unsigned long lastScanTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastScanTime >= 1000) {
    lastScanTime = currentTime;
    int numOfNets = WiFi.scanNetworks();    
    int index[numOfNets], order[numOfNets];    
      
    for (int i = 0; i < numOfNets; i++) {    
      index[i] = i;    
      order[i] = WiFi.RSSI(i);    
    }    
      
    for (int i = 0; i < numOfNets - 1; i++) {    
      for (int j = i + 1; j < numOfNets; j++) {    
        if (order[i] < order[j]) {    
          int temp = order[i];    
          order[i] = order[j];    
          order[j] = temp;    
          int tempIdx = index[i];    
          index[i] = index[j];    
          index[j] = tempIdx;    
        }    
      }    
    }    
      
    if (numOfNets == 0) {    
      addListFunction = "";    
      Serial.println("No networks found.");    
    } else {    
      addListFunction = "";    
      Serial.println("Networks found:");    
      for (int i = 0; i < numOfNets; ++i) {    
        addListFunction += "makeList(\"";    
        addListFunction += WiFi.SSID(index[i]);    
        addListFunction += "\", ";    
        addListFunction += WiFi.RSSI(index[i]);    
        addListFunction += ");\n";    
      }    
    }
  }
}
void startWiFiServer() {
  const char ssidN[] = "Find Wi-Fi \xF0\x9F\x94\x8E";
  WiFi.softAP(ssidN);
  server.on("/isConnect", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(204);
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  });
  server.on("/isConnect", HTTP_POST, [](AsyncWebServerRequest *request){
    String action = "";
    if (request->hasParam("action", true)) {
      action = request->getParam("action", true)->value();
    }

    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "all ok");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });
  server.on("/find", HTTP_GET, [](AsyncWebServerRequest *request){
    callF = false;
    String html = String(FPSTR(index_html_find_wifi));
    html.replace("//No_Network_Available", addListFunction);
    request->send(200, "text/html", html);
  });
  server.on("/find-load", HTTP_GET, [](AsyncWebServerRequest *request){
    callF = true;
    String html = String(FPSTR(index_html_find_load));
    request->send(200, "text/html", html);
  });
  server.on("/wifi", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("ssid", true)) {
      ssid = request->getParam("ssid", true)->value();
    }
    if (request->hasParam("pass", true)) {
      password = request->getParam("pass", true)->value();
    }
    saveWiFi(ssid.c_str(), password.c_str());
    WiFi.begin(ssid.c_str(), password.c_str());
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 50) {
      delay(100);
      Serial.print(".");
      attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      request->send(200, "text/plain", "WiFi Connected");
    } else if (WiFi.status() != WL_CONNECTED) {
      request->send(200, "text/plain", "Connection is being processed via the async web server. If the password is correct, Wi-Fi will connect shortly. Please wait...");
    } else {
      request->send(200, "text/plain", "WiFi Lost");
    }
  });
  
  server.on("/copy",  HTTP_GET, [](AsyncWebServerRequest *request){
      String html = String(FPSTR(index_html_copy));
      html.replace("LOCAL_IP", ipStr);
      html.replace("SSID_NAME", ssid);
      request->send(200, "text/html", html);
  });
  
  server.on("/command",  HTTP_POST, [](AsyncWebServerRequest *request){
    if (state == "on") BlinkOnce = true;
    if (request->hasParam("action", true)) {
      action = request->getParam("action", true)->value();
      if  (action.endsWith("-stop")) stop();
      else if (action == "forward")  forward();
      else if (action == "left")     left();
      else if (action == "right")    right();
      else if (action == "backward") backward();
      else   Serial.println(">> Unknown command");
    }
  });
  server.on("/clear", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "WiFi Cleared. Restarting...");
    saveWiFi("","");
    WiFi.disconnect(true);
    delay(1000);
    ESP.restart();
  });
  server.on("/light",  HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("light", true)) {
      state = request->getParam("light", true)->value();
      if (state == "on") {
        digitalWrite(LED_PIN, LOW);
      } else if (state == "off") {
        digitalWrite(LED_PIN, HIGH);
      }
    } 
  });
  server.on("/speed",  HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("speed", true)) {
      String speedStr = request->getParam("speed", true)->value();
      speed = speedStr.toInt();
      if (!action.endsWith("-stop")) setSpeed(speed);
    }
  });
  server.onNotFound([](AsyncWebServerRequest *request){
    if (WiFi.status() != WL_CONNECTED){
      request->redirect("http://192.168.4.1/find-load");
    }else {
      request->redirect("http://192.168.4.1/copy");
    }
  });
  server.begin();
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  IPAddress apIP(192,168,4,1);
  digitalWrite(LED_PIN, HIGH);
  Serial.begin(115200);
  EEPROM.begin(512);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP_STA);
  WiFi.setOutputPower(20.5);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
  dnsServer.start(53, "*", apIP);
  startWiFiServer();
  delay(200);
  if (EEPROM.read(SSID_ADDR) == 0xFF) saveWiFi(ssid.c_str(), password.c_str());

  char loadedSSID[MAX_LEN], loadedPass[MAX_LEN];
  loadWiFi(loadedSSID, loadedPass);
  ssid = loadedSSID;
  password = loadedPass;
  WiFi.begin(loadedSSID, loadedPass);
  while (WiFi.status() != WL_CONNECTED) {
    dnsServer.processNextRequest();
    if (callF == true) scanWiFiNetworks();
    if (BlinkOnce) {
    digitalWrite(LED_BUILTIN, HIGH);
    blinkStart = millis();
    ledOn = true;
    BlinkOnce = false;
  }
  if (ledOn && millis() - blinkStart >= 100) {
    digitalWrite(LED_BUILTIN, LOW);
    ledOn = false;
  }

  }
  WiFi.softAPdisconnect(true);
  delay(200);
  
  const char* ssid = "User Manual  \xF0\x9F\x93\x9C";
  WiFi.softAP(ssid);
  ipStr = WiFi.localIP().toString();
  if (state == "on") digitalWrite(LED_PIN, LOW);
}

void loop() {
  dnsServer.processNextRequest();
  if (BlinkOnce) {
    digitalWrite(LED_BUILTIN, HIGH);
    blinkStart = millis();
    ledOn = true;
    BlinkOnce = false;
  }
  if (ledOn && millis() - blinkStart >= 100) {
    digitalWrite(LED_BUILTIN, LOW);
    ledOn = false;
  }

  if (millis() - prev >= 1000) {
    prev = millis();
    if (WiFi.status() != WL_CONNECTED) ESP.restart();
  }
}

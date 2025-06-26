const html = document.documentElement,
  box = document.querySelector('.box'),
  fullScreenBox = document.querySelector('.fullScreenBox'),
  dragBox = document.querySelector('.dragBox'),
  manu = document.querySelector('.manu'),
  button = document.querySelectorAll('.button'),
  h3 = document.querySelector('h3'),
  icon = document.querySelector('#icon'),
  metaTheme = document.querySelector('meta[name="theme-color"]'),
  radios = document.querySelectorAll('[name="theme"]'),
  slider = document.getElementById('speedSlider'),
  label = document.querySelector('.slider-label'),
  overlay = document.querySelector('[data-overlay]'),
  checkboxD = document.querySelector(".toggleD"),
  checkboxB = document.querySelector(".toggleB"),
  dragI0 = dragBox.querySelectorAll('svg')[0],
  dragI1 = dragBox.querySelectorAll('svg')[1],
  innerH = h3.innerHTML,
  originalHTMLfull = fullScreenBox.innerHTML,
  innerI = icon.innerHTML,
  setS = (e, v) => sessionStorage.setItem(e, v),
  getS = (e) => sessionStorage.getItem(e),
  getB = (e) => JSON.parse(getS(e)),
  touchE = ['touchstart', onTouchStart, 'touchmove', onTouchMove, 'touchend', onTouchEnd],
  buttonE = ['touchstart', pressureStart, 'touchmove', pressureMove, 'touchend', pressureEnd];
let offsetX, offsetY, isDragging = false,
  isClickedDrag = false,
  outT = false,
  last = null,
  longPressTimer, topL, originalClass, nowS;
const BASE_URL = (prompt = prompt('localIP?')) ? prompt : "http://192.168.4.1";

function dragEffectAdd()
{
  removeButton();
  dragI1.style.display = 'block';
  dragI0.style.display = 'none';
  box.classList.add('dragging');
  isDragging = true;
}

function dragEffectRemove()
{
  if (!checkboxD.checked) addButton();
  dragI0.style.display = 'block';
  dragI1.style.display = 'none';
  box.classList.remove('dragging');
  isDragging = false;
}

function drag()
{
  if (!isClickedDrag)(dragEffectAdd(), isClickedDrag = true);
  else if (isClickedDrag)(dragEffectRemove(), isClickedDrag = false);
}

function onTouchStart(e)
{
  if ((e.target.tagName.toLowerCase() === 'button' || e.target.closest('.speed') || e.target.closest('.gear-control')) && !isClickedDrag) return;
  const touch = e.touches[0],
    rect = box.getBoundingClientRect();
  offsetY = touch.clientY - rect.top;
  offsetX = touch.clientX - rect.left;
  longPressTimer = setTimeout(() => { dragEffectAdd(); }, 800);
}

function onTouchMove(e)
{
  if (!isDragging) return;
  e.preventDefault();
  const touch = e.touches[0],
    maxX = document.documentElement.clientWidth - box.offsetWidth,
    maxY = document.documentElement.clientHeight - box.offsetHeight;
  box.style.left = Math.max(0, Math.min(touch.clientX - offsetX, maxX)) + 'px';
  box.style.top = Math.max(0, Math.min(touch.clientY - offsetY, maxY)) + 'px';
  topL = Math.max(document.querySelector('h1').getBoundingClientRect().bottom + 1, Math.min(touch.clientY - offsetY, maxY)) + 'px';
}

function onTouchEnd(e)
{
  const boxRect = box.getBoundingClientRect(),
    boxCenter = boxRect.left + boxRect.width / 2,
    screenCenter = window.innerWidth / 2;
  if (boxCenter < screenCenter) {
    (icon.classList.contains('rotated')) ? (box.style.left = "", box.style.right = "0", setS('site', false)) : (box.style.left = "0", box.style.right = "",
      setS('site', true));
  } else {
    box.style.left = "";
    box.style.right = "0";
    setS('site', false);
  }
  if (!getB('site') && !['lefBox', 'lefBoxF'].some(cls => box.classList.contains(cls))) {
    box.classList.add('lefBox');
    ['lefBoxF', 'rigBox', 'rigBoxF'].forEach(cls => box.classList.remove(cls));
    setS('class', 'lefBox');
  } else if (getB('site') && (!['rigBox', 'rigBoxF'].some(cls => box.classList.contains(cls)))) {
    box.classList.add('rigBox');
    ['lefBox', 'lefBoxF', 'rigBoxF'].forEach(cls => box.classList.remove(cls));
    setS('class', 'rigBox');
  }
  box.style.top = topL;
  clearTimeout(longPressTimer);
  setS('left', box.style.left);
  setS('right', box.style.right);
  setS('top', box.style.top);
  dragEffectRemove();
  if (!e.target.closest('.dragBox')) isClickedDrag = false;
}

function addDrag() {
  box.classList.add('normalScreen');
  box.classList?.remove('fullScreen');
  for (let i = 0; i < touchE.length; i += 2) {
    box.addEventListener(touchE[i], touchE[i + 1]);
  }
}

function removeDrag() {
  box.classList.add('fullScreen');
  box.classList?.remove('normalScreen');
  for (let i = 0; i < touchE.length; i += 2) {
    box.removeEventListener(touchE[i], touchE[i + 1]);
  }
}

function addButton() {
  button.forEach((el) => {
    for (let i = 0; i < buttonE.length; i += 2) {
      el.addEventListener(buttonE[i], buttonE[i + 1]);
    }
  });
}

function removeButton() {
  button.forEach((el) => {
    for (let i = 0; i < buttonE.length; i += 2) {
      el.removeEventListener(buttonE[i], buttonE[i + 1]);
    }
  });
}

function Fullscreen() {
  if (!document.fullscreenElement) {
    originalClass = [...box.classList].filter(e => !['box', 'normalScreen', 'dragging']?.includes(e));
    nowS = getB('site');
    box.classList.remove(originalClass[0])
    box.classList.add('lefBoxF');
    setS('site', false);
    html.requestFullscreen();
    removeDrag();
    dragBox.style.display = 'none';
    h3.innerHTML = "";
    fullScreenBox.innerHTML =
      `<svg xmlns="http://www.w3.org/2000/svg" height="24px" viewBox="0 -960 960 960" width="24px" fill="#erefe3"><path d="M240-120v-120H120v-80h200v200h-80Zm400 0v-200h200v80H720v120h-80ZM120-640v-80h120v-120h80v200H120Zm520 0v-200h80v120h120v80H640Z"/></svg>`;
    if (screen.orientation && screen.orientation.lock) {
      screen.orientation.lock('landscape');
    }
    closeMemu();
  } else if (document.fullscreenElement) exitFullScreen();
}

function exitFullScreen() {
  box.classList.replace('lefBoxF', originalClass[0]);
  (nowS !== false) ? setS('site', nowS): null;
  dragBox.style.display = 'block';
  h3.innerHTML = innerH;
  fullScreenBox.innerHTML = originalHTMLfull;
  originalClass = [];
  nowS = null;
  closeMemu();
  addDrag();
  if (document.fullscreenElement) {
    document.exitFullscreen();
  }
}

function showMemu() {
  setTimeout(() => {
    icon.innerHTML =
      '<svg xmlns="http://www.w3.org/2000/svg" height="24px" style="filter: drop-shadow(1px 1px 1.8px #00000080);" viewBox="0 -960 960 960" width="24px" fill="#1f1f1f"><path d="m256-200-56-56 224-224-224-224 56-56 224 224 224-224 56 56-224 224 224 224-56 56-224-224-224 224Z"/></svg>';
    icon.classList.add('rotated');
    manu.classList.add('manuShow');
    if (!document.fullscreenElement) {
      overlay.classList.add('active');
    }
  }, 300);
  
}

function closeMemu() {
  document.querySelector('#toggle').checked = false;
  setTimeout(() => {
    icon.innerHTML = innerI;
    icon.classList.remove('rotated');
    manu.classList.remove('manuShow');
    overlay.classList?.remove('active');
  }, 300);
}

function speedLimitFL() {
  box.classList?.remove('rigBox', 'rigBoxF');
  if (box.classList.contains('lefBox')) {
    box.classList.replace('lefBox', 'lefBoxF');
  }
  else if (box.classList.contains('lefBoxF')) {
    box.classList.replace('lefBoxF', 'lefBox');
  }
}

function speedLimitFR() {
  box.classList?.remove('lefBox', 'lefBoxF');
  if (box.classList.contains('rigBox')) {
    box.classList.replace('rigBox', 'rigBoxF');
  }
  else if (box.classList.contains('rigBoxF')) {
    box.classList.replace('rigBoxF', 'rigBox');
  }
}

function sl() {
  if (getB('site') === false && !checkboxD.checked) {
    speedLimitFL();
  } else if (getB('site') === true && !checkboxD.checked) {
    speedLimitFR();
  }
}

function vibrator(duration = 50) {
  if (navigator.vibrate) {
    navigator.vibrate(duration);
  }
}

function sendXhrPost(url, data, onSuccess, onError) {
  const xhr = new XMLHttpRequest();
  xhr.open("POST", url, true);
  xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
  
  xhr.onload = function() {
    if (xhr.status === 200) {
      if (onSuccess) onSuccess(xhr.responseText);
    } else if (xhr.status === 404) {
      //alert("404 Not Found");
      if (onError) onError(xhr.status);
    } else {
      //alert("Error code: " + xhr.status);
      if (onError) onError(xhr.status);
    }
  };
  
  xhr.onerror = function() {
    //alert("Request failed");
    if (onError) onError(null);
  };
  
  xhr.send(data);
}

function pressureStart(e) {
  const found = e.currentTarget.innerHTML.toLowerCase();
  if (!found) return;
  if (!BASE_URL) location.reload();
  const url = BASE_URL + "/command";
  const data = "action=" + encodeURIComponent(found);
  sendXhrPost(url, data,
    response => console.log("Success:", response),
    error => null
  );
  e.currentTarget.style = 'transform: scale(0.97); text-shadow:none; box-shadow: none;';
  vibrator(50);
}

function pressureEnd(e) {
  if (outT) {
    addButton();
    outT = false;
    return;
  };
  const found = e.currentTarget.innerHTML.toLowerCase();
  if (!found) return;
  const url = BASE_URL + "/command";
  const data = "action=" + encodeURIComponent(found + "-stop");
  sendXhrPost(url, data,
    response => console.log("Success:", response),
    error => null
  );
  e.currentTarget.removeAttribute('style');
  vibrator(50);
}

function pressureMove(e) {
  const touch = e.touches[0];
  const rect = e.currentTarget.getBoundingClientRect();
  const insideX = touch.clientX >= rect.left && touch.clientX <= rect.right;
  const insideY = touch.clientY >= rect.top && touch.clientY <= rect.bottom;
  
  if (!insideX || !insideY) {
    pressureEnd(e);
    removeButton();
    outT = true;
  } else {
    outT = false;
  }
}

function changeSpeed(amount) {
  let value = parseInt(slider.value);
  value = Math.min(100, Math.max(0, value + amount));
  slider.value = value;
  updateLabel(null, 60);
}

function updateMeta() {
  const color = getComputedStyle(html)
    .getPropertyValue('--bgC')
    .trim();
  metaTheme.setAttribute('content', color);
}

function disconnect() {
  const state = (event.target.checked) ? "off" : "on";
  setS("togglD", state);
  (state === "on") ? addButton(): (removeButton(), (box.classList.contains('lefBoxF')) ? box.classList.replace('lefBoxF', 'lefBox') : null);
}

function light() {
  const state = (event.target.checked) ? "on" : "off";
  setS("togglB", state);
    const url = BASE_URL + "/light";
    const data = "light=" + encodeURIComponent(state);
  sendXhrPost(url, data,
    response => console.log("Success:", response),
    error => null
  );
}

function updateLabel(a, v = 8) {
  vibrator(v)
  const val = +slider.value;
  label.textContent = val + '%';
  setS('rValue', val);
  if (!BASE_URL) location.reload();
  const url = BASE_URL + "/speed";
  const data = "speed=" + encodeURIComponent(val);
  sendXhrPost(url, data,
    response => console.log("Success:", response),
    error => null
  );
  
}

icon.addEventListener('click', () => icon.classList.contains('rotated') ? closeMemu() : showMemu());
slider.addEventListener('input', updateLabel);

document.addEventListener('fullscreenchange', () => {
  if (!document.fullscreenElement && originalClass[0] !== undefined && nowS !== null) {
    exitFullScreen();
  }
});
radios.forEach(radio => {
  radio.addEventListener('click', () => {
    if (radio === last) {
      radio.checked = false;
      html.removeAttribute('data-theme');
      sessionStorage.removeItem('theme');
      last = null;
    } else {
      last = radio;
      html.dataset.theme = radio.value;
      setS('theme', radio.value);
    }
    updateMeta();
  });
});

window.onload = () => {
  checkboxD.checked = getS("togglD") === "off";
  checkboxB.checked = getS("togglB") === "off";
  if (getS("togglD") === "on") {
    addButton();
  } else {
    removeButton();
  }
  if (!getB('site'))
  {
    setS('site', false);
  }
  if (getS('class'))
  {
    box.classList.remove('lefBox');
    box.classList.add(getS('class'));
  }
  if (getS('theme'))
  {
    html.dataset.theme = getS('theme');
    last = document.querySelector(`[name="theme"][value="${getS('theme')}"]`);
    if (last) last.checked = true;
    updateMeta();
  }
  if (getS('top') || getS('right') || getS('left'))
  {
    box.style.cssText = `left:${getS('left')}; right:${getS('right')}; top:${getS('top')};`
  }
  if (getS('rValue'))
  {
    let a = sessionStorage.getItem('rValue');
    label.textContent = a + '%';
    slider.value = a;
  }
  button.forEach((item) => {
    item.addEventListener('touchend', (e) => {
      if (!outT) return;
      console.log('end');
      button.forEach((item) => {
        item.addEventListener('touchend', (e) => {
          if (!outT) return;
          console.log('end');
          pressureEnd(e);
        });
      });
      pressureEnd(e);
    });
  });
  updateMeta();
  addDrag();
  dragEffectRemove();
  sendXhrPost(
    BASE_URL + "/light",
    `light=${encodeURIComponent(getS("togglB") ?? "off")}`,
    response => console.log("Success:", response),
    error => null
  );
};
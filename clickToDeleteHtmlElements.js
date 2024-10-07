// To use this conveniently, add as a bookmark with destination of: javascript:

(function() {

var customDeleter;
var lastHoveredElement;
var mouseClientX;
var mouseClientY;

function updateDeleter() {
	if (customDeleter && mouseClientX && mouseClientY) {
		let topElements = document.elementsFromPoint(mouseClientX, mouseClientY);
		let topElement = null;
		for (element of topElements) {
			if (element != customDeleter && element != document.body.parentElement /* root */ && element != document.body) {
				topElement = element;
				break;
			}
		}
		if (topElement && lastHoveredElement !== topElement) {
			topElementPosition = topElement.getBoundingClientRect();
			customDeleter.style.left = topElementPosition.left + window.scrollX + 'px';
			customDeleter.style.top = topElementPosition.top + window.scrollY + 'px';
			customDeleter.style.width = Math.max(topElementPosition.width, 5) + 'px';
			customDeleter.style.height = Math.max(topElementPosition.height, 5) + 'px';
			let textSize = Math.max(
				Math.min(topElementPosition.height / 1.1, topElementPosition.width / 9) /* CLICK TO DELETE (1 row) */,
				Math.min(topElementPosition.height / 2.2, topElementPosition.width / 6) /* CLICK TO + DELETE (2 rows) */,
				Math.min(topElementPosition.height / 3.3, topElementPosition.width / 4) /* CLICK + TO + DELETE (3 rows) */);
			textSize = Math.max(textSize, 5);
			customDeleter.style.font = textSize + 'px Arial';
			lastHoveredElement = topElement;
		}
	}
}

customDeleter = document.createElement('div');
customDeleter.innerHTML = 'CLICK TO DELETE';
customDeleter.style.position = 'absolute';
customDeleter.style.background = '#222'; /* dark gray */
customDeleter.style.opacity = 0.7;
customDeleter.style.color = '#fff'; /* white text */
customDeleter.style.textAlign = 'center';
customDeleter.style.font = '20px Arial';
customDeleter.style.fontWeight = 'bold';
customDeleter.style.zIndex = '10000000'; /* put in front of everything */
customDeleter.addEventListener('click', function() {
	if (lastHoveredElement) {
		lastHoveredElement.parentNode.removeChild(lastHoveredElement);
		lastHoveredElement = null;
		customDeleter.style.width = '0px';
		customDeleter.style.height = '0px';
		updateDeleter();
	}
});
document.body.appendChild(customDeleter);

var oldonmousemove = document.onmousemove;
document.onmousemove = function(mouseMoveEvent) {
	mouseClientX = mouseMoveEvent.clientX;
	mouseClientY = mouseMoveEvent.clientY;
	updateDeleter();
};

var oldonkeyup = document.onkeyup;
document.onkeyup = function(keyboardEvent) {
	if (keyboardEvent.key === 'Escape') {
		document.onmousemove = oldonmousemove;
		document.onkeyup = oldonkeyup;
		customDeleter?.parentNode.removeChild(customDeleter);
	}
};

})();
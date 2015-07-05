var lastTooltip;

function showTooltip(strMainImage,strTitle,strShowsUserIcons) {
	if(strShowsUserIcons === 'showIcons') {
		_showTooltip(strMainImage,strTitle);
	}
}

function _showTooltip(strMainImage,strTitle,strText,strHelpText,strHelpImage) {
	if (!lastTooltip) {
		var newDiv = document.createElement("div");
		newDiv.className = "toolTip";
		newDiv.style.visibility='hidden';
		
		// if(strTitle) {
		// 	var title = document.createElement("span"); 
		// 	title.appendChild(document.createTextNode(strTitle));
		// 	newDiv.appendChild(title);
		// }         
		
		if (strMainImage) {
			var mainImg = new Image();
			mainImg.src = strMainImage;  
			newDiv.appendChild(mainImg);
		}
		
		if(strText) {
			newDiv.appendChild(document.createTextNode(strText));
		}
		
		
		if (strHelpText) {
			var horLine = document.createElement("hr"); 
			horLine.style.width='96%';
			horLine.style.clear='both';    
			
			var helpDiv = document.createElement("div");   
			helpDiv.style.styleFloat='left';
			helpDiv.style.cssFloat='left';   
			helpDiv.style.clear='both';
			helpDiv.style.paddingLeft='6px';     
			helpDiv.style.height='24px';     
			
			if (strHelpImage) {
				var helpImg = new Image();
				helpImg.src = strHelpImage;
				helpImg.style.marginRight='8px';
				helpImg.style.verticalAlign='middle';
				helpDiv.appendChild(helpImg);
			}
			
			var helpText = document.createTextNode(strHelpText);                           
			helpDiv.appendChild(helpText);  
			newDiv.appendChild(horLine);
			newDiv.appendChild(helpDiv);                         
		}                    
		
		lastTooltip=newDiv;    
		if (document.addEventListener) document.addEventListener("mousemove",moveTooltip, true);
		if (document.attachEvent) document.attachEvent("onmousemove",moveTooltip);  
		
		var bodyRef = document.getElementsByTagName("body").item(0);
		bodyRef.appendChild(newDiv);                    
	}                                 
}


function moveTooltip(e) {
	if (lastTooltip) {
		if (document.all)
			e = event;
		if (e.target)
			sourceEl = e.target;
		else if (e.srcElement)
			sourceEl = e.srcElement;
		
		var coors=findPos(sourceEl);
		var positionLeft = e.clientX+20;            
		var positionTop  = coors[1] - sourceEl.clientHeight - 40;
		
		lastTooltip.style.top=positionTop+'px';
		lastTooltip.style.left=positionLeft+'px';
		lastTooltip.style.visibility='visible';
	}
}

function hideTooltip() {              
	var bodyRef = document.getElementsByTagName("body").item(0);
	if (lastTooltip) {
		bodyRef.removeChild(lastTooltip);
	}
	lastTooltip=null;
}

function findPos(obj) {
	var curleft = curtop = 0;
	if (obj.offsetParent) {
		curleft = obj.offsetLeft
		curtop = obj.offsetTop
		while (obj == obj.offsetParent) {
			curleft += obj.offsetLeft
			curtop += obj.offsetTop
		}
	}
	return [curleft,curtop];
}

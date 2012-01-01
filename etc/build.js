function checkstatus( url )
{
	if(window.XMLHttpRequest && !(window.ActiveXObject)) {
	try {req = new XMLHttpRequest(); } catch(e) {req = false;  }
	} else if(window.ActiveXObject) {
		try {req = new ActiveXObject("Msxml2.XMLHTTP");} catch(e) {
			try {req = new ActiveXObject("Microsoft.XMLHTTP");} catch(e) {req = false;}	}
	}
	if (req !== false) {
	 req.onreadystatechange = function() {
		 if ((req.readyState == 4) && (req.status == 200)) {
		 eval( req.responseText );
		 if(ping1==1) 
		 {
		    setTimeout('buildping();', 1000*2);
		 }
		}
	 };
	 req.open("GET", url);
	 req.send(null);
	}
}
let auto = false
let off = false


function sendLightAction(name, checked){
	//simple jQuery call to the server
	$.ajax({
	  type: "POST",
	  url: "/",
	  data: { 
				light: name,
				value: checked ? (1) : (0)
			}
	}).done(function(o) {
		let message = "updated light " + name + " with value " + checked
	   console.log(message)
	});
}

setInterval(function(){ 
		console.log('checking auto', document.getElementsByClassName("auto")[0].checked)
		if(document.getElementsByClassName("auto")[0].checked){
			sendLightAction("auto", true)
		}
	}, 
5500);


//event handlers
function toggle(event){
	sendLightAction(event.value, event.checked)
}

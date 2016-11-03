function validateEmail(form) {

	 //Validating the email field
	 var re = /^(([^<>()[\]\\.,;:\s@\"]+(\.[^<>()[\]\\.,;:\s@\"]+)*)|(\".+\"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/
	if (! form.Email.value.match(re)) {
		alert("Invalid email address");
		form.Email.focus();
		form.Email.select();
		return (false);
	}
	return(true);
}


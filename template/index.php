<html>

<head>
<title>Template Database</title>
</head>

<body bgcolor="FFFFFF" onload="document.login.login_name.focus()">

<form enctype="multipart/form-data"
	name="login" method=POST action="/cgi-bin/post_login?template:template">
<input type="hidden" name="filename" value="index.php">

<table	border="0"
cellpadding="0"
cellspacing="0"
width="540"
align="center"
valign="top">
<tr><td align="center">
	<img src="images/logo_appaserver.jpg" border="0">
	<tr>	<td align="center">
			<font color="blue">
			<h1>Template Database</h1>
	<tr><td><hr>
	<tr><td>
	<table align=center>
<?php
$invalid_login_yn=$_GET['invalid_login_yn'];
if ( $invalid_login_yn == 'y' )
{
	print "
	<tr><td colspan=99 align=center>
			<font color=blue>
			Invalid login. Please try again.</td>
	";
}
?>
	<tr>	<td align="right">
			<font color="blue">
				Login name</td>
		<td aligh="left">
			<input type="text" name="login_name" size=21 value="">
	<tr>	<td align="right">
			<font color="blue">
				Password</td>
		<td aligh="left">
			<input type="password" name="password" size=20 value="">
	</td>
	</table>
	<tr>	<td align="center">
			<font color="blue">
			<input type="submit" value="Login">
</table>
</form>
</body>
</HTML>

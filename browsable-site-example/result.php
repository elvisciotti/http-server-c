<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>Dati di <? print $nome ?> <? print $cognome ?></title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>

<body>
<center>
<p><strong>VARIABILI RICEVUTE:</strong><br>
nome: <? print $nome ?>
<br>
cognome: <? print $cognome ?>
<br>
matricola: <? print $matricola ?><br>
<strong><br>
VARIABILI PHP HTTP</strong><br>
<font size="2">HTTP_USER_AGENT: <? print $HTTP_USER_AGENT ?><br>
REMOTE_ADDR: <? print $REMOTE_ADDR ?><br>
HTTP_USER_LANGUAGE: <? print $HTTP_USER_LANGUAGE ?></font></p>
<hr>
</center>
<div align="left">
  <p>Pagina .PHP=&gt;<br>
  -  variabili predefinite php (<font size="2">REMOTE_ADDR</font>, <font size="2">REMOTE_ADDR</font>...)  rimpiazzate con i valori relativi letti dalle informazioni HTTP passate da browser (User-Agent:, Host:...).<br>
  -  se pagina richiesta con POST:<br>
    <font color="#0000FF" size="2" face="Courier New, Courier, mono">POST /result.php HTTP/1.1<br>
    [...]<br>
  Content-length: 51</font></p>
  <p><font color="#0000FF" size="2" face="Courier New, Courier, mono">nome=Luigi&amp;cognome=Mattei+Gentili&amp;matricola=200244&amp;Submit=Invia<br>
  </font>Se nella pagina php fra i tag &lt;? ?&gt; &egrave; presente una print della variabile $matricola, il parser sostituisce tutte le occorrenze di $nome con 200244 (che ha ricevuto in post dal browser).<br>
  I + vengono convertiti in spazi (come nel cognome).</p>
  <p><font size="2" face="Courier New, Courier, mono"><strong>CODICE DI QUESTA PAGINA:</strong><br>
&lt;html&gt;<br>
&lt; head&gt;<br>
&lt;
title&gt;Dati di <font color="#FF0000">&lt;? print $nome ?&gt;</font> <font color="#FF0000">&lt;? print $cognome ?&gt;</font>&lt;/title&gt;<br>
&lt;
meta http-equiv=&quot;Content-Type&quot; content=&quot;text/html; charset=iso-8859-1&quot;&gt;<br>
&lt; /head&gt;<br>
  </font><font size="2" face="Courier New, Courier, mono">&lt;body&gt;<br>
&lt; center&gt;<br>
&lt;
  p&gt;&lt;b&gt;<strong>VARIABILI RICEVUTE</strong>:&lt;/b&gt;&lt;br&gt;<br>
  nome: <font color="#FF0000">&lt;? print $nome ?&gt;</font>&lt; br&gt;<br>
  cognome: <font color="#FF0000">&lt;? print $cognome ?&gt;</font>&lt; br&gt;<br>
  matricola: <font color="#FF0000">&lt;? print $matricola ?&gt;</font>&lt;br&gt;<br>
&lt;
  strong&gt;&lt;br&gt;  <strong>VARIABILI PHP HTTP</strong>&lt;/strong&gt;&lt;br&gt;<br>
&lt;
  font size=&quot;2&quot;&gt;HTTP_USER_AGENT: <font color="#FF0000">&lt;? print $HTTP_USER_AGENT ?&gt;</font>&lt;br&gt;<br>
  REMOTE_ADDR: <font color="#FF0000">&lt;? print $REMOTE_ADDR ?&gt;</font>&lt;br&gt;<br>
  HTTP_USER_LANGUAGE: <font color="#FF0000">&lt;? print $HTTP_USER_LANGUAGE ?&gt;</font>&lt;/font&gt;&lt;/p&gt;<br>
&lt; hr&gt;<br>
[...]<br>
&lt;/body&gt;<br>
&lt; /html&gt;</font><br>
</p>
</div>
</body>
</html>
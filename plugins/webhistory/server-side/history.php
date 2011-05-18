<?php
/*
        qutIM WebHistory
	server-side PHP script

	Copyright (c) 2009 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

  $mysql_server = "localhost";
  $mysql_user = "qutim";
  $mysql_pass = "qutimpass";
  $mysql_db = "qutim_history";

  mysql_connect($mysql_server, $mysql_user, $mysql_pass) || die("DB connection error");
  mysql_select_db($mysql_db) || die("DB selection error");
  mysql_query("set names utf8");

  header("Content-Type: text/plain; charset=UTF-8");

  if ($_POST) {
    $vars = array("protocol", "account", "direction", "itemname");
    foreach ($vars as $var) if (!isset($_POST[$var])) die("Variable '$var' is absent");
    $ts = isset($_POST['ts']) ? $_POST['ts'] : time();
    $message = $_POST['message'];
    $dbq = "insert into `history` set `ts`='".mysql_escape_string($ts)."'";
    $dbq .= ", `remote_addr`='".mysql_escape_string($_SERVER['REMOTE_ADDR'])."'";
    $dbq .= ", `remote_user`='".mysql_escape_string($_SERVER['REMOTE_USER'])."'";
    $dbq .= ", `message`='".mysql_escape_string($message)."'";
    foreach ($vars as $var) $dbq .= ", `$var`='".mysql_escape_string($_POST[$var])."'";
    if (mysql_query($dbq)) echo "OK";
  }
  elseif ($_GET['ts']) {
    $history_array = array();    
    $dbq = "select * from `history` where `remote_user`='".mysql_escape_string($_SERVER['REMOTE_USER'])."' and `ts`>{$_GET['ts']} order by `protocol`,`account`,`itemname`,`ts`";
    $dbrs = mysql_query($dbq);
    while ($dbr = mysql_fetch_array($dbrs)) {
      $history_array[] = array($dbr['ts'], $dbr['protocol'], $dbr['account'], $dbr['itemname'], $dbr['direction'], $dbr['message']);
    }
    echo php2js($history_array);
  }
  elseif ($_GET['test']) {
    echo "qutIM WebHistory server-side script";
  }

function php2js($a=false)
{
  if (is_null($a)) return 'null';
  if ($a === false) return 'false';
  if ($a === true) return 'true';
  if (is_scalar($a))
  {
    if (is_float($a))
    {
      $a = str_replace(",", ".", strval($a));
    }
    static $jsonReplaces = array(array("\\", "/", "\n", "\t", "\r", "\b", "\f", '"'),
    array('\\\\', '\\/', '\\n', '\\t', '\\r', '\\b', '\\f', '\"'));
    return '"' . str_replace($jsonReplaces[0], $jsonReplaces[1], $a) . '"';
  }
  $isList = true;
  for ($i = 0, reset($a); $i < count($a); $i++, next($a))
  {
    if (key($a) !== $i)
    {
      $isList = false;
      break;
    }
  }
  $result = array();
  if ($isList)
  {
    foreach ($a as $v) $result[] = php2js($v);
    return '[' . join(',', $result) . ']';
  }
  else
  {
    foreach ($a as $k => $v) $result[] = php2js($k).': '.php2js($v);
    return '{' . join(',', $result) . '}';
  }
}

?>

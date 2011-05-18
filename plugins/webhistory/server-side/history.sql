
CREATE TABLE `history` (
  `id` int(14) NOT NULL auto_increment,
  `ts` int(11) default NULL,
  `protocol` varchar(32) default NULL,
  `account` varchar(64) default NULL,
  `itemname` varchar(128) default NULL,
  `direction` tinyint(1) default '0',
  `message` text,
  `remote_user` varchar(32) default NULL,
  `remote_addr` varchar(15) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM


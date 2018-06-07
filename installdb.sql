USE `mysql`;

DROP FUNCTION IF EXISTS `cassandra`;
CREATE FUNCTION `cassandra` RETURNS STRING SONAME 'lib_mysqludf_cassandra.so';

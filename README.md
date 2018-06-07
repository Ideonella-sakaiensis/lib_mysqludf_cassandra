lib_mysqludf_cassandra
======================
Provides UDF commands to access Cassandra from Mysql/MariaDB.

[English](README.md) | [繁體中文](README.hant.md)


#### Table of Contents
* [Synopsis](#synopsis)
* [System Requirements](#system-requirements)
* [Compilation and Install Plugin Library](#compilation-and-install-plugin-library)
    * [Compilation Argumnets](#compilation-argumnets)
    * [Compilation Variable](#compilation-variable)
* [Install and Uninstall UDF](#install-and-uninstall-udf)
* [Usage](#usage)
* [TODO](#todo)
* [Copyright and License](#copyright-and-license)
* [See Also](#see-also)


Synopsis
--------
![Alt text](https://g.gravizo.com/source/figure01?https%3A%2F%2Fraw.githubusercontent.com%2FIdeonella-sakaiensis%2Flib_mysqludf_cassandra%2Fmaster%2FREADME.md?1)
<details>
<summary></summary>
figure01
  digraph G {

    rankdir = "LR";
    size ="8,8";
    edge [
        fontname = "Consolas"
        fontsize = 10
    ];
    MariaDB [
        label = "MariaDB\n(presistence)"
        shape = "box"
    ];
    Cassandra [
        label = "Cassandra\n(cached)"
        shape = "box"
    ];
    edge [
        fontcolor = "blue"
        color = "blue"
    ];
    writer;
    writer:e -> MariaDB [
        label="INSERT\nUPDATE\nDELETE"
    ];
    MariaDB -> Cassandra [
        label = "INSERT\nUPDATE\nDELETE"
    ];
    edge [
        fontcolor = "red"
        color = "red"
    ];
    node [
        fontname = "Consolas"
        fontsize = 10
        penwidth = 0.5
        color    = "gray"
        shape = "record"
        style = "rounded"
    ];
    MariaDB_Data [
      label = <<TABLE border="0" cellspacing="0" cellborder="1"><TR><TD COLSPAN="2">MariaDB data</TD></TR><TR><TD>item</TD><TD>qty</TD></TR><TR><TD>shoes</TD><TD>35</TD></TR><TR><TD>books</TD><TD>158</TD></TR></TABLE>>
    ];
    {
      rank = "same";
      MariaDB:n -> MariaDB_Data:s;
    }
    Cassandra_Data [
      label = <<TABLE border="0" cellspacing="0" cellborder="1"><TR><TD COLSPAN="2">Cassandra data</TD></TR><TR><TD>item</TD><TD>qty</TD></TR><TR><TD>shoes</TD><TD>35</TD></TR><TR><TD>books</TD><TD>158</TD></TR></TABLE>>
    ];
    {
      rank = "same";
      Cassandra:n -> Cassandra_Data:s;
    }
  }
figure01
</details>


[Back to TOC](#table-of-contents)


System Requirements
-------------------
* Architectures: Linux 64-bit(x64)
* Compilers: GCC 4.1.2+
* MariaDB 5.5+
* Cassandra 3.9+
* Dependencies:
    * MariaDB development library 5.5+
    * libuv 1.20.3+
    * datastax/cpp-driver 2.9.0+
    * cJSON 1.6+

[Back to TOC](#table-of-contents)


Compilation and Install Plugin Library
--------------------------------------
Installing compilation tools
> CentOS
> ```bash
> # install tools
> $ yum install -y make wget gcc automake cmake gcc-c++ libtool git
>
> # install dependencies
> $ yum install -y openssl openssl-devel
>
> # install mariadb development tool
> $ yum install -y mariadb-devel
> ```

To compile the plugin library just simply type `make` and `make install`.
```bash
$ make

# install plugin library to plugin directory
$ make install

# install UDF to Mysql/MariaDB server
$ make installdb
```
> **NOTE**: If the Mysql/MariaDB is an earlier version or installed from source code, the default include path might be invalid; use `` make INCLUDE_PATH=`mysql_config --variable=pkgincludedir` `` to assign `INCLUDE_PATH` variable for compilation.


#### Compilation Argumnets
* `install`

  Install the plugin library to Mysql plugin directory.

* `installdb`

  Install UDFs to Mysql/MariaDB server.

* `uninstalldb`

  Uninstall UDFs from Mysql/MariaDB server.

* `clean`

  Clear the compiled files and resources.

* `distclean`

  Like the `clean` and also remove the dependencies resources.

#### Compilation Variable
The compilation variable can be use in `make`:
* `LIBUV_MODULE_VER`

  The [libuv](https://dist.libuv.org/) version to be compiled. If it is not specified, the default value is `1.20.3`.

* `CPP_DRIVER_MODULE_VER`

  The [datastax/cpp-driver](https://github.com/datastax/cpp-driver) version to be compiled. If it is not specified, the default value is `2.9.0`.

* `CJSON_MODULE_VER`

  The [cJSON](https://github.com/DaveGamble/cJSON) version to be compiled. If it is not specified, the default value is `1.6.0`

* `INCLUDE_PATH`

  The MariaDB or Mysql C header path. If it is not specified, the default will be the Mysql variable `pkgincludedir`. The value can be displayed by executing the following command:
  ``` bash
  $ echo `mysql_config --variable=pkgincludedir`/server
  ```

* `PLUGIN_PATH`

  The MariaDB or Mysql plugin path. The value can be obtained via running the sql statement `SHOW VARIABLES LIKE '%plugin_dir%';` in MariaDB/Mysql server. If it is not specified, the default will be Mysql variable `plugindir`. The value can be displayed by executing the following command:
  ``` bash
  $ mysql_config --plugindir
  ```

example:
```bash
# specify the plugin install path with /opt/mysql/plugin
$ make PLUGIN_PATH=/opt/mysql/plugin
$ make install
```
[Back to TOC](#table-of-contents)


Install and Uninstall UDF
-------------------------
To install UDF from `make`:

```bash
$ make installdb
```

> or executing the following sql statement on Mysql/MariaDB server:
> 
> ```sql
> mysql>  CREATE FUNCTION `cassandra` RETURNS STRING SONAME 'lib_mysqludf_cassandra.so';
> ```

To uninstall UDF with `make uninstalldb` -or- executing the following sql statement on Mysql/MariaDB server:

```sql
mysql>  DROP FUNCTION IF EXISTS `cassandra`;
```

[Back to TOC](#table-of-contents)


Usage
-----
### `cassandra`(*$hosts*,  *$query_string*)

Call a Cassandra command by specified `$hosts`, `$command`, and individual arguments.

* **$hosts** - represent the Cassandra clusters to be connected, the value is a string, like `"127.0.0.1"` or `"192.168.2.15,192.168.2.17"`.
* **$query_string** - the Cassandra cql command. See also [http://cassandra.apache.org/doc/latest/cql/index.html](http://cassandra.apache.org/doc/latest/cql/index.html) for further details.


The function returns a JSON string indicating success or failure of the operation.
> the success output:
> ```json
> {
>    "result_code": 0
> }
> ```

> the failure output:
> ```json
> {
>    "result_code": 16777226,
>    "message"    : "No hosts available for the control connection"
> }
> ```

> **NOTE**: The error code see [enum CassError](https://docs.datastax.com/en/developer/cpp-driver/2.9/api/cassandra.h/#enum-CassError).

The following examples illustrate how to use the function contrast with `cqlsh` utility.
```sql
/*
  the following statement likes:

    cqlsh> INSERT INTO test.t1 (id, value) VALUES (1, 'one');
*/
mysql>  SELECT `cassandra`('192.168.24.29', 'INSERT INTO test.t1 (id, value) VALUES (1, ''one'')')\G
*************************** 1. row ***************************
`cassandra`('192.168.24.29', 'INSERT INTO test.t1 (id, value) VALUES (1, ''one'')'): {
        "result_code":  0
}
1 row in set (0.056 sec)


/*
  the following statement likes:

    cqlsh> INSERT INTO test.t1 (id, unknow_col) VALUES (1, 'one');
    InvalidRequest: Error from server: code=2200 [Invalid query] message="Undefined column name unknow_col"

*/
mysql>  SELECT `cassandra`('192.168.24.29', 'INSERT INTO test.t1 (id, unknow_col) VALUES (1, ''one'')')\G
*************************** 1. row ***************************
`cassandra`('192.168.24.29', 'INSERT INTO test.t1 (id, value) VALUES (1, ''one'')'): {
        "result_code":  33563136,
        "message":      "Undefined column name unknow_col"
}
1 row in set (0.046 sec)
```

[Back to TOC](#table-of-contents)


TODO
----
- [ ] implement Cassandra Authentication

[Back to TOC](#table-of-contents)


Copyright and License
---------------------

[Back to TOC](#table-of-contents)


See Also
---------

[Back to TOC](#table-of-contents)

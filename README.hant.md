lib_mysqludf_cassandra
======================
提供一組 UDF 指令於 Mysql/MariaDB 中存取 Cassandra。

[English](README.md) | [繁體中文](README.hant.md)


#### 目錄
* [簡介](#%E7%B0%A1%E4%BB%8B)
* [系統需求](#%E7%B3%BB%E7%B5%B1%E9%9C%80%E6%B1%82)
* [編譯與安裝外掛元件](#%E7%B7%A8%E8%AD%AF%E8%88%87%E5%AE%89%E8%A3%9D%E5%A4%96%E6%8E%9B%E5%85%83%E4%BB%B6)
    * [編譯參數](#%E7%B7%A8%E8%AD%AF%E5%8F%83%E6%95%B8)
    * [編譯變數](#%E7%B7%A8%E8%AD%AF%E8%AE%8A%E6%95%B8)
* [安裝與卸載 UDF](#%E5%AE%89%E8%A3%9D%E8%88%87%E5%8D%B8%E8%BC%89-udf)
* [使用方式](#%E4%BD%BF%E7%94%A8%E6%96%B9%E5%BC%8F)
* [待完成事項](#%E5%BE%85%E5%AE%8C%E6%88%90%E4%BA%8B%E9%A0%85)
* [授權條款](#%E6%8E%88%E6%AC%8A%E6%A2%9D%E6%AC%BE)
* [相關連結](#%E7%9B%B8%E9%97%9C%E9%80%A3%E7%B5%90)


簡介
----



[回目錄](#%E7%9B%AE%E9%8C%84)


系統需求
--------
* 架構： Linux 64-bit(x64)
* 編譯器： GCC 4.1.2+
* MariaDB 5.5+
* Cassandra 3.9+
* 相依套件：
    * MariaDB development library 5.5+
    * libuv 1.20.3+
    * datastax/cpp-driver 2.9.0+
    * cJSON 1.6+

[回目錄](#%E7%9B%AE%E9%8C%84)


編譯與安裝外掛元件
------------------
安裝相依套件
> CentOS
> ```bash
> # 安裝工具
> $ yum install -y make wget gcc automake cmake gcc-c++ libtool git
>
> # 安裝相依套件
> $ yum install -y openssl openssl-devel
>
> # 安裝 mariadb development tool
> $ yum install -y mariadb-devel
> ```

要編譯外掛元件最簡單的方式就是直接執行 `make` 與 `make install`。
```bash
$ make

# 安裝外掛程式庫到目的資料夾
$ make install

# 安裝 UDF 到 Mysql/MariaDB 伺服器
$ make installdb
```
> **附記**：如果使用的 Mysql/MariaDB 是早期版本，或是使用手動編譯方式安裝，預設的 include 路徑可能無法使用；請於編譯時使用 `` make INCLUDE_PATH=`mysql_config --variable=pkgincludedir` `` 指定 `INCLUDE_PATH` 變數進行編譯。


#### 編譯參數
* `install`

  安裝外掛程式庫到指定的 Mysql 外掛資料夾。

* `installdb`

  安裝/註冊 UDFs 到 Mysql/MariaDB 伺服器。

* `uninstalldb`

  卸載/註銷 UDFs。

* `clean`

  清除編譯檔案。

* `distclean`

  如同 `clean` 指令，且同時還清理相依套件資源。

#### 編譯變數
下面是編譯時的變數，可以被使用在 `make`：
* `LIBUV_MODULE_VER`

  要提供給元件編譯的 [libuv](https://dist.libuv.org/) 版本。如果該值為空或未指定，其預設值為 `1.20.3`。

* `CPP_DRIVER_MODULE_VER`

  要提供給元件編譯的 [datastax/cpp-driver](https://github.com/datastax/cpp-driver) 版本。如果該值為空或未指定，其預設值為 `2.9.0`。

* `CJSON_MODULE_VER`

  要提供給元件編譯的 [cJSON](https://github.com/DaveGamble/cJSON)  版本。如果該值為空或未指定，其預設值為 `1.6.0`。

* `INCLUDE_PATH`

  指定要被參考的 MariaDB/Mysql C 標頭檔。如果該值為空或未指定，其預設值指定為 Mysql `pkgincludedir` 變數。這個值可以經由下列命令取得：
  ``` bash
  $ echo `mysql_config --variable=pkgincludedir`/server
  ```

* `PLUGIN_PATH`

  指定 MariaDB/Mysql 的外掛元件檔案路徑。這個值可以在 MariaDB/Mysql 中，經由 `SHOW VARIABLES LIKE '%plugin_dir%';` 指令取得。如果該值為空或未指定，其預設值指定為 Mysql `plugindir` 變數。這個值可以經由下列命令取得：
  ``` bash
  $ mysql_config --plugindir
  ```

範例:
```bash
# 指定 MariaDB/Mysql 的外掛元件檔案路徑為 /opt/mysql/plugin
$ make PLUGIN_PATH=/opt/mysql/plugin
$ make install
```
[回目錄](#%E7%9B%AE%E9%8C%84)


安裝與卸載 UDF
--------------
使用 `make` 安裝 UDF：

```bash
$ make installdb
```

> 或於 Mysql/MariaDB 中，手動執行下列 sql 陳述式：
>
> ```sql
> mysql>  CREATE FUNCTION `cassandra` RETURNS STRING SONAME 'lib_mysqludf_cassandra.so';
> ```

要卸載/註銷 UDF，可以使用 `make uninstalldb`；或於 Mysql/MariaDB 中，執行下列 sql 陳述式：
```sql
mysql>  DROP FUNCTION IF EXISTS `cassandra`;
```

[回目錄](#%E7%9B%AE%E9%8C%84)


使用方式
--------
### `cassandra`(*$hosts*,  *$query_string*)

呼叫 cassandra 命令，藉由指定 `$hosts`, `$command` 以及命令參數。

* **$hosts** - 表示要連線的 Cassandra 群集主機，這個字串表示如：`"127.0.0.1"` 或 `"192.168.2.15,192.168.2.17"`。
* **$query_string** - Cassandra cql查詢命令。請詳見 Cassandra 參考手冊 [http://cassandra.apache.org/doc/latest/cql/index.html](http://cassandra.apache.org/doc/latest/cql/index.html)。


函式回傳 JSON 字串指示操作成功或失敗。
> 若成功則輸出：
> ```json
> {
>    "result_code": 0
> }
> ```

> 若失敗則輸出：
> ```json
> {
>    "result_code": 16777226,
>    "message"    : "No hosts available for the control connection"
> }
> ```

> **附記**: 錯誤代碼詳見 [enum CassError](https://docs.datastax.com/en/developer/cpp-driver/2.9/api/cassandra.h/#enum-CassError)。

下列範例說明函式的使用方式，並且與 `cqlsh` 命令工具作對照。
```sql
/*
  下面的陳述式如同：

    cqlsh> INSERT INTO test.t1 (id, value) VALUES (1, 'one');
*/
mysql>  SELECT `cassandra`('192.168.24.29', 'INSERT INTO test.t1 (id, value) VALUES (1, ''one'')')\G
*************************** 1. row ***************************
`cassandra`('192.168.24.29', 'INSERT INTO test.t1 (id, value) VALUES (1, ''one'')'): {
        "result_code":  0
}
1 row in set (0.056 sec)


/*
  下面的陳述式如同：

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

[回目錄](#%E7%9B%AE%E9%8C%84)


待完成事項
----------
- [ ] 實作 Cassandra 連線驗證機制。

[回目錄](#%E7%9B%AE%E9%8C%84)


授權條款
--------

[回目錄](#%E7%9B%AE%E9%8C%84)


相關連結
--------

[回目錄](#%E7%9B%AE%E9%8C%84)

## QtDao2.0
#### TODO List:
- **support**
  - [x] sqlite
  - [ ] mysql
  - [ ] sqlserver
  - [ ] obfuscate field

- **loader**
  - [x] configure loader
  - [ ] driver loader
  - [x] connect test
  - [x] auto create database/table (use config)
  - [x] truncate/drop table
  - [x] drop database
  - [ ] ~~database switch~~
- **upgrader**
  - [x] version check
  - [x] table upgrade
- **log**
  - [x] sql statement print 
  - [x] value list print
- **entity generator**
  - [x] field getter/setter
  - [x] table field
  - [x] transient field
  - [x] default value
  - [x] function query result getter
- **query condition**
  - [x] operator field <-> value
  - [x] operator field <-> field
  - [ ] ~~operator field <-> function~~
  - [ ] ~~operator function <-> function~~
  - [ ] ~~operator field <-> select results~~(move to function query)
- **query executor**
  - insert operator
    - [x] object(s) insert
    - [x] part field value insert
    - [x] insert in select results
    - [x] insert or replace
  - select operator
    - [x] unique/list select
    - [ ] ~~multi-table select~~
    - [x] union select
    - [x] inner/left/right/full join
    - [x] recursive select
    - [x] select in select
    - [x] join result <-> select result 
    - [x] original query
    - [x] custom function query
    - [ ] ~~custom statement query~~
  - count operator
    - [x] condition count
    - [ ] ~~multi-table count~~
    - [x] count in select result
  - update operator
    - [x] set by where update
    - [x] object(s) update
    - [ ] ~~update with select results~~(move to function query)
  - delete operator
    - [x] condition delete
    - [x] object(s) delete
    - [ ] ~~delete by select results~~(move to function query)
  - other
    - [x] function with sub query
    - [ ] transaction
    - [x] glob
    - [x] distinct
    - [x] having
    - [ ] between date to date
- **connection pool**
  - [x] connection reuse
  - [x] work thread connection separate
  - [x] work thread connection reuse
- **auto close connection**
  - [ ] auto close connection when work thread end
- **exception**
  - [x] load configure exception
  - [x] upgrade exception
  - [x] query exception
  - [x] connection pool exception

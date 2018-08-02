# Installation

These instructions are for installing tabitha on the Ubuntu Operating System.

## Overview

You will need to install the PostgreSQL server.  PostgreSQL is an
object-relational database that is used to store data.  There are manual
steps to initialize the database. Following, you will install tabitha
and then load the database schema into PostgreSQL.

## Summary

* Install PostgreSQL
* Download and install tabitha
* Load tabitha schema

## PostgreSQL

Open a terminal and install PostgreSQL, client and libraries

```
sudo apt-get install postgresql postgresql-client libpq5
```

The next thing to do is to set password for user postgres.

```
sudo su - postgres
psql
postgres=# \password postgres
\q
exit
```

Afterwards, set the authentication method for user postgres to md5.

```
sudo nano /etc/postgresql/10/main/pg_hba.conf
```

At the time of this writing PostgreSQL version is 10. If you have a 
later version, the path may be different than the one shown.

Scroll down to the following line

```
local   all             postgres                                peer
```

and change peer to md5. Save (CTRL+O), then exit (CTRL+X).

Restart the postgres service

```
sudo /etc/init.d/postgresql reload
```

## tabitha

tabitha for debian based systems comes as a package. Download the latest
release from here:

[https://github.com/markjolesen/tabitha/releases/download/v1.0.0/tabitha_1.0-1-amd64.deb](https://github.com/markjolesen/tabitha/releases/download/v1.0.0/tabitha_1.0-1-amd64.deb)

After downloading, as root user install the package

```
sudo dpkg -i tabitha_1.0-1-amd64.deb
```

The final step is to load tabitha database schema into PostgreSQL. To do this
issue the following command.

```
sudo su - postgres
psql -f /usr/share/tabitha/sql/schema.sql
exit
```

Upgrading from PostgreSQL server to another requires a backup and restore 
of the databases. It is recommended that automatic updates are blocked.
To do this, as root, set the PostgreSQL package to hold.


```
apt-mark hold postgresql 
```








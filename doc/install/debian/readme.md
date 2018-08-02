
# Installation

These instructions are for installing tabitha on the Debian Operating System.

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

Debian has a wiki on how to install PostgreSQL. It can be found here

[https://wiki.debian.org/PostgreSql](https://wiki.debian.org/PostgreSql)

This section will summarize the commands. 

You will need root privileges. Open a terminal, and swith to the root
user.

```
su -
```

Following install PostgreSQL, client and libraries.

```
apt-get install postgresql postgresql-client libpq5
```

[Terminal 1](/doc/install/debian/terminal1.png)

The next thing to do is to set password for user postgres.

```
su - postgres
psql
postgres=# \password postgres
\q
exit
```

[Terminal 2](/doc/install/debian/terminal2.png)

Afterwards, set authentication for user postgres to md5.

```
nano /etc/postgresql/9.6/main/pg_hba.conf
```

At the time of this writing PostgreSQL version is 9.6. If you have a 
later version, the path may be different than the one shown.

Scroll down to the following line

```
local   all             postgres                                peer
```

and change peer to md5. Save (CTRL+O), then exit (CTRL+X).

Restart the postgres service

```
/etc/init.d/postgresql reload
```

## tabitha

tabitha for debian based systems comes as a package. Download the latest
release from here:

[https://github.com/markjolesen/tabitha/releases/download/v1.0.0/tabitha_1.0-1-amd64.deb](https://github.com/markjolesen/tabitha/releases/download/v1.0.0/tabitha_1.0-1-amd64.deb)

After downloading, as root user install the package

```
dpkg -i tabitha_1.0-1-amd64.deb
```

The final step is to load tabitha database schema into PostgreSQL. To do this
issue the following command.

```
su - postgres
psql -f /usr/share/tabitha/sql/schema.sql
```

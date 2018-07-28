
![tabitha](https://github.com/markjolesen/tabitha/tree/master/image/logo/tabitha-64x64.png)

# tabitha

tabitha is a very simple single entry invoicing system for the service
sector such as HVAC.  It is Public Domain software.

# Operating Systems

BSD flavors, GNU/Linux, and Windows 64-bit Operating Systems.

In a nutshell, any Operating System environment GTK-+3.0 and PostgreSQL is
available, tabitha will run.

# Dependencies

Following, are the primary dependencies. Each module may have additional
dependencies, however.

| STATIC | MOUDLE        | LICENSE      | URI                                           |
|:------:|---------------|--------------|-----------------------------------------------|
| YES    | DecNumber     | ICU          | http://speleotrove.com/decimal/decnumber.html | 
| NO     | pq            | PostgreSQL   | https://www.postgresql.org                    |`
| NO     | GTK+-3.0      | GNU LGPL 2.1 | https://www.gtk.org                           |

# Installation

	There are two parts to installing tabitha: (1) PostgreSQL and
	(2) tabitha. Downloading the files and installing them could
	take up to an hour. 

## Overview

	There is a process to installing and getting a working version
	of the software. You must install PostgreSQL. PostgreSQL is an
	object-relational database that is used to store tabitha data.
	There are manual steps required. Therefore, it is recommened you
	read all of the steps first to get acquanited with the commands.
	Failure to exucute them correctly can leave the database in an
	unkown state. If this happens, additional steps will have to be
	executed to restore the system back to a sane state.

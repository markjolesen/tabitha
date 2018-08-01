
# Installation

These instructions are for the Windows 64-bit Operating System.

There are two parts to installing tabitha: (1) PostgreSQL and (2)
tabitha. Downloading the files and installing them could take up to
an hour.

## Overview

There is a process to installing and getting a working version
of the software. You must install PostgreSQL. PostgreSQL is an
object-relational database that is used to store data.  There are manual
steps required. Therefore, it is recommened you read all of the steps
first to get acquanited with the commands.  Failure to exucute them
correctly can leave the database in an unkown state. If this happens,
additional steps will have to be executed to restore the system back to
a sane state.


## Summary

* Download and install PostgreSQL
* Download and install tabitha
* Load tabitha schema

## PostgreSQL

You may skip this section if PostgreSQL is already installed.
However, you may want to think about upgrading if your version
is nearing it's end of life.

PostgreSQL download page has several releases of it's software
available for download. It is recommened to retrieve the latest
stable version. 

You will be using PostgreSQL website to locate and download the
release of the specific version you require.

Primary site: [https://www.postgresql.org/](https://www.postgresql.org/)

Download page: [https://www.postgresql.org/download/windows/](https://www.postgresql.org/download/windows/)

It is recommened to install the bundle supplied from BigSQL.

![BIGSQL](/doc/install/windows/bigsql.png)

Download page: [https://www.openscg.com/bigsql/postgresql/installers.jsp/](https://www.openscg.com/bigsql/postgresql/installers.jsp/)

The lastet version as of this writing is PostgreSQL 10.4. Using
your mouse click on the 10.4 version by the Windows icon. Follow
the instructions to 'Save' the file to disk. Browsers will
typically save the file into your users Downloads directory. 

After downloading, using a login with adminstrative rights, browse to
the directory and execute it.

Accepting the default install options throughout most of the screens
should be sufficent.

### Screen 1

Press the next buttton to begin installation

![PostgreSQl install screen 1](/doc/install/windows/pginstall1.png)

### Screen 2

Choose the install directory. Accepting the default is recommended.

![PostgreSQl install screen 2](/doc/install/windows/pginstall2.png)

### Screen 3

Choose the components you would like to install. PostgreSQL Server and
Command Line Tools must be checked. Selecting them all (th default)
is recommended.

![PostgreSQl install screen 3](/doc/install/windows/pginstall3.png)

### Screen 4

Choose database data location. Accepting the default is recommended.

![PostgreSQl install screen 4](/doc/install/windows/pginstall4.png)

### Screen 5

Enter the password for PostgreSQL server. This password is for the
supeuser 'postgres.'

![PostgreSQl install screen 5](/doc/install/windows/pginstall5.png)

### Screen 6

Enter the port PostgreSQL server will communicate through. The default
port is recommended.

![PostgreSQl install screen 6](/doc/install/windows/pginstall6.png)

### Screen 7

Select the locale. 

![PostgreSQl install screen 7](/doc/install/windows/pginstall7.png)

### Screen 8

Before committing, review the installation options. If there is an issue,
hit the back button until you get to the screen which will allow you
to correct it.

![PostgreSQl install screen 8](/doc/install/windows/pginstall8.png)

### Screen 9

Finished screen.

![PostgreSQl install screen 9](/doc/install/windows/pginstall9.png)

## tabitha

tabitha for Windows comes with a prepackeged self installer.
After downloading the package, unzip it, and then start the installer
executable.  It will guide you through the process.

Zip Download: [https://github.com/markjolesen/tabitha/files/2245146/tabitha.zip](https://github.com/markjolesen/tabitha/files/2245146/tabitha.zip)

tabitha does not make use of the system registry. All files are
installed in a root directory. That folder can be deleted to remove
the software. However, it is recommened to use WIndows Control Panel to
remove the application.

### Screen 1

Choose the install directory. Accepting the default is recommended.

![tabitha install screen 1](/doc/install/windows/tabithainstall1.png)

### Screen 2

If you want a desktop icon, check Create a desktop shortcut checkbox.

![tabitha install screen 2](/doc/install/windows/tabithainstall2.png)

### Screen 3

Before committing, review the installation options. If there is an issue,
hit the back button until you get to the screen which will allow you
to correct it.

![tabitha install screen 3](/doc/install/windows/tabithainstall3.png)

## Post Install

The post install step will load the tabitha database schema into
PostgreSQL. This phase will require entering commands in the command
shell.

Open a shell by entering 'cmd' in Cortana search bar, and then clicking on
'Command Prompt.'

![Cortana](/doc/install/windows/cortana.png)

This will open a shell like the one shown below.

![cmd screen 1](/doc/install/windows/cmd1.png)

Type the following commands:

```
chcp 1252
cd "C:\Program Files\PostgreSQL\10\bin"
psql -U postgres -f "c:\Program Files\tabitha\sql\schema.sql"
```
If everything executed without error, you can now run tabitha.
The default password for the tabitha database is tabitha.


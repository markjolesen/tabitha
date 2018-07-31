
# Installation

	These instructions are for the Windows 64-bit Operating System.

	There are two parts to installing tabitha: (1) PostgreSQL and
	(2) tabitha. Downloading the files and installing them could
	take up to an hour. 

## Overview

	There is a process to installing and getting a working version
	of the software. You must install PostgreSQL. PostgreSQL is an
	object-relational database that is used to store data.
	There are manual steps required. Therefore, it is recommened you
	read all of the steps first to get acquanited with the commands.
	Failure to exucute them correctly can leave the database in an
	unkown state. If this happens, additional steps will have to be
	executed to restore the system back to a sane state.

## Summary

	* Download and install PostgreSQL
	* Download and install tabitha
	* Initialize PostgreSQL database
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

	![BIGSQL](/docinstall/bigsql.png)

	Download page: [https://www.openscg.com/bigsql/postgresql/installers.jsp/](https://www.openscg.com/bigsql/postgresql/installers.jsp/)

	The lastet version as of this writing is PostgreSQL 10.4. Using
	your mouse click on the 10.4 version by the Windows icon. Follow
	the instructions to 'Save' the file to disk. Browsers will
	typically save the file into your users Downloads directory. 

	After downloading, using a login with adminstrative rights,
	browse to the directory and execute it.

	![PostgreSQl screen 1](/docinstall/pginstall1.png)

	Accepting the default install options throughout the screens should
	be sufficent. 

	![PostgreSQl install screen 1](/docinstall/pginstall1.png)

	![PostgreSQl install screen 2](/docinstall/pginstall2.png)

	![PostgreSQl install screen 3](/docinstall/pginstall3.png)

	![PostgreSQl install screen 4](/docinstall/pginstall4.png)

	![PostgreSQl install screen 5](/docinstall/pginstall5.png)

	![PostgreSQl install screen 6](/docinstall/pginstall6.png)

	![PostgreSQl install screen 7](/docinstall/pginstall7.png)

	![PostgreSQl install screen 8](/docinstall/pginstall8.png)

	![PostgreSQl install screen 9](/docinstall/pginstall9.png)

## tabitha

	tabitha for Windows comes with a prepackeged self installer.
	After downloading the package, start the installer and it will
	guide you through the process.

	tabitha does not make use of the system registry. All files are
	installed in a root directory. That folder can be deleted to
	remove the software. However, it is recommened to use WIndows
	Control Panel to remove the application.

	![tabitha install screen 1](/docinstall/tabithainstall1.png)

	![tabitha install screen 2](/docinstall/tabithainstall2.png)

	![tabitha install screen 3](/docinstall/tabithainstall3.png)

## Post Install

	The post install step will load the tabitha database schema
	into PostgreSQL. This phase will require entering commands in 
	the command shell.

	Open a shell by entering 'cmd' in Cortana search bar, and
	then clicking on 'Command Prompt.'

	![Cortana](/docinstall/cortana.png)

	This will open a shell like the one shown below.

	![cmd screen 1](/docinstall/cmd1.png)

	Type the following commands:

	```
	chcp 1252
	cd "C:\Program Files\PostgreSQL\10\bin"
	psql -U postgres -f "c:\Program Files\tabitha\sql\schema.sql"
	```
	If everything executed without error, you can now run tabitha.
	The default password for the tabitha database is tabitha.


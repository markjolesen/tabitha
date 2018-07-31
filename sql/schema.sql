/*
  schema.sql

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/

/*
  psql -U <username> -f schema.sql
   Creates tabitha database using <username>
*/

CREATE DATABASE tabitha;
CREATE ROLE tabitha WITH LOGIN PASSWORD 'tabitha';

\c tabitha

CREATE EXTENSION IF NOT EXISTS pgcrypto;

CREATE TYPE SALESTYPE AS ENUM ('Estimate', 'Invoice');
CREATE TYPE CONTACTTYPE AS ENUM ('Residential', 'Commercial', 'Vendor');

CREATE TABLE company
(
	company_id BIGINT UNIQUE default(0),
	company_name VARCHAR(32),
	street1 VARCHAR(64),
	street2 VARCHAR(64),
	street3 VARCHAR(64),
	city VARCHAR(24),
	state VARCHAR(4),
	zipcode VARCHAR(11),
	phone VARCHAR(16),
	cellphone VARCHAR(16),
	fax VARCHAR(16),
	email VARCHAR(41),
	website VARCHAR(41),
	smtp_server VARCHAR(41),
	smtp_username VARCHAR(41),
	smtp_password TEXT,
	smtp_port SMALLINT,
	CONSTRAINT check_id_singlerow CHECK (company_id = 0)
);

INSERT INTO company (company_id) VALUES(0);

CREATE TABLE contact
(
	contact_id VARCHAR(19) PRIMARY KEY NOT NULL,
	contact_type CONTACTTYPE NOT NULL,
	company_name VARCHAR(32),
	first_name VARCHAR(32),
	last_name VARCHAR(32),
	street1 VARCHAR(64),
	street2 VARCHAR(64),
	street3 VARCHAR(64),
	city VARCHAR(24),
	state VARCHAR(4),
	zipcode VARCHAR(11),
	phone VARCHAR(16),
	cellphone VARCHAR(16),
	fax VARCHAR(16),
	email VARCHAR(41) /*UNIQUE*/,
	website VARCHAR(41),
	notes TEXT
);

CREATE TABLE product
(
	product_id VARCHAR(19) PRIMARY KEY NOT NULL,
	description VARCHAR(64),
	unit_price NUMERIC(12,2)
);

CREATE TABLE sales
(
	sales_id BIGSERIAL PRIMARY KEY NOT NULL,
	sales_type SALESTYPE,
	billing_id VARCHAR(19) REFERENCES contact(contact_id),
	service_contact TEXT,
	sales_date DATE,
	start_date DATE,
	completed_date DATE,
	due_date DATE,
	customer_po VARCHAR(32),
	discount NUMERIC(12,2),
	notes TEXT,
	notes_internal TEXT
);

ALTER SEQUENCE sales_sales_id_seq RESTART WITH 1510;

CREATE TABLE sales_detail
(
	sales_id BIGINT REFERENCES sales(sales_id) NOT NULL,
	line_number SMALLINT NOT NULL,
	product_id VARCHAR(19) REFERENCES product(product_id),
	description VARCHAR(64),
	quantity BIGINT,
	unit_price NUMERIC(12,2),
	UNIQUE (sales_id, line_number)
);

CREATE TABLE receipt
(
	receipt_id BIGSERIAL PRIMARY KEY NOT NULL,
	sales_id BIGINT REFERENCES sales(sales_id) NOT NULL,
	received_date DATE NOT NULL,
	amount NUMERIC(12,2) NOT NULL,
	reference VARCHAR(32)
);

GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO tabitha;
GRANT USAGE, SELECT ON ALL SEQUENCES in SCHEMA public TO tabitha;

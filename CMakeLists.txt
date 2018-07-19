# CMakeLists.txt
#
# License CC0 PUBLIC DOMAIN
#
# To the extent possible under law, Mark J. Olesen has waived all copyright
# and related or neighboring rights to tabitha. This work is published from:
# United States.

cmake_minimum_required(VERSION 3.11)

project(tabitha)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -rdynamic -pedantic -Wall -Wextra")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/glade/tabitha.glade
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GLIB2 REQUIRED glib-2.0)
pkg_check_modules(GMODULE REQUIRED gmodule-export-2.0)
pkg_check_modules(PSQL REQUIRED libpq)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src/decNumber)
include_directories(${GTK3_INCLUDE_DIRS})
include_directories(${GLIB2_INCLUDE_DIRS})
include_directories(${GMODULE_INCLUDE_DIRS})
include_directories(${PSQL_INCLUDE_DIRS})

link_directories(${GTK3_LIBRARY_DIRS})
link_directories(${GLIB2_LIBRARY_DIRS})
link_directories(${GMODULE_LIBRARY_DIRS})
link_directories(${PSQL_LIBRARY_DIRS})

add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(tabitha
	src/aspect.c
	src/aspect_form.c
	src/calendar.c
	src/calendar_form.c
	src/company.c
	src/company_form.c
	src/contact.c
	src/contact_form.c
	src/contact_index_form.c
	src/edit_form.c
	src/error.c
	src/format.c
	src/glade.c
	src/main.c
	src/menu.c
	src/product.c
	src/product_form.c
	src/product_index_form.c
	src/receipt.c
	src/receipt_form.c
	src/receipt_index_form.c
	src/sales.c
	src/sales_detail.c
	src/sales_detail_form.c
	src/sales_form.c
	src/sales_index_form.c
	src/sales_print.c
	src/session.c
	src/smtp_client.c
	src/trim.c
	src/decNumber/decContext.c
	src/decNumber/decNumber.c)

target_link_libraries(tabitha
	${GTK3_LIBRARIES}
	${GLIB2_LIBRARIES}
	${GMODULE_LIBRARIES}
	${PSQL_LIBRARIES}
	ssl
	crypto)


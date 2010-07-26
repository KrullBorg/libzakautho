CREATE TABLE resources (id INTEGER, resource_id VARCHAR (255));
CREATE TABLE resources_parents (id_resources INTEGER, id_resources_parent INTEGER);
CREATE TABLE roles (id INTEGER, role_id VARCHAR (255));
CREATE TABLE roles_parents (id_roles INTEGER, id_roles_parent INTEGER);
CREATE TABLE rules (id INTEGER, type INTEGER, id_roles INTEGER, id_resources INTEGER);

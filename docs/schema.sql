CREATE TABLE roles
(
  id integer NOT NULL,
  role_id character varying(255) DEFAULT ''::character varying,
  CONSTRAINT roles_pkey PRIMARY KEY (id)
)

CREATE TABLE roles_parents
(
  id_roles integer NOT NULL,
  id_roles_parent integer NOT NULL,
  CONSTRAINT roles_parents_pkey PRIMARY KEY (id_roles, id_roles_parent)
)

CREATE TABLE resources
(
  id integer NOT NULL,
  resource_id character varying(255) DEFAULT ''::character varying,
  CONSTRAINT resources_pkey PRIMARY KEY (id)
)

CREATE TABLE resources_parents
(
  id_resources integer NOT NULL,
  id_resources_parent integer NOT NULL,
  CONSTRAINT resources_parents_pkey PRIMARY KEY (id_resources, id_resources_parent)
)

CREATE TABLE rules
(
  id integer NOT NULL,
  "type" integer,
  id_roles integer,
  id_resources integer,
  CONSTRAINT rules_pkey PRIMARY KEY (id)
)
COMMENT ON COLUMN rules."type" IS '1 - allow
2 - deny';

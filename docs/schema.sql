--
-- PostgreSQL database dump
--

-- Dumped from database version 8.4.10
-- Dumped by pg_dump version 9.1.2
-- Started on 2011-12-28 08:55:26

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

--
-- TOC entry 460 (class 2612 OID 16386)
-- Name: plpgsql; Type: PROCEDURAL LANGUAGE; Schema: -; Owner: -
--

CREATE OR REPLACE PROCEDURAL LANGUAGE plpgsql;


SET search_path = public, pg_catalog;

--
-- TOC entry 158 (class 1255 OID 107639)
-- Dependencies: 3 460
-- Name: update_update(); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION update_update() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
	begin
	update timestamp_update set update=now();
	return NULL;
end;
$$;


SET default_with_oids = false;

--
-- TOC entry 142 (class 1259 OID 61459)
-- Dependencies: 1792 3
-- Name: resources; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE resources (
    id integer NOT NULL,
    resource_id character varying(255) DEFAULT ''::character varying
);


--
-- TOC entry 143 (class 1259 OID 61465)
-- Dependencies: 3
-- Name: resources_parents; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE resources_parents (
    id_resources integer NOT NULL,
    id_resources_parent integer NOT NULL
);


--
-- TOC entry 140 (class 1259 OID 61448)
-- Dependencies: 1791 3
-- Name: roles; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE roles (
    id integer NOT NULL,
    role_id character varying(255) DEFAULT ''::character varying
);


--
-- TOC entry 141 (class 1259 OID 61454)
-- Dependencies: 3
-- Name: roles_parents; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE roles_parents (
    id_roles integer NOT NULL,
    id_roles_parent integer NOT NULL
);


--
-- TOC entry 144 (class 1259 OID 61470)
-- Dependencies: 3
-- Name: rules; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rules (
    id integer NOT NULL,
    type integer,
    id_roles integer,
    id_resources integer
);


--
-- TOC entry 1812 (class 0 OID 0)
-- Dependencies: 144
-- Name: COLUMN rules.type; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON COLUMN rules.type IS '1 - allow
2 - deny';


--
-- TOC entry 145 (class 1259 OID 107636)
-- Dependencies: 3
-- Name: timestamp_update; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE timestamp_update (
    update timestamp without time zone
);


--
-- TOC entry 1800 (class 2606 OID 61469)
-- Dependencies: 143 143 143
-- Name: resources_parents_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY resources_parents
    ADD CONSTRAINT resources_parents_pkey PRIMARY KEY (id_resources, id_resources_parent);


--
-- TOC entry 1798 (class 2606 OID 61464)
-- Dependencies: 142 142
-- Name: resources_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY resources
    ADD CONSTRAINT resources_pkey PRIMARY KEY (id);


--
-- TOC entry 1796 (class 2606 OID 61458)
-- Dependencies: 141 141 141
-- Name: roles_parents_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY roles_parents
    ADD CONSTRAINT roles_parents_pkey PRIMARY KEY (id_roles, id_roles_parent);


--
-- TOC entry 1794 (class 2606 OID 61453)
-- Dependencies: 140 140
-- Name: roles_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY roles
    ADD CONSTRAINT roles_pkey PRIMARY KEY (id);


--
-- TOC entry 1802 (class 2606 OID 61474)
-- Dependencies: 144 144
-- Name: rules_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rules
    ADD CONSTRAINT rules_pkey PRIMARY KEY (id);


--
-- TOC entry 1806 (class 2620 OID 107641)
-- Dependencies: 143 158
-- Name: resources_parents_updated; Type: TRIGGER; Schema: public; Owner: -
--

CREATE TRIGGER resources_parents_updated
    AFTER INSERT OR DELETE OR UPDATE ON resources_parents
    FOR EACH STATEMENT
    EXECUTE PROCEDURE update_update();


--
-- TOC entry 1805 (class 2620 OID 107640)
-- Dependencies: 142 158
-- Name: resources_updated; Type: TRIGGER; Schema: public; Owner: -
--

CREATE TRIGGER resources_updated
    AFTER INSERT OR DELETE OR UPDATE ON resources
    FOR EACH STATEMENT
    EXECUTE PROCEDURE update_update();


--
-- TOC entry 1804 (class 2620 OID 107643)
-- Dependencies: 158 141
-- Name: roles_parents_updated; Type: TRIGGER; Schema: public; Owner: -
--

CREATE TRIGGER roles_parents_updated
    AFTER INSERT OR DELETE OR UPDATE ON roles_parents
    FOR EACH STATEMENT
    EXECUTE PROCEDURE update_update();


--
-- TOC entry 1803 (class 2620 OID 107642)
-- Dependencies: 158 140
-- Name: roles_updated; Type: TRIGGER; Schema: public; Owner: -
--

CREATE TRIGGER roles_updated
    AFTER INSERT OR DELETE OR UPDATE ON roles
    FOR EACH STATEMENT
    EXECUTE PROCEDURE update_update();


--
-- TOC entry 1807 (class 2620 OID 107644)
-- Dependencies: 144 158
-- Name: rules_updated; Type: TRIGGER; Schema: public; Owner: -
--

CREATE TRIGGER rules_updated
    AFTER INSERT OR DELETE OR UPDATE ON rules
    FOR EACH STATEMENT
    EXECUTE PROCEDURE update_update();


-- Completed on 2011-12-28 08:55:26

--
-- PostgreSQL database dump complete
--


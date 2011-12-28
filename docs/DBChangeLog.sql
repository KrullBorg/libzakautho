--
-- 0.0.1 -> 0.0.2
--

CREATE TABLE timestamp_update
(
	update timestamp without time zone
);

INSERT INTO timestamp_update VALUES (NOW());

CREATE OR REPLACE FUNCTION update_update() RETURNS trigger AS
$BODY$
	begin
	update timestamp_update set update=now();
	return NULL;
end;
$BODY$ LANGUAGE plpgsql;

CREATE TRIGGER resources_updated
	AFTER INSERT OR UPDATE OR DELETE
	ON resources
	EXECUTE PROCEDURE update_update();

CREATE TRIGGER resources_parents_updated
	AFTER INSERT OR UPDATE OR DELETE
	ON resources_parents
	EXECUTE PROCEDURE update_update();

CREATE TRIGGER roles_updated
	AFTER INSERT OR UPDATE OR DELETE
	ON roles
	EXECUTE PROCEDURE update_update();

CREATE TRIGGER roles_parents_updated
	AFTER INSERT OR UPDATE OR DELETE
	ON roles_parents
	EXECUTE PROCEDURE update_update();

CREATE TRIGGER rules_updated
	AFTER INSERT OR UPDATE OR DELETE
	ON rules
	EXECUTE PROCEDURE update_update();
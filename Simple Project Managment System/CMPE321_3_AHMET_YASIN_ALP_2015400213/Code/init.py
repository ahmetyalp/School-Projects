import psycopg2

conn = psycopg2.connect("dbname=database.db user=postgres password=1234")
c = conn.cursor()

# Tables
c.execute("CREATE TABLE Users (username text  PRIMARY KEY, password text , isadmin integer)")
c.execute("CREATE TABLE NoP (pmname text , noprojects integer)")
c.execute("CREATE TABLE Employee (id integer  PRIMARY KEY, name text)")
c.execute("CREATE TABLE Task (id serial PRIMARY KEY , name text, startdate text , finishdate text , pname text)")
c.execute("CREATE TABLE PT (tid integer , pname text)")
c.execute("CREATE TABLE ET (tid integer , eid integer)")
c.execute("CREATE TABLE Project (project_name text  PRIMARY KEY, startdate text , ettd integer)")
c.execute("CREATE TABLE PPM (pname text , pmname text)")
c.execute("ALTER TABLE public.ppm ADD CONSTRAINT u1 UNIQUE (pname, pmname);")

# SPs and Triggers
c.execute("""
CREATE FUNCTION trig1() RETURNS trigger AS $trig1$
    BEGIN
        INSERT INTO PPM VALUES (NEW.project_name ,(SELECT pmname FROM NoP ORDER BY noprojects LIMIT 1) );
        RETURN NULL;
    END;
$trig1$ LANGUAGE plpgsql;
""")

c.execute("""
CREATE FUNCTION trig4() RETURNS trigger AS $trig4$
	BEGIN
		DELETE FROM ET WHERE eid = OLD.id;
		RETURN NULL;
	END;
$trig4$ LANGUAGE plpgsql;
""")

c.execute("""
CREATE FUNCTION trig2() RETURNS trigger AS $trig2$
    BEGIN
        UPDATE NoP SET noprojects = noprojects + 1 WHERE pmname = NEW.pmname;
        RETURN NULL;
    END;
$trig2$ LANGUAGE plpgsql;
""")

c.execute("""
CREATE FUNCTION trig3() RETURNS trigger AS $trig3$
    BEGIN
        UPDATE NoP SET noprojects = noprojects - 1 WHERE pmname = OLD.pmname;
        RETURN NULL;
    END;
$trig3$ LANGUAGE plpgsql;
""")

c.execute("""
CREATE FUNCTION listcomp(uname text) RETURNS TABLE (projectname text) AS $$
	BEGIN
		IF uname = 'ALL' THEN
			RETURN QUERY SELECT project_name FROM Project WHERE
			EXISTS (SELECT * FROM PT WHERE pname = project_name) AND 
			NOT EXISTS (SELECT * FROM Task WHERE EXISTS (SELECT * FROM PT WHERE pname = project_name AND tid = id) AND to_date(finishdate, 'YYYY-MM-DD') > (SELECT CURRENT_DATE) );
		ELSE
			RETURN QUERY SELECT project_name FROM Project WHERE EXISTS (SELECT * FROM PPM WHERE pname = project_name AND pmname = uname) AND
			EXISTS (SELECT * FROM PT WHERE pname = project_name) AND 
			NOT EXISTS (SELECT * FROM Task WHERE EXISTS (SELECT * FROM PT WHERE pname = project_name AND tid = id) AND to_date(finishdate, 'YYYY-MM-DD') > (SELECT CURRENT_DATE) );
		END IF;
	END
$$ LANGUAGE plpgsql	
""")

c.execute("""
CREATE FUNCTION listuncomp(uname text) RETURNS TABLE (projectname text) AS $$
	BEGIN
		IF uname = 'ALL' THEN
			RETURN QUERY SELECT project_name FROM Project WHERE
			NOT EXISTS (SELECT * FROM PT WHERE pname = project_name) OR 
			EXISTS (SELECT * FROM Task WHERE EXISTS (SELECT * FROM PT WHERE pname = project_name AND tid = id) AND to_date(finishdate, 'YYYY-MM-DD') > (SELECT CURRENT_DATE) );
		ELSE
			RETURN QUERY SELECT project_name FROM Project WHERE EXISTS (SELECT * FROM PPM WHERE pname = project_name AND pmname = uname) AND
			NOT EXISTS (SELECT * FROM PT WHERE pname = project_name) OR 
			EXISTS (SELECT * FROM Task WHERE EXISTS (SELECT * FROM PT WHERE pname = project_name AND tid = id) AND to_date(finishdate, 'YYYY-MM-DD') > (SELECT CURRENT_DATE) );
		END IF;
	END
$$ LANGUAGE plpgsql	
""")

c.execute("CREATE TRIGGER trig1 AFTER INSERT ON Project FOR EACH ROW EXECUTE PROCEDURE trig1();")
c.execute("CREATE TRIGGER trig2 AFTER INSERT ON PPM FOR EACH ROW EXECUTE PROCEDURE trig2();")
c.execute("CREATE TRIGGER trig3 AFTER DELETE ON PPM FOR EACH ROW EXECUTE PROCEDURE trig3();")
c.execute("CREATE TRIGGER trig4 AFTER DELETE ON Employee FOR EACH ROW EXECUTE PROCEDURE trig4();")

# # Predefined Admins
c.execute("INSERT INTO Users VALUES ('admin2' , '123456789' , 1)")
c.execute("INSERT INTO Users VALUES ('admin4' , 'abc123' , 1)")
c.execute("INSERT INTO Users VALUES ('admin1' , '1' , 1)")

conn.commit()
c.close()
conn.close()
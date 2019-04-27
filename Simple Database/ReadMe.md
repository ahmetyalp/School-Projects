Simple database written in Java

- "javac Main.java" to compile java code
- "java Main" to run it
- It supoorts following commands:<br>
	INIT : initialize database full zeros<br>
	CREATE TYPE <TypeName> <Number of fields> <FiledNames>...: Creates new type<br>
	CREATE RECORD <Type> <Fields> : Create new record<br>
	SEARCH <Type> <PrimaryKey>: Search records by primary key<br>
	LIST TYPE : list all types<br>
	LIST RECORD <type> : list all records of that type <br>
	DELETE RECORD <Type> <PrimaryKey> : searches record according to PrimaryKey and deletes it<br>
	DELETE TYPE <Type> : delete type<br>
	EXIT: quits program<br>
- Use stdin to run commands
- Please do not use(open) any file except database that is created by program while program executes a command

- "javac Main.java" to compile java code
- "java Main" to run it
- It supoorts following commands:
	INIT : initialize database full zeros
	CREATE TYPE <TypeName> <Number of fields> <FiledNames>...: Creates new type
	CREATE RECORD <Type> <Fields> : Create new record
	SEARCH <Type> <PrimaryKey>: Search records by primary key
	LIST TYPE : list all types
	LIST RECORD <type> : list all records of that type 
	DELETE RECORD <Type> <PrimaryKey> : searches record according to PrimaryKey and deletes it
	DELETE TYPE <Type> : delete type
	EXIT: quits program
- Use stdin to run commands
- Please do not use(open) any file except database that is created by program while program executes a command

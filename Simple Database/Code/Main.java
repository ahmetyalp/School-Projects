import java.io.*;
import java.nio.ByteBuffer;
import java.util.*;
import static java.util.stream.Collectors.toList;

public class Main {

    public static RandomAccessFile db ;

    public static void init() throws IOException{
        byte[] by = new byte[10000000];
        db.seek(0);
        db.write(by);
        db.seek(0);
        System.out.println("Database is created");
    }

    public static void main(String[] args) throws IOException{
        db = new RandomAccessFile("database.db" , "rw");
        String s;
        String[] ss;
        Scanner in = new Scanner(System.in);
        while(true){
            s  =  in.nextLine();
            ss = s.split(" ");
            if( ss[0].equals("CREATE")){
                if( ss[1].equals("TYPE") ){
                    createRecordType(ss[2] , Integer.parseInt(ss[3]) , Arrays.copyOfRange(ss , 4 , ss.length));
                }
                else if( ss[1].equals("RECORD") ){
                    createRecord(ss[2] , Arrays.asList(Arrays.copyOfRange(ss , 3 , ss.length)).stream().map( i -> Integer.parseInt(i) ).collect(toList()).toArray(new Integer[ss.length-3]));
                }
                else{
                    System.out.println("Wrong Syntax. Please try again");
                }
            }
            else if ( ss[0].equals("DELETE")){
                if( ss[1].equals("TYPE") ){
                    deleteType( ss[2] );
                }
                else if( ss[1].equals("RECORD") ){
                    deleteRecord(ss[2] , Integer.parseInt(ss[3]));
                }
                else{
                    System.out.println("Wrong Syntax. Please try again");
                }

            }
            else if ( ss[0].equals("LIST")){
                if( ss[1].equals("TYPE") )
                    listTypes();
                else if( ss[1].equals("RECORD") ){
                    listRecords( ss[2] );
                }
                else
                    System.out.println("Wrong Syntax");
            }
            else if ( ss[0].equals("EXIT"))
                return;
            else if( ss[0].equals("SEARCH") )
                searchRecord( ss[1] , Integer.parseInt(ss[2]) );
            else if( ss[0].equals("INIT") )
                init();
            else
                System.out.println("Wrong Syntax. Please try again");
        }
    }

    public static byte[] readPage(int pageid) throws IOException{
        db.seek(pageid*1000);
        byte[] ba = new byte[1000];
        db.read(ba , 0 , 1000);
        db.seek(0);
        return ba;
    }

    public static void writePage(int pageid , byte[] ba) throws IOException{
        db.seek(pageid*1000);
        db.write(ba);
        db.seek(0);
    }

    public static void createRecordType(String recordName , int numberOfFields , String[] fields) throws IOException{
        File f = new File("SystemCatalog.txt");
        if(!f.exists())
            f.createNewFile();
        FileWriter sc = new FileWriter(f.getAbsoluteFile() , true);
        sc.write(recordName);
        sc.write(" ");
        sc.write(Integer.toString(numberOfFields));
        sc.write(" ");
        for( String s : fields)
            sc.write(s + " ");
        File ff = new File(recordName + "_DataFile.txt");
        if(!ff.exists())
            ff.createNewFile();
        FileWriter df = new FileWriter(ff.getAbsoluteFile() , true);
        for( int i = 0 ; i < 10000 ; i++){
            byte[] by = readPage(i);
            if ( by[0] == 0 ){
                df.write(Integer.toString(i));
                sc.write(Integer.toString(i));
                df.write("\n");
                by[0] = 1;
                writePage(i , by);
                df.close();
                break;
            }
        }
        sc.write(" ");
        sc.write(Integer.toString(1));
        sc.write("\n");
        sc.close();
        df.close();
        System.out.println("New Record Type was created with name: " + recordName);
    }

    public static void listTypes() throws IOException{
        FileReader fr = new FileReader("SystemCatalog.txt");
        BufferedReader sc = new BufferedReader(fr);
        System.out.println(" All types in Database: ");
        while(true) {
            String s = sc.readLine();
            if ( s == null )
                break;
            StringTokenizer st = new StringTokenizer(s);
            if( !st.hasMoreTokens() )
                break;
            System.out.println("--> " + st.nextToken());
        }
        sc.close();
        fr.close();
    }

    public static void deleteType(String recordName) throws IOException{
        //System.out.println(recordName);
        File f = new File(recordName + "_DataFile.txt");
        Scanner sc = new Scanner(f);
        byte[] by = new byte[1000];
        while(sc.hasNextLine()){
            //System.out.print(sc.nextLine());
            writePage(Integer.parseInt(sc.nextLine()) , by);
        }
        sc.close();
        f.delete();
        f = new File("SystemCatalog.txt");
        File tempFile = new File("Temp.txt");
        FileWriter fw = new FileWriter(tempFile);
        BufferedWriter writer = new BufferedWriter(fw);
        sc = new Scanner(f);
        StringTokenizer st ;
        String ss;
        while(sc.hasNextLine()){
            ss = sc.nextLine();
            st = new StringTokenizer(ss);
            if(st.nextToken().equals(recordName))
                continue;
            writer.write(ss);
            writer.write("\n");
        }
        writer.close();
        sc.close();
        f.delete();
        fw.close();
        tempFile.renameTo(new File("SystemCatalog.txt"));
        System.out.println("Deleted type: " + recordName);
    }

    public static void createRecord(String recordType, Integer[] fields) throws IOException {
        Scanner sc = new Scanner(new File("SystemCatalog.txt"));
        String s;
        String[] st;
        byte[] by;
        int recordSize;
        while(sc.hasNextLine()){
            s = sc.nextLine();
            st = s.split(" ");
            //System.out.println(st[0]);
            if( st[0].equals(recordType)){
                if( Integer.parseInt(st[st.length-2]) >= 0 ){
                    by = readPage(Integer.parseInt(st[st.length-2]));
                    //System.out.println(st[st.length-2] );
                    recordSize = Integer.parseInt(st[1])*4+1;
                    for(int i = 6 ; i+recordSize<1000 ; i = i+recordSize){
                        if(by[i] == 0){
                            by[i] = 1;
                            //Fieldleri doldur
                            ByteBuffer bb;
                            byte[] ba;
                            int j = i+1;
                            for( Integer fi : fields){
                                bb = ByteBuffer.allocate(4);
                                bb.putInt(fi);
                                ba = bb.array();
                                by[j] = ba[0];
                                by[j+1] = ba[1];
                                by[j+2] = ba[2];
                                by[j+3] = ba[3];
                                j = j + 4;
                            }
                            break;
                        }
                    }
                    // numberofrecords++
                    byte[] bf = {by[2] , by[3] , by[4] , by[5]};
                    ByteBuffer bb = ByteBuffer.wrap(bf);
                    int nor = bb.getInt();
                    nor++;
                    bb = ByteBuffer.allocate(4);
                    bb.putInt(nor);
                    byte[] re = bb.array();
                    by[2] = re[0];
                    by[3] = re[1];
                    by[4] = re[2];
                    by[5] = re[3];
                    if( (nor+1)*recordSize > 994)
                    {
                        //set fisrt empty page negative
                        int emptyPage = -1;
                        boolean flag = true;
                        by[1] = 1;
                        Scanner dff = new Scanner(new File(recordType + "_DataFile.txt"));
                        while(dff.hasNextInt()){
                            int pageid = dff.nextInt();
                            byte[] page = readPage(pageid);
                            if(page[1] == 0){
                                //Set new first empty page
                                emptyPage = pageid;
                                flag = false;
                                break;
                            }
                        }
                        dff.close();
                        boolean editFlag = false;
                        if( Integer.parseInt(st[st.length-1]) < 512 && flag ){
                            //Create new empty page
                            for( int k = 0 ; k < 10000 ; k++){
                                byte[] page = readPage(k);
                                if( page[0] == 0){
                                    page[0] = 1;
                                    writePage(k , page);
                                    emptyPage = k;
                                    editFlag = true;
                                    // bu pagei data filea ekle
                                    File f = new File(recordType + "_DataFile.txt");
                                    FileWriter fw = new FileWriter(f.getAbsoluteFile() , true);
                                    fw.write(Integer.toString(k));
                                    fw.write("\n");
                                    fw.close();
                                    break;
                                }
                            }
                        }
                        // systemcatalog da empty page i degistir
                        sc.close();
                        File syc = new File("SystemCatalog.txt");
                        sc = new Scanner(syc);
                        File ff = new File("Temp");
                        if(!ff.exists())
                            ff.createNewFile();
                        FileWriter fw = new FileWriter(ff.getAbsoluteFile());
                        while(sc.hasNextLine()){
                            String nl = sc.nextLine();
                            String[] nls = nl.split(" ");
                            if( nls[0].equals(recordType) ){
                                nls[nls.length - 2] = Integer.toString(emptyPage);
                                if(editFlag)
                                    nls[nls.length - 1] = Integer.toString( Integer.parseInt(nls[nls.length - 1]) + 1);
                                String t = "";
                                for( String a : nls)
                                    t = t + a + " ";
                                fw.write(t);
                            }
                            else
                                fw.write(nl);
                            fw.write("\n");
                        }
                        fw.close();
                        sc.close();
                        syc.delete();
                        ff.renameTo(new File("SystemCatalog.txt"));

                    }
                    writePage(Integer.parseInt(st[st.length-2]) , by);
                    System.out.println("Record was created");
                }
                else{
                    System.out.println("Record can't created");
                    sc.close();
                    return;
                }
                break;
            }
        }
        sc.close();
    }

    public static void searchRecord(String recordType , int primarykey) throws IOException{
        Scanner df = new Scanner(new File(recordType + "_DataFile.txt"));
        Scanner sc = new Scanner(new File("SystemCatalog.txt"));
        String s;
        String[] ss;
        int recordSize = 1;
        while(sc.hasNextLine()){
            s = sc.nextLine();
            ss = s.split(" ");
            if( ss[0].equals(recordType)){
                recordSize = Integer.parseInt(ss[1])*4+1;
                break;
            }
        }
        while(df.hasNextInt()){
            //System.out.println(df.nextInt());
            int pageid = df.nextInt();
            byte[] by = readPage(pageid);
            for(int i = 6 ; i + recordSize < 1000 ; i = i+recordSize){
                byte[] bf = {by[i+1] , by[i+2], by[i+3], by[i+4]};
                ByteBuffer bb = ByteBuffer.wrap(bf);
                if( primarykey == bb.getInt() && by[i] == 1 ){
                    System.out.print("Record found: " + recordType + " " + Integer.toString(primarykey) + " ");//Add other fields
                    for( int k = 0 ; k < (recordSize-1)/4 - 1 ; k++ ){
                        byte[] ba = { by[i+5+k*4] , by[i+6+k*4] , by[i+7+k*4] , by[i+8+k*4]};
                        bb = ByteBuffer.wrap(ba);
                        System.out.print(Integer.toString(bb.getInt()) + " ");
                    }
                    System.out.println();
                    df.close();
                    sc.close();
                    return;
                }
            }
        }
        df.close();
        sc.close();
        System.out.println("Record didn't found on database");
    }

    public static void deleteRecord(String recordType , int primarykey) throws IOException{
        Scanner df = new Scanner(new File(recordType + "_DataFile.txt"));
        Scanner sc = new Scanner(new File("SystemCatalog.txt"));
        String s;
        String[] ss;
        int recordSize = 1;
        while(sc.hasNextLine()){
            s = sc.nextLine();
            ss = s.split(" ");
            if( ss[0].equals(recordType)){
                recordSize = Integer.parseInt(ss[1])*4+1;
                break;
            }
        }
        while(df.hasNextInt()){
            int pageid = df.nextInt();
            byte[] by = readPage(pageid);
            for(int i = 6 ; i + recordSize < 1000 ; i = i+recordSize){
                byte[] bf = {by[i+1] , by[i+2], by[i+3], by[i+4]};
                ByteBuffer bb = ByteBuffer.wrap(bf);
                if( primarykey == bb.getInt() && by[i] == 1 ){
                    System.out.println("Record found and deleted");
                    by[i] = 0;
                    byte[] bff = {by[2] , by[3] , by[4] , by[5]};
                    bb = ByteBuffer.wrap(bff);
                    int nor = bb.getInt();
                    nor--;
                    bb = ByteBuffer.allocate(4);
                    bb.putInt(nor);
                    byte[] re = bb.array();
                    by[2] = re[0];
                    by[3] = re[1];
                    by[4] = re[2];
                    by[5] = re[3];
                    if ( by[1] == 1 ){
                       by[1] = 0;
                    }
                    writePage(pageid , by);
                    df.close();
                    sc.close();
                    return;
                }
            }
        }
        df.close();
        sc.close();
    }

    public static void listRecords(String recordType) throws IOException{
        Scanner df = new Scanner(new File(recordType + "_DataFile.txt"));
        Scanner sc = new Scanner(new File("SystemCatalog.txt"));
        String s;
        String[] ss;
        int recordSize = 1;
        while(sc.hasNextLine()){
            s = sc.nextLine();
            ss = s.split(" ");
            if( ss[0].equals(recordType)){
                recordSize = Integer.parseInt(ss[1])*4+1;
                break;
            }
        }
        while(df.hasNextInt()){
            int pageid = df.nextInt();
            byte[] by = readPage(pageid);
            for(int i = 6 ; i + recordSize < 1000 ; i = i+recordSize){
                if( by[i] == 1 ){
                    int j = i+1;
                    for(int k = 0 ; k < (recordSize-1 )/4 ; k++){
                        byte[] ba = { by[j] , by[j+1] , by[j+2] , by[j+3]};
                        ByteBuffer bb = ByteBuffer.wrap(ba);
                        System.out.print(bb.getInt());
                        System.out.print(" ");
                        j = j+4;
                    }
                    System.out.println();
                }
            }
        }
        df.close();
        sc.close();
    }

}

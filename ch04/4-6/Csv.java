import java.io.BufferedReader;
import java.io.IOException;
import java.util.ArrayList;

public class Csv {
    private final int PREALLOCATE = 16;

    private char    fieldSep;
    private String  line;

    private StringBuilder field;
    private int nField;
    private ArrayList<String> fields;

    private BufferedReader reader;

    Csv(BufferedReader reader, char sep) {
        this.reader = reader;
        this.line = null;
        this.fieldSep = sep;
        this.nField = 0;
        this.fields = new ArrayList<String>(this.PREALLOCATE);
        this.field = new StringBuilder();
    }

    Csv(BufferedReader reader) {
        this(reader, ',');
    }

    public String getLine() {
        try {
            this.line = reader.readLine();
            if (line == null)
                return null;
            split();
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
        return line;
    }
    
    public String getField(int n) {
        if (n >= 0 && n < nField)
            return fields.get(n);
        return null;
    }
    
    public int getNfield() {
        return this.nField;
    }

    private void split() {
        fields.clear();
        if (line.length() == 0)
            return;

        boolean quote = false;
        field.setLength(0);
        nField = 0;
        for (int i = 0; i < line.length(); i++) {
            char c = line.charAt(i);
            if (c == '"') {
                quote = !quote;
            } else if (c == ',' && !quote) {
                fields.add(field.toString());
                field.setLength(0);
                nField++;
            } else { 
                field.append(c);
            }
        }
        if (field.length() > 0) {
            fields.add(field.toString());
            nField++;
        }
    }
    
}


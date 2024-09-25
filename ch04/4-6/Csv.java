import java.io.*;
import java.util.ArrayList;

public class Csv {
    private final int PREALLOCATE = 16;

    private char fieldSep;
    private String line;
    private char[] charBuffer;
    private int nField;
    private ArrayList<String> fields;

    private BufferedReader reader;

    Csv(BufferedReader reader, char sep) {
        this.reader = reader;
        this.fieldSep = sep;
        this.fields = new ArrayList<>(PREALLOCATE);
    }

    Csv(BufferedReader reader) {
        this(reader, ',');
    }

    public String getLine() {
        try {
            this.line = reader.readLine();
            if (line == null) return null;
            this.charBuffer = line.toCharArray();  // Convert once to char array for faster access
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
        if (charBuffer.length == 0) return;

        boolean quote = false;
        int start = 0;
        nField = 0;

        for (int i = 0; i < charBuffer.length; i++) {
            char c = charBuffer[i];
            if (c == '"') {
                quote = !quote;
            } else if (c == fieldSep && !quote) {
                fields.add(new String(charBuffer, start, i - start));  // Avoid extra StringBuilder
                start = i + 1;
                nField++;
            }
        }
        // Add the last field
        if (start < charBuffer.length) {
            fields.add(new String(charBuffer, start, charBuffer.length - start));
            nField++;
        }
    }
}


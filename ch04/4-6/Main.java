import java.io.BufferedReader;
import java.io.InputStreamReader;

public class Main {
    public static void main(String[] args) {
        int nField;
        String line;
        Csv csv = new Csv(new BufferedReader(new InputStreamReader(System.in)));
        for (int i = 0; i < 20; i++) {
            System.out.println((20-i) + " seconds to launch...");
            Thread.sleep(1000);
        }
        while ((line = csv.getLine()) != null) {
            System.out.println("line = '" + line + "'");
            nField = csv.getNfield();
            for (int i = 0; i < nField; i++)
                System.out.println("field["+i+"] = '" + csv.getField(i) + "'");
        }
    }
}

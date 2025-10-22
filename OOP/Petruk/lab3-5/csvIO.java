import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.util.ArrayList;

public class csvIO {

    public static void fillingTable(ArrayList<String[]> arrList) {
        String csvFile = "/Users/kyo/uni/oop/lab5/src/data.csv";
        String line;
        String delimiter = ";";
        try (BufferedReader br = new BufferedReader(new FileReader(csvFile))) {
            while ((line = br.readLine()) != null) {
                String[] values = line.split(delimiter);
                arrList.add(values);
            }
        } catch (FileNotFoundException e) {
            System.out.println("File not found: " + e.getMessage());
        } catch (IOException e) {
            System.out.println("IO exception: " + e.getMessage());
        } catch (Exception e) {
            System.out.println("Error: " + e.getMessage());
        }
    }

    public static void addSaveButton(JPanel panel) {
        JButton saveButton = new JButton("Save and exit");
        saveButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                try {
                    BufferedWriter writer = new BufferedWriter (new FileWriter("src/data.csv"));
                    for (int i = 0; i< lab5.model.getRowCount(); i++) {
                        for (int j = 0; j< lab5.model.getColumnCount(); j++) {
                            writer.write((lab5.model.getValueAt(i,j)).toString());
                            writer.write(";");
                        }
                        writer.write("\n");
                    }
                    writer.close();
                } catch(IOException io) { io.printStackTrace(); }
                lab5.appFrame.setVisible(false);
                lab5.appFrame.dispose();
            }
        });
        panel.add(saveButton);
    }

    public static void main() {

    }
}
import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;

public class lab2 {
    private DefaultTableModel model;
    private JFrame appFrame;
        private JButton edit;
        private JButton add;
        private JButton delete;
        private JToolBar toolBar;
        private JTable list;
        private JTextField driverName;
        private JComboBox driver;
        private JScrollPane scroll;

        public void show() {
            // setting up the app frame
            appFrame = new JFrame("Bus Parking");
            appFrame.setSize(500,300);
            appFrame.setLocation(100,100);
            appFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

            // adding buttons
            edit = new JButton("Edit");
            add = new JButton("Add");
            delete = new JButton("Delete");

            // tips while hovering the buttons
            edit.setToolTipText("Save drivers list");
            add.setToolTipText("Add new element to the table");
            delete.setToolTipText("Export the table to a .txt file");

            // setting up toolbar and adding buttons
            toolBar = new JToolBar("Toolbar");
            toolBar.add(edit);
            toolBar.add(add);
            toolBar.add(delete);
            appFrame.setLayout(new BorderLayout());
            appFrame.add(toolBar, BorderLayout.NORTH);

            // filling the table out
            String[] options = {"Bus", "Driver", "Route", "Schedule", "Violations"};
            String[][] data = {{"57", "Pyotr Petrov", "Mostovik-Sergiev Posad", "6AM-10PM", "None"},{"75K", "Ivan Ivanov", "Vasilyevskoe-Mostovik", "8AM-11PM", "Late to work: 15min!"}};
            model = new DefaultTableModel(data, options);
            list = new JTable(model);
            scroll = new JScrollPane(list);

            appFrame.add(scroll, BorderLayout.CENTER);

            // search bar at the bottom
            JLabel search_label = new JLabel("Search by");
            driver = new JComboBox(new String[]{"Driver", "Bus", "Route"});
            driverName = new JTextField("Enter..");
            JButton filter = new JButton("Search");
            JPanel filterPanel = new JPanel();
            filterPanel.add(search_label);
            filterPanel.add(driver);
            filterPanel.add(driverName);
            filterPanel.add(filter);
            appFrame.add(filterPanel, BorderLayout.SOUTH);

            appFrame.setVisible(true);
    }

    public static void main(String[] args) {
        new lab2().show();
    }
}

import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class lab5 {
    public static DefaultTableModel model;
    public static JFrame appFrame;

    private JFrame editFrame;
    private JFrame addFrame;
    private JFrame deleteFrame;

    private JButton editButton;
    private JButton addButton;
    private JButton deleteButton;
    private JToolBar toolBar;
    private JTable busList;
    private JTextField driverName;
    private JComboBox driver;
    private JScrollPane scroll;

    private ArrayList<String[]> data = new ArrayList<String[]>();
    private ArrayList<String> options = new ArrayList<String>(Arrays.asList("Bus", "Driver", "Route", "Schedule", "Violations"));
    private JComboBox box;
    private boolean flag = false;

    public void frameShow(JFrame frame, String name) {
            frame = new JFrame(name);
            frame.pack();

        frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
    }

    // declaring an exception and the standard output
    private class emptyException extends Exception {
        public emptyException() {
            super("Wrong input! Try again..");
        }}

    private void isArrayEmpty( String[] arr) throws emptyException {
        for (String i: arr) {
            if (i.isEmpty()) {
                flag = true;
                throw new emptyException();
            } else {
                flag = false;
            }
        }
    }

    private void panelWithExitButton(JFrame frame) {
            JButton exit = new JButton("Exit without saving");
            exit.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    frame.setVisible(false);
                    frame.dispose();
                }
            });
        JPanel panel = new JPanel();
        BorderLayout layout = new BorderLayout();
        panel.add(exit);
        frame.add(panel, BorderLayout.NORTH);
        }

    private void editFrameDisplay() {
        panelWithExitButton(editFrame);
        JButton selectButton = new JButton("Select");
        JPanel editPanel = new JPanel();

        ArrayList<String> buses = new ArrayList<String>();
        for (int i = 0; i < busList.getRowCount(); i++) {
            buses.add(busList.getValueAt(i,0).toString());
        }

        box = new JComboBox(buses.toArray());
        BorderLayout layout = new BorderLayout();
        editPanel.add(selectButton);
        editFrame.add(editPanel, BorderLayout.SOUTH);
        editFrame.add(box, BorderLayout.CENTER);

        selectButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                JFrame editingWindow = new JFrame();
                editFrame.setVisible(false);
                editFrame.dispose();

                String busToEdit = box.getSelectedItem().toString();
                for (int i = model.getRowCount() - 1; i >= 0; i--) {
                    if (model.getValueAt(i, 0) == busToEdit) {
                        JTextField bus = new JTextField(model.getValueAt(i,0).toString());
                        JTextField driver = new JTextField(model.getValueAt(i,1).toString());
                        JTextField route = new JTextField(model.getValueAt(i,2).toString());
                        JTextField schedule = new JTextField(model.getValueAt(i,3).toString());
                        JTextField violations = new JTextField(model.getValueAt(i,4).toString());

                        JPanel panel = new JPanel();
                        panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));

                        panel.add(new JLabel("Bus"));
                        panel.add(bus);
                        panel.add(new JLabel("Driver"));
                        panel.add(driver);
                        panel.add(new JLabel("Route"));
                        panel.add(route);
                        panel.add(new JLabel("Schedule"));
                        panel.add(schedule);
                        panel.add(new JLabel("Violations"));
                        panel.add(violations);

                        JButton confirmButton = new JButton("Confirm changes");
                        panel.add(confirmButton);

                        panel.add(Box.createVerticalStrut(400));

                        editingWindow.add(panel);
                        editingWindow.setVisible(true);
                        editingWindow.setSize(new Dimension(800,800));

                        confirmButton.addActionListener(new ActionListener() {
                            public void actionPerformed(ActionEvent e) {
                                String[] newRow = {bus.getText(), driver.getText(), route.getText(), schedule.getText(), violations.getText()};
                                for (int i = model.getRowCount() - 1; i >= 0; i--) {
                                    String[] oldRow = {model.getValueAt(i, 0).toString(), model.getValueAt(i, 1).toString(), model.getValueAt(i, 2).toString(), model.getValueAt(i, 3).toString(), model.getValueAt(i, 4).toString()};
                                    if (model.getValueAt(i, 0) == busToEdit) {
                                        // checking if array is empty with exception handling
                                        try { isArrayEmpty(newRow); }
                                        catch (emptyException ex) {
                                            JOptionPane.showMessageDialog(null, ex.getMessage());
                                            // addFrame.setVisible(false);
                                            // addFrame.dispose();
                                        }
                                        if (!flag) {
                                            if (newRow != oldRow) {
                                                model.removeRow(i);
                                                box.removeItemAt(i);
                                                data.remove(i);

                                                box.addItem(newRow[0]);
                                                model.addRow(newRow);
                                                data.add(newRow);
                                                JOptionPane.showMessageDialog(null, "Selected element edited.");
                                                editingWindow.setVisible(false);
                                                editingWindow.dispose();
                                            }
                                        }
                                    }
                                }
                            }
                            });
                    }
                }
            }
        });
    }


    private void addFrameDisplay() {
            panelWithExitButton(addFrame);

           JTextField bus = new JTextField();
           JTextField driver = new JTextField();
           JTextField route = new JTextField();
           JTextField schedule = new JTextField();
           JTextField violations = new JTextField();

           JPanel addPanel = new JPanel();
           addPanel.setLayout(new BoxLayout(addPanel, BoxLayout.Y_AXIS));


        addPanel.add(new JLabel("Bus"));
           addPanel.add(bus);
        addPanel.add(new JLabel("Driver"));
           addPanel.add(driver);
        addPanel.add(new JLabel("Route"));
           addPanel.add(route);
        addPanel.add(new JLabel("Schedule"));
           addPanel.add(schedule);
        addPanel.add(new JLabel("Violations"));
           addPanel.add(violations);

           addPanel.add(Box.createVerticalStrut(400));

           addFrame.add(addPanel);

           JButton addButton = new JButton("Add");
           addButton.addActionListener(new ActionListener() {
               public void actionPerformed(ActionEvent e) {
                   String[] newElement_array = {bus.getText(), driver.getText(), route.getText(), schedule.getText(), violations.getText()};
                       // if (bus.getText().isEmpty() || driver.getText().isEmpty() || route.getText().isEmpty() || schedule.getText().isEmpty() || violations.getText().isEmpty()) {
                   // checking if array is empty with exception handling
                   try { isArrayEmpty(newElement_array); }
                   catch (emptyException ex) {
                       JOptionPane.showMessageDialog(null, ex.getMessage());
                       // addFrame.setVisible(false);
                       // addFrame.dispose();
                   }
                   if (!flag) {
                       model.addRow(newElement_array);
                       data.add(newElement_array);
                       JOptionPane.showMessageDialog(null, "New element added.");
                       addFrame.setVisible(false);
                       addFrame.dispose();
                   }
               }
           });
           addFrame.add(addButton, BorderLayout.SOUTH);
        }

        // TODO: fix bug with 'buses' contents not updating when deleting the element - DONE
        private void deleteFrameDisplay() {
            panelWithExitButton(deleteFrame);
            ArrayList<String> buses = new ArrayList<String>();
            for (int i = 0; i < busList.getRowCount(); i++) {
                buses.add(busList.getValueAt(i,0).toString());
            }

            JLabel note = new JLabel("Select a bus to delete");
            box = new JComboBox(buses.toArray());
            deleteFrame.add(note);
            deleteFrame.add(box);

            JButton deleteButton = new JButton("Delete");
            deleteButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    String busToDelete = box.getSelectedItem().toString();
                    for (int i=model.getRowCount()-1; i>=0; i--) {
                        if (model.getValueAt(i, 0).equals(busToDelete)) {
                            model.removeRow(i);
                            buses.remove(i);
                            box.removeItemAt(i);
                            JOptionPane.showMessageDialog(null, "Element is deleted.");
                        }
                    }

                    deleteFrame.setVisible(false);
                    deleteFrame.dispose();
                }
            });

            JPanel deletePanel = new JPanel();
            deletePanel.add(deleteButton);
            BorderLayout layout = new BorderLayout();
            deleteFrame.add(deletePanel, BorderLayout.SOUTH);
    }

        public void show() {
            // setting up the app frame
            appFrame = new JFrame();
            editFrame = new JFrame();
            addFrame = new JFrame();
            deleteFrame = new JFrame();

            // adding buttons
            editButton = new JButton("Edit");
            editButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    ArrayList<String> buses = new ArrayList<String>();
                    for (int i = 0; i < busList.getRowCount(); i++) {
                        buses.add(busList.getValueAt(i,0).toString());
                    }
                    frameShow(editFrame, "Editing");
                    editFrameDisplay();
                    editFrame.setVisible(true);
                    editFrame.setSize(new Dimension(800,800));
                }
            });

            addButton = new JButton("Add");
            addButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    frameShow(addFrame, "Adding");
                    addFrameDisplay();
                    addFrame.setVisible(true);
                    addFrame.setSize(new Dimension(800,800));

                }
            });
            deleteButton = new JButton("Delete");
            deleteButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    ArrayList<String> buses = new ArrayList<String>();
                    for (int i = 0; i < busList.getRowCount(); i++) {
                        buses.add(busList.getValueAt(i,0).toString());
                    }
                    frameShow(deleteFrame, "Delete");
                    deleteFrameDisplay();
                    deleteFrame.setVisible(true);
                    deleteFrame.setSize(new Dimension(800,800));
                }
            });
            // tips while hovering the buttons
            editButton.setToolTipText("Edit bus table");
            addButton.setToolTipText("Add new element to the table");
            deleteButton.setToolTipText("Delete bus from table");

            // setting up toolbar and adding buttons
            toolBar = new JToolBar("Toolbar");
            toolBar.add(editButton);
            toolBar.add(addButton);
            toolBar.add(deleteButton);
            appFrame.setLayout(new BorderLayout());
            appFrame.add(toolBar, BorderLayout.NORTH);

            // filling the table out
            csvIO.fillingTable(data);
            model = new DefaultTableModel(data.toArray(new String[0][]), options.toArray(new String[0]));
            busList = new JTable(model);
            scroll = new JScrollPane(busList);
            busList.setDefaultEditor(Object.class, null);

            appFrame.add(scroll, BorderLayout.CENTER);

            // save button at the bottom panel
            JPanel bottomPanel = new JPanel();
            csvIO.addSaveButton(bottomPanel);
            appFrame.add(bottomPanel, BorderLayout.SOUTH);

            frameShow(appFrame, "Bus Parking");
            appFrame.setVisible(true);
            appFrame.setSize(new Dimension(1440,900));
    }

    public static void main(String[] args) {
        new lab5().show();
    }
}




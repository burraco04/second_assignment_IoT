import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.IOException;
import java.time.LocalTime;
import java.time.format.DateTimeFormatter;
import java.util.List;
import java.util.Locale;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;

public final class DroneRemoteUnitFrame extends JFrame {

  private static final int BAUD_RATE = 115200;
  private static final DateTimeFormatter LOG_TIME = DateTimeFormatter.ofPattern("HH:mm:ss");

  private final JComboBox<String> portSelector = new JComboBox<>();
  private final JButton refreshButton = new JButton("Refresh");
  private final JButton connectButton = new JButton("Connect");
  private final JButton takeOffButton = new JButton("Take off");
  private final JButton landButton = new JButton("Land");
  private final JButton resetButton = new JButton("Reset");
  private final JTextField rawCommandField = new JTextField(14);
  private final JButton rawCommandButton = new JButton("Send");
  private final JLabel droneStateLabel = valueLabel("Resting");
  private final JLabel hangarStateLabel = valueLabel("Normal");
  private final JLabel distanceLabel = valueLabel("--");
  private final JLabel connectionLabel = valueLabel("Disconnected");
  private final JTextArea logArea = new JTextArea(14, 70);

  private SerialConnection connection;

  public DroneRemoteUnitFrame() {
    super("Drone Remote Unit");
    setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
    setLayout(new BorderLayout(10, 10));

    add(buildConnectionPanel(), BorderLayout.NORTH);
    add(buildStatusPanel(), BorderLayout.CENTER);
    add(buildBottomPanel(), BorderLayout.SOUTH);

    refreshPorts();
    setControlsConnected(false);
    pack();
    setLocationRelativeTo(null);

    addWindowListener(new WindowAdapter() {
      @Override
      public void windowClosed(WindowEvent event) {
        disconnect();
      }
    });
  }

  private JPanel buildConnectionPanel() {
    JPanel panel = new JPanel(new FlowLayout(FlowLayout.LEFT));
    panel.setBorder(BorderFactory.createEmptyBorder(10, 10, 0, 10));

    portSelector.setEditable(true);
    refreshButton.addActionListener(event -> refreshPorts());
    connectButton.addActionListener(event -> toggleConnection());

    panel.add(new JLabel("Serial port"));
    panel.add(portSelector);
    panel.add(refreshButton);
    panel.add(connectButton);
    panel.add(new JLabel("Status"));
    panel.add(connectionLabel);
    return panel;
  }

  private JPanel buildStatusPanel() {
    JPanel panel = new JPanel(new BorderLayout(10, 10));
    panel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

    JPanel states = new JPanel(new GridBagLayout());
    states.setBorder(BorderFactory.createTitledBorder("System state"));
    addStateRow(states, 0, "Drone", droneStateLabel);
    addStateRow(states, 1, "Hangar", hangarStateLabel);
    addStateRow(states, 2, "Landing distance", distanceLabel);

    logArea.setEditable(false);
    logArea.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 12));
    JScrollPane scrollPane = new JScrollPane(logArea);
    scrollPane.setBorder(BorderFactory.createTitledBorder("Serial log"));

    panel.add(states, BorderLayout.NORTH);
    panel.add(scrollPane, BorderLayout.CENTER);
    return panel;
  }

  private JPanel buildBottomPanel() {
    JPanel panel = new JPanel(new BorderLayout(10, 10));
    panel.setBorder(BorderFactory.createEmptyBorder(0, 10, 10, 10));

    JPanel commands = new JPanel(new FlowLayout(FlowLayout.LEFT));
    takeOffButton.addActionListener(event -> sendCommand("TAKEOFF"));
    landButton.addActionListener(event -> sendCommand("LAND"));
    resetButton.addActionListener(event -> sendCommand("RESET"));
    commands.add(takeOffButton);
    commands.add(landButton);
    commands.add(resetButton);

    JPanel raw = new JPanel(new FlowLayout(FlowLayout.RIGHT));
    rawCommandField.setToolTipText("Optional raw serial command");
    rawCommandButton.addActionListener(event -> sendRawCommand());
    raw.add(new JLabel("Raw"));
    raw.add(rawCommandField);
    raw.add(rawCommandButton);

    panel.add(commands, BorderLayout.WEST);
    panel.add(raw, BorderLayout.EAST);
    return panel;
  }

  private void addStateRow(JPanel panel, int row, String label, Component value) {
    GridBagConstraints labelConstraints = new GridBagConstraints();
    labelConstraints.gridx = 0;
    labelConstraints.gridy = row;
    labelConstraints.anchor = GridBagConstraints.WEST;
    labelConstraints.insets = new Insets(6, 8, 6, 16);
    panel.add(new JLabel(label), labelConstraints);

    GridBagConstraints valueConstraints = new GridBagConstraints();
    valueConstraints.gridx = 1;
    valueConstraints.gridy = row;
    valueConstraints.weightx = 1.0;
    valueConstraints.fill = GridBagConstraints.HORIZONTAL;
    valueConstraints.insets = new Insets(6, 8, 6, 8);
    panel.add(value, valueConstraints);
  }

  private static JLabel valueLabel(String value) {
    JLabel label = new JLabel(value);
    label.setFont(label.getFont().deriveFont(Font.BOLD, 15f));
    return label;
  }

  private void refreshPorts() {
    Object selected = portSelector.getSelectedItem();
    portSelector.removeAllItems();

    List<String> ports = SerialPorts.listAvailablePorts();
    if (ports.isEmpty()) {
      portSelector.addItem("COM3");
    } else {
      for (String port : ports) {
        portSelector.addItem(port);
      }
    }

    if (selected != null) {
      portSelector.setSelectedItem(selected);
    }
  }

  private void toggleConnection() {
    if (connection != null && connection.isOpen()) {
      disconnect();
    } else {
      connect();
    }
  }

  private void connect() {
    Object selectedPort = portSelector.getSelectedItem();
    if (selectedPort == null || selectedPort.toString().trim().isEmpty()) {
      showError("Select a serial port first.");
      return;
    }

    connection = new JsscSerialConnection();
    String port = selectedPort.toString().trim();

    try {
      connection.open(port, BAUD_RATE, line -> SwingUtilities.invokeLater(() -> handleIncomingLine(line)));
      setControlsConnected(true);
      connectionLabel.setText("Connected to " + port);
      connectionLabel.setForeground(new Color(0, 120, 0));
      appendLog("Connected to " + port + " @ " + BAUD_RATE);
    } catch (IOException ex) {
      connection = null;
      setControlsConnected(false);
      showError("Cannot open " + port + ": " + ex.getMessage());
    }
  }

  private void disconnect() {
    if (connection == null) {
      return;
    }

    try {
      connection.close();
    } catch (IOException ex) {
      appendLog("Disconnect error: " + ex.getMessage());
    } finally {
      connection = null;
      setControlsConnected(false);
      connectionLabel.setText("Disconnected");
      connectionLabel.setForeground(Color.DARK_GRAY);
      appendLog("Disconnected");
    }
  }

  private void setControlsConnected(boolean connected) {
    connectButton.setText(connected ? "Disconnect" : "Connect");
    takeOffButton.setEnabled(connected);
    landButton.setEnabled(connected);
    resetButton.setEnabled(connected);
    rawCommandField.setEnabled(connected);
    rawCommandButton.setEnabled(connected);
    portSelector.setEnabled(!connected);
    refreshButton.setEnabled(!connected);
  }

  private void sendRawCommand() {
    String command = rawCommandField.getText().trim();
    if (!command.isEmpty()) {
      sendCommand(command);
      rawCommandField.setText("");
    }
  }

  private void sendCommand(String command) {
    if (connection == null || !connection.isOpen()) {
      showError("Serial connection is not open.");
      return;
    }

    try {
      connection.sendLine(command);
      appendLog("> " + command);
    } catch (IOException ex) {
      showError("Cannot send command: " + ex.getMessage());
    }
  }

  private void handleIncomingLine(String line) {
    appendLog("< " + line);
    ProtocolEvent event = ProtocolParser.parse(line);

    switch (event.getType()) {
      case DRONE_STATE:
        droneStateLabel.setText(formatDroneState(event.getValue()));
        break;
      case HANGAR_STATE:
        updateHangarState(event.getValue());
        break;
      case DISTANCE:
        distanceLabel.setText(formatDistance(event.getValue()));
        break;
      case ERROR:
        showError(event.getValue());
        break;
      case LOG:
      case UNKNOWN:
        break;
    }
  }

  private void updateHangarState(String state) {
    String normalized = state.toUpperCase(Locale.ROOT);
    hangarStateLabel.setText(formatHangarState(normalized));
    if ("ALARM".equals(normalized)) {
      hangarStateLabel.setForeground(Color.RED.darker());
    } else if ("PRE_ALARM".equals(normalized)) {
      hangarStateLabel.setForeground(new Color(180, 100, 0));
    } else {
      hangarStateLabel.setForeground(new Color(0, 120, 0));
    }
  }

  private static String formatDroneState(String state) {
    return switch (state.toUpperCase(Locale.ROOT)) {
      case "RESTING" -> "Rest";
      case "TAKING_OFF" -> "Taking off";
      case "FLYING" -> "Operating";
      case "LANDING" -> "Landing";
      case "SUSPENDED" -> "Suspended";
      default -> state;
    };
  }

  private static String formatHangarState(String state) {
    return switch (state.toUpperCase(Locale.ROOT)) {
      case "NORMAL" -> "Normal";
      case "PRE_ALARM" -> "Pre-alarm";
      case "ALARM" -> "Alarm";
      default -> state;
    };
  }

  private static String formatDistance(String value) {
    try {
      double distance = Double.parseDouble(value.trim());
      if (distance < 0) {
        return "No object";
      }
      return String.format(Locale.US, "%.2f m", distance);
    } catch (NumberFormatException ex) {
      return value;
    }
  }

  private void appendLog(String message) {
    logArea.append("[" + LocalTime.now().format(LOG_TIME) + "] " + message + System.lineSeparator());
    logArea.setCaretPosition(logArea.getDocument().getLength());
  }

  private void showError(String message) {
    appendLog("ERROR: " + message);
    JOptionPane.showMessageDialog(this, message, "Drone Remote Unit", JOptionPane.ERROR_MESSAGE);
  }
}

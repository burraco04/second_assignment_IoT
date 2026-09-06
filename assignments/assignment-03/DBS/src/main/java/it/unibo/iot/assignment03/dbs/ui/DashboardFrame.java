package it.unibo.iot.assignment03.dbs.ui;

import it.unibo.iot.assignment03.dbs.config.DbsConfig;
import it.unibo.iot.assignment03.dbs.http.CusHttpClient;
import it.unibo.iot.assignment03.dbs.model.CusState;
import it.unibo.iot.assignment03.dbs.model.SystemMode;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.util.concurrent.CompletableFuture;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.SwingUtilities;
import javax.swing.Timer;

public final class DashboardFrame extends JFrame {
    private static final Color OK = new Color(0, 120, 0);
    private static final Color WARN = new Color(170, 100, 0);
    private static final Color BAD = new Color(170, 0, 0);

    private final CusHttpClient client;
    private final HistoryChartPanel chartPanel;
    private final JLabel cusStatus = new JLabel("CUS: NOT AVAILABLE");
    private final JLabel modeValue = new JLabel("NOT_AVAILABLE");
    private final JLabel waterLevelValue = new JLabel("--%");
    private final JLabel valveOpeningValue = new JLabel("--%");
    private final JLabel mqttValue = new JLabel("--");
    private final JLabel wcsValue = new JLabel("--");
    private final JLabel sampleCount = new JLabel("0 samples");
    private final JLabel commandStatus = new JLabel(" ");
    private final JButton automaticButton = new JButton("Automatic");
    private final JButton manualButton = new JButton("Manual");
    private final JButton applyValveButton = new JButton("Apply");
    private final JSlider valveSlider = new JSlider(0, 100, 0);
    private volatile boolean polling;
    private boolean programmaticSliderUpdate;
    private boolean userEditedValve;
    private Integer pendingValveOpening;

    public DashboardFrame(final DbsConfig config, final CusHttpClient client) {
        super("Smart Tank DBS");
        this.client = client;
        this.chartPanel = new HistoryChartPanel(config.historySize());

        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setMinimumSize(new Dimension(850, 520));
        setContentPane(buildContent());
        wireCommands();
        pack();
        setLocationRelativeTo(null);

        final Timer timer = new Timer(config.pollPeriodMs(), ignored -> pollState());
        timer.setInitialDelay(0);
        timer.start();
    }

    private JPanel buildContent() {
        final JPanel root = new JPanel(new BorderLayout(10, 10));
        root.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

        root.add(new JLabel("Smart Tank Monitoring Dashboard"), BorderLayout.NORTH);

        final JPanel main = new JPanel(new BorderLayout(10, 10));
        main.add(statusPanel(), BorderLayout.NORTH);
        main.add(chartPanel(), BorderLayout.CENTER);
        main.add(controlPanel(), BorderLayout.SOUTH);
        root.add(main, BorderLayout.CENTER);
        return root;
    }

    private JPanel statusPanel() {
        final JPanel panel = new JPanel(new GridLayout(2, 4, 8, 4));
        panel.setBorder(BorderFactory.createTitledBorder("State"));
        panel.add(new JLabel("CUS"));
        panel.add(new JLabel("Mode"));
        panel.add(new JLabel("Water level"));
        panel.add(new JLabel("Valve"));
        panel.add(cusStatus);
        panel.add(modeValue);
        panel.add(waterLevelValue);
        panel.add(valveOpeningValue);

        final JPanel wrapper = new JPanel(new BorderLayout(0, 8));
        wrapper.add(panel, BorderLayout.CENTER);
        wrapper.add(linkPanel(), BorderLayout.SOUTH);
        return wrapper;
    }

    private JPanel linkPanel() {
        final JPanel panel = new JPanel(new FlowLayout(FlowLayout.LEFT));
        panel.add(new JLabel("MQTT:"));
        panel.add(mqttValue);
        panel.add(new JLabel("WCS:"));
        panel.add(wcsValue);
        return panel;
    }

    private JPanel chartPanel() {
        final JPanel panel = new JPanel(new BorderLayout(0, 4));
        panel.setBorder(BorderFactory.createTitledBorder("Rainwater history"));
        panel.add(sampleCount, BorderLayout.NORTH);
        panel.add(chartPanel, BorderLayout.CENTER);
        return panel;
    }

    private JPanel controlPanel() {
        final JPanel panel = new JPanel(new FlowLayout(FlowLayout.LEFT));
        panel.setBorder(BorderFactory.createTitledBorder("Controls"));
        panel.add(automaticButton);
        panel.add(manualButton);
        panel.add(new JLabel("Valve"));
        valveSlider.setPreferredSize(new Dimension(240, 40));
        panel.add(valveSlider);
        panel.add(applyValveButton);
        panel.add(commandStatus);
        return panel;
    }

    private void wireCommands() {
        automaticButton.addActionListener(ignored -> sendMode(SystemMode.AUTOMATIC));
        manualButton.addActionListener(ignored -> sendMode(SystemMode.MANUAL));
        applyValveButton.addActionListener(ignored -> sendValve());
        valveSlider.addChangeListener(ignored -> {
            if (!programmaticSliderUpdate && valveSlider.isEnabled()) {
                userEditedValve = true;
                commandStatus.setText("Valve selected: " + valveSlider.getValue() + "%");
            }
        });
        setControlsEnabled(false, false);
    }

    private void pollState() {
        if (polling) {
            return;
        }
        polling = true;
        CompletableFuture
            .supplyAsync(() -> {
                try {
                    return client.readState();
                } catch (final Exception ex) {
                    return CusState.notAvailable();
                }
            })
            .thenAccept(state -> SwingUtilities.invokeLater(() -> {
                polling = false;
                render(state);
            }));
    }

    private void render(final CusState state) {
        final boolean available = state.mode() != SystemMode.NOT_AVAILABLE;
        cusStatus.setText(available ? "AVAILABLE" : "NOT AVAILABLE");
        cusStatus.setForeground(available ? OK : BAD);

        modeValue.setText(state.mode().name());
        modeValue.setForeground(colorForMode(state.mode()));
        waterLevelValue.setText(state.waterLevelAvailable() ? state.waterLevel() + "%" : "--%");
        valveOpeningValue.setText(state.valveOpening() + "%");
        mqttValue.setText(state.mqttConnected() ? "OK" : "DOWN");
        mqttValue.setForeground(state.mqttConnected() ? OK : BAD);
        wcsValue.setText(state.wcsConnected() ? "OK" : "DOWN");
        wcsValue.setForeground(state.wcsConnected() ? OK : BAD);

        if (state.waterLevelAvailable()) {
            chartPanel.addSample(state.waterLevel());
        }
        sampleCount.setText(chartPanel.sampleCount() + " samples");

        updateValveSlider(state);
        setControlsEnabled(available, state.mode() == SystemMode.MANUAL);
    }

    private void updateValveSlider(final CusState state) {
        if (state.mode() != SystemMode.MANUAL) {
            userEditedValve = false;
            pendingValveOpening = null;
            setSliderValue(state.valveOpening());
            return;
        }

        if (pendingValveOpening != null && state.valveOpening() == pendingValveOpening) {
            pendingValveOpening = null;
        }

        if (!userEditedValve && pendingValveOpening == null && !valveSlider.getValueIsAdjusting()) {
            setSliderValue(state.valveOpening());
        }
    }

    private void setSliderValue(final int value) {
        programmaticSliderUpdate = true;
        valveSlider.setValue(value);
        programmaticSliderUpdate = false;
    }

    private void setControlsEnabled(final boolean available, final boolean manual) {
        automaticButton.setEnabled(available);
        manualButton.setEnabled(available);
        valveSlider.setEnabled(available && manual);
        applyValveButton.setEnabled(available && manual);
    }

    private void sendMode(final SystemMode mode) {
        commandStatus.setText("Sending " + mode.name());
        CompletableFuture.runAsync(() -> {
            try {
                client.setMode(mode);
                SwingUtilities.invokeLater(() -> commandStatus.setText("Mode accepted"));
            } catch (final Exception ex) {
                SwingUtilities.invokeLater(() -> commandStatus.setText("Mode command failed"));
            }
        });
    }

    private void sendValve() {
        final int opening = valveSlider.getValue();
        pendingValveOpening = opening;
        userEditedValve = false;
        commandStatus.setText("Sending valve " + opening + "%");
        CompletableFuture.runAsync(() -> {
            try {
                client.setValveOpening(opening);
                SwingUtilities.invokeLater(() -> commandStatus.setText("Valve accepted"));
            } catch (final Exception ex) {
                SwingUtilities.invokeLater(() -> {
                    pendingValveOpening = null;
                    commandStatus.setText("Valve command failed");
                });
            }
        });
    }

    private static Color colorForMode(final SystemMode mode) {
        return switch (mode) {
            case AUTOMATIC, MANUAL -> OK;
            case UNCONNECTED -> WARN;
            case NOT_AVAILABLE -> BAD;
        };
    }
}

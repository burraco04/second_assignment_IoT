package it.unibo.iot.assignment03.cus.tasks;

import com.fazecast.jSerialComm.SerialPort;
import it.unibo.iot.assignment03.cus.config.CusConfig;
import it.unibo.iot.assignment03.cus.model.CusContext;
import it.unibo.iot.assignment03.cus.model.SystemMode;
import it.unibo.iot.assignment03.cus.scheduler.Task;
import java.nio.charset.StandardCharsets;

public final class SerialTask implements Task {
    private final CusConfig config;
    private final CusContext context;
    private SerialPort port;
    private final StringBuilder lineBuffer = new StringBuilder();

    public SerialTask(final CusConfig config, final CusContext context) {
        this.config = config;
        this.context = context;
    }

    @Override
    public String name() {
        return "Serial task";
    }

    @Override
    public long periodMillis() {
        return config.serialPeriodMs();
    }

    @Override
    public void init() {
        openPort();
    }

    @Override
    public void tick() {
        if (port == null || !port.isOpen()) {
            context.setWcsConnected(false);
            openPort();
            return;
        }

        readSerial();
        writeSerial();
    }

    private void openPort() {
        port = SerialPort.getCommPort(config.serialPort());
        port.setBaudRate(config.serialBaudRate());
        port.setComPortTimeouts(SerialPort.TIMEOUT_NONBLOCKING, 0, 0);
        context.setWcsConnected(port.openPort());
    }

    private void readSerial() {
        final int available = port.bytesAvailable();
        if (available <= 0) {
            return;
        }

        final byte[] buffer = new byte[available];
        final int read = port.readBytes(buffer, buffer.length);
        for (int i = 0; i < read; i++) {
            final char next = (char) buffer[i];
            if (next == '\r') {
                continue;
            }
            if (next == '\n') {
                handleLine(lineBuffer.toString().trim());
                lineBuffer.setLength(0);
            } else {
                lineBuffer.append(next);
            }
        }
    }

    private void handleLine(final String line) {
        if (line.isBlank()) {
            return;
        }
        context.setWcsConnected(true);

        if (line.equals("HELLO WCS") || line.equals("PONG")) {
            return;
        }
        if (line.startsWith("MODE ")) {
            context.requestMode(parseMode(line.substring(5)));
        } else if (line.startsWith("VALVE ")) {
            context.setManualOpening(Integer.parseInt(line.substring(6).trim()), false);
        }
    }

    private void writeSerial() {
        final var snapshot = context.snapshot();

        if (context.consumeModeChanged()) {
            writeLine("MODE " + snapshot.mode().name());
        }

        if (snapshot.mode() == SystemMode.AUTOMATIC && context.consumeValveStateChanged()) {
            writeLine(snapshot.valveState().serialCommand());
        }

        if (snapshot.mode() == SystemMode.MANUAL && context.consumeManualValveChanged()) {
            writeLine("VALVE " + snapshot.manualOpening());
        }
    }

    private void writeLine(final String line) {
        final byte[] bytes = (line + "\n").getBytes(StandardCharsets.US_ASCII);
        port.writeBytes(bytes, bytes.length);
    }

    private static SystemMode parseMode(final String value) {
        return switch (value.trim()) {
            case "MANUAL" -> SystemMode.MANUAL;
            case "AUTOMATIC", "AUTO" -> SystemMode.AUTOMATIC;
            default -> SystemMode.AUTOMATIC;
        };
    }
}


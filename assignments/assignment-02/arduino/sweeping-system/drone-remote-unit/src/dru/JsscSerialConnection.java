import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.function.Consumer;
import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;

public final class JsscSerialConnection implements SerialConnection {

  private final StringBuilder inputBuffer = new StringBuilder();
  private SerialPort serialPort;

  @Override
  public void open(String portName, int baudRate, Consumer<String> lineHandler) throws IOException {
    if (isOpen()) {
      close();
    }

    serialPort = new SerialPort(portName);

    try {
      serialPort.openPort();
      serialPort.setParams(
          baudRate,
          SerialPort.DATABITS_8,
          SerialPort.STOPBITS_1,
          SerialPort.PARITY_NONE);
      serialPort.addEventListener(new LineReader(lineHandler), SerialPort.MASK_RXCHAR);
    } catch (SerialPortException ex) {
      serialPort = null;
      throw toIOException(ex);
    }
  }

  @Override
  public void sendLine(String line) throws IOException {
    if (!isOpen()) {
      throw new IOException("Serial port is not open");
    }

    try {
      serialPort.writeBytes((line + "\n").getBytes(StandardCharsets.US_ASCII));
    } catch (SerialPortException ex) {
      throw toIOException(ex);
    }
  }

  @Override
  public boolean isOpen() {
    return serialPort != null && serialPort.isOpened();
  }

  @Override
  public void close() throws IOException {
    if (serialPort == null) {
      return;
    }

    try {
      if (serialPort.isOpened()) {
        serialPort.removeEventListener();
        serialPort.closePort();
      }
    } catch (SerialPortException ex) {
      throw toIOException(ex);
    } finally {
      serialPort = null;
      inputBuffer.setLength(0);
    }
  }

  private void appendIncoming(String chunk, Consumer<String> lineHandler) {
    synchronized (inputBuffer) {
      for (int i = 0; i < chunk.length(); i++) {
        char ch = chunk.charAt(i);
        if (ch == '\n') {
          String line = inputBuffer.toString().trim();
          inputBuffer.setLength(0);
          if (!line.isEmpty()) {
            lineHandler.accept(line);
          }
        } else if (ch != '\r') {
          inputBuffer.append(ch);
        }
      }
    }
  }

  private static IOException toIOException(SerialPortException ex) {
    return new IOException(ex.getPortName() + " " + ex.getMethodName() + ": " + ex.getExceptionType(), ex);
  }

  private final class LineReader implements SerialPortEventListener {

    private final Consumer<String> lineHandler;

    private LineReader(Consumer<String> lineHandler) {
      this.lineHandler = lineHandler;
    }

    @Override
    public void serialEvent(SerialPortEvent event) {
      if (!event.isRXCHAR() || event.getEventValue() <= 0 || serialPort == null) {
        return;
      }

      try {
        String chunk = serialPort.readString(event.getEventValue());
        if (chunk != null && !chunk.isEmpty()) {
          appendIncoming(chunk, lineHandler);
        }
      } catch (SerialPortException ex) {
        lineHandler.accept("ERROR:" + ex.getExceptionType());
      }
    }
  }
}

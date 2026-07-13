import java.io.IOException;
import java.util.function.Consumer;

public interface SerialConnection extends AutoCloseable {

  void open(String portName, int baudRate, Consumer<String> lineHandler) throws IOException;

  void sendLine(String line) throws IOException;

  boolean isOpen();

  @Override
  void close() throws IOException;
}

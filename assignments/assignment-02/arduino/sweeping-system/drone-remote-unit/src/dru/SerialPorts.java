import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import jssc.SerialPortList;

public final class SerialPorts {

  private SerialPorts() {
  }

  public static List<String> listAvailablePorts() {
    String[] portNames = SerialPortList.getPortNames();
    if (portNames == null || portNames.length == 0) {
      return Collections.emptyList();
    }

    List<String> ports = new ArrayList<>(Arrays.asList(portNames));
    Collections.sort(ports);
    return ports;
  }
}

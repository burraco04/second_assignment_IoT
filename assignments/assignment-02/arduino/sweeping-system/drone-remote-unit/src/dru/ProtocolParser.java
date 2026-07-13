import java.util.Locale;

public final class ProtocolParser {

  private ProtocolParser() {
  }

  public static ProtocolEvent parse(String rawLine) {
    String line = rawLine.trim();
    if (line.isEmpty()) {
      return new ProtocolEvent(ProtocolEvent.Type.UNKNOWN, rawLine);
    }

    if (line.startsWith("ERROR:")) {
      return new ProtocolEvent(ProtocolEvent.Type.ERROR, line.substring("ERROR:".length()));
    }

    if (line.startsWith("lo:")) {
      String logLine = line.substring(3);
      String upperLog = logLine.toUpperCase(Locale.ROOT);
      if (upperLog.contains("[HANGAR] ALARM")) {
        return new ProtocolEvent(ProtocolEvent.Type.HANGAR_STATE, "ALARM");
      }
      if (upperLog.contains("[HANGAR] PRE-ALARM")) {
        return new ProtocolEvent(ProtocolEvent.Type.HANGAR_STATE, "PRE_ALARM");
      }
      if (upperLog.contains("[HANGAR] NORMAL")) {
        return new ProtocolEvent(ProtocolEvent.Type.HANGAR_STATE, "NORMAL");
      }
      return new ProtocolEvent(ProtocolEvent.Type.LOG, logLine);
    }

    if (line.startsWith("STATE:")) {
      return new ProtocolEvent(ProtocolEvent.Type.DRONE_STATE, line.substring("STATE:".length()));
    }

    if (line.startsWith("HANGAR:")) {
      return new ProtocolEvent(ProtocolEvent.Type.HANGAR_STATE, line.substring("HANGAR:".length()));
    }

    if (line.startsWith("DISTANCE:")) {
      return new ProtocolEvent(ProtocolEvent.Type.DISTANCE, line.substring("DISTANCE:".length()));
    }

    if ("ALARM".equalsIgnoreCase(line)) {
      return new ProtocolEvent(ProtocolEvent.Type.HANGAR_STATE, "ALARM");
    }

    return new ProtocolEvent(ProtocolEvent.Type.LOG, line);
  }
}

public final class ProtocolEvent {

  public enum Type {
    DRONE_STATE,
    HANGAR_STATE,
    DISTANCE,
    LOG,
    ERROR,
    UNKNOWN
  }

  private final Type type;
  private final String value;

  public ProtocolEvent(Type type, String value) {
    this.type = type;
    this.value = value;
  }

  public Type getType() {
    return type;
  }

  public String getValue() {
    return value;
  }
}

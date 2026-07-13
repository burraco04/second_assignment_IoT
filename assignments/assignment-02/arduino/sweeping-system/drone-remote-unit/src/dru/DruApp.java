import javax.swing.SwingUtilities;
import javax.swing.UIManager;

public final class DruApp {

  private DruApp() {
  }

  public static void main(String[] args) {
    SwingUtilities.invokeLater(() -> {
      try {
        UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
      } catch (Exception ignored) {
      }

      new DroneRemoteUnitFrame().setVisible(true);
    });
  }
}

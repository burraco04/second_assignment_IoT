package it.unibo.iot.assignment03.dbs;

import it.unibo.iot.assignment03.dbs.config.DbsConfig;
import it.unibo.iot.assignment03.dbs.http.CusHttpClient;
import it.unibo.iot.assignment03.dbs.ui.DashboardFrame;
import javax.swing.SwingUtilities;

public final class DbsApplication {
    private DbsApplication() {
    }

    public static void main(final String[] args) throws Exception {
        final DbsConfig config = DbsConfig.load(args.length > 0 ? args[0] : "config.properties");
        final CusHttpClient client = new CusHttpClient(config);
        SwingUtilities.invokeLater(() -> new DashboardFrame(config, client).setVisible(true));
    }
}


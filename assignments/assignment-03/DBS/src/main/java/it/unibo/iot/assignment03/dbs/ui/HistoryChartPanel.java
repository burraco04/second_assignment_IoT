package it.unibo.iot.assignment03.dbs.ui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Deque;
import java.util.List;
import javax.swing.JPanel;

public final class HistoryChartPanel extends JPanel {
    private static final int LEFT = 38;
    private static final int RIGHT = 10;
    private static final int TOP = 10;
    private static final int BOTTOM = 24;

    private final int maxSamples;
    private final Deque<Integer> samples = new ArrayDeque<>();

    public HistoryChartPanel(final int maxSamples) {
        this.maxSamples = maxSamples;
        setPreferredSize(new Dimension(700, 280));
        setBackground(Color.WHITE);
    }

    public void addSample(final int value) {
        samples.addLast(Math.max(0, Math.min(100, value)));
        while (samples.size() > maxSamples) {
            samples.removeFirst();
        }
        repaint();
    }

    public int sampleCount() {
        return samples.size();
    }

    @Override
    protected void paintComponent(final Graphics graphics) {
        super.paintComponent(graphics);
        final Graphics2D g = (Graphics2D) graphics.create();

        final int width = getWidth();
        final int height = getHeight();
        final int plotWidth = width - LEFT - RIGHT;
        final int plotHeight = height - TOP - BOTTOM;

        g.setColor(Color.LIGHT_GRAY);
        for (int level = 0; level <= 100; level += 25) {
            final int y = TOP + plotHeight - Math.round(level / 100f * plotHeight);
            g.drawLine(LEFT, y, width - RIGHT, y);
            g.setColor(Color.DARK_GRAY);
            g.drawString(level + "%", 6, y + 4);
            g.setColor(Color.LIGHT_GRAY);
        }

        if (samples.size() < 2) {
            g.setColor(Color.DARK_GRAY);
            g.drawString("Waiting for samples", LEFT, height / 2);
            g.dispose();
            return;
        }

        final List<Integer> values = new ArrayList<>(samples);
        g.setColor(Color.BLUE);
        for (int i = 1; i < values.size(); i++) {
            g.drawLine(
                xFor(i - 1, values.size(), plotWidth),
                yFor(values.get(i - 1), plotHeight),
                xFor(i, values.size(), plotWidth),
                yFor(values.get(i), plotHeight)
            );
        }
        g.dispose();
    }

    private static int xFor(final int index, final int size, final int plotWidth) {
        return LEFT + Math.round(index / (float) Math.max(1, size - 1) * plotWidth);
    }

    private static int yFor(final int value, final int plotHeight) {
        return TOP + plotHeight - Math.round(value / 100f * plotHeight);
    }
}


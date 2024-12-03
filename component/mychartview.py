from PyQt5.QtChart import QChartView, QLineSeries, QChart, QScatterSeries
from PyQt5.QtGui import QPainter


class myChartView(QChartView):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setRenderHint(QPainter.Antialiasing)
        # 隐藏图例
        self.chart().legend().setVisible(False)
        self.chart().setAnimationOptions(QChart.SeriesAnimations)
        self.myseries_line = QLineSeries(self.chart())
        self.myseries_point = QScatterSeries(self.chart())
        self.chart().addSeries(self.myseries_line)
        self.chart().addSeries(self.myseries_point)
        self.chart().createDefaultAxes()
        self.x = []
        self.y = []

    def append(self, x, y):
        self.x.append(x)
        self.y.append(y)
        self.myseries_line.append(x, y)
        self.myseries_point.append(x, y)
        self.chart().axisX().setRange(0, max(self.x))
        if self.chart().title() == 'Loss':
            self.chart().axisY().setRange(0, max(self.y))
        self.update()

    def clearchart(self):
        self.myseries_point.clear()
        self.myseries_line.clear()

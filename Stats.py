from PyQt6.QtWidgets import QWidget, QLabel, QVBoxLayout, QTextEdit, QPushButton, QHBoxLayout
from PyQt6.QtCore import Qt

class StatsWindow(QWidget):
    def __init__(self, output_text):
        super().__init__()
        self.setWindowTitle("Stats Output")
        self.showFullScreen()

        self.setStyleSheet("""
            QWidget {
                background-color: black;
                color: gold;
                font-size: 18px;
                font-family: Arial;
            }
            QTextEdit {
                background-color: #111;
                border: 1px solid gold;
                padding: 10px;
            }
            QPushButton {
                background-color: black;
                color: gold;
                padding: 10px 20px;
                border: 2px solid gold;
                border-radius: 5px;
                font-weight: bold;
            }
            QPushButton:hover {
                background-color: #222;
            }
        """)

        layout = QVBoxLayout()
        layout.setContentsMargins(50, 50, 50, 50)
        layout.setSpacing(20)

        label = QLabel("Stats Output")
        label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(label)

        output_box = QTextEdit()
        output_box.setText(output_text)
        output_box.setReadOnly(True)
        output_box.setHtml(self.format_output(output_text))
        layout.addWidget(output_box)

        # Buttons
        button_layout = QHBoxLayout()
        retry_button = QPushButton("Retry Simulation")
        exit_button = QPushButton("Exit")

        retry_button.clicked.connect(self.retry_simulation)
        exit_button.clicked.connect(self.close)

        button_layout.addWidget(retry_button)
        button_layout.addWidget(exit_button)

        layout.addLayout(button_layout)
        self.setLayout(layout)

    def format_output(self, raw_output):
        lines = [line.strip() for line in raw_output.strip().split('\n') if line.strip()]

        values = []
        for line in lines:
            parts = [p.strip() for p in line.split('|') if p.strip()]
            values.append(parts[1:])
        
        processes = values[1]
        arrival = values[2]
        service = values[3]
        finish = values[4]
        turnaround = values[5]
        normturn = values[6]

        # Parse mean values
        mean_turnaround = turnaround[-1]
        mean_normturn = normturn[-1]

        # Build HTML table
        html = """
        <style>
            table {
                width: 100%;
                color: gold;
                font-size: 40px;
                margin-top: 200px;
                text-align: center;
            }
            th, td {
                border: 1px solid gold;
                padding: 12px 20px;
                text-align: center;
            }
        </style>
        <table>
            <tr>
                <th>Process</th><th>Arrival</th><th>Service</th><th>Finish</th><th>Turnaround</th><th>NormTurn</th>
            </tr>
        """

        for i in range(len(processes)):
            html += f"""
            <tr>
                <td>{processes[i]}</td>
                <td>{arrival[i]}</td>
                <td>{service[i]}</td>
                <td>{finish[i]}</td>
                <td>{turnaround[i]}</td>
                <td>{normturn[i]}</td>
            </tr>
            """

        html += "</table><br>"

        html += f"""
        <p style='font-size: 40px; margin-top: 20px; text-align: center;'>
            <b>Mean Turnaround:</b> {mean_turnaround}<br>
            <b>Mean NormTurn:</b> {mean_normturn}
        </p>
        """

        return html


    def retry_simulation(self):
        from Input import InputWindow
        self.input_window = InputWindow()
        self.input_window.show()
        self.close()

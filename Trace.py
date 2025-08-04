from PyQt6.QtWidgets import QWidget, QLabel, QVBoxLayout, QTextEdit, QPushButton, QHBoxLayout
from PyQt6.QtCore import Qt

class TraceWindow(QWidget):
    def __init__(self, output_text):
        super().__init__()
        self.setWindowTitle("Trace Output")
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

        label = QLabel("Trace Output")
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

        # Remove dashed lines
        lines = [line for line in lines if set(line) != {'-', '|'}]

        # First line: headers (mode + time units)
        headers = lines[0].split()
        mode = headers[0]
        time_headers = headers[1:]

        # Rest: process rows
        process_rows = []
        for line in lines[1:]:
            parts = line.replace('|', ' ').replace('.', '.').split()
            if not parts: continue
            label = parts[0]
            timeline = parts[1:]
            if label.isalpha():
                process_rows.append((label, timeline))

        # Create HTML table
        html = f"""
        <style>
            table {{
                width: 100%;
                color: gold;
                margin-top: 200px;
                font-size: 40px;
                text-align: center;
            }}
            th, td {{
                border: 1px solid gold;
                padding: 6px 10px;
                text-align: center;
            }}
            caption {{
                font-size: 40px;
                margin-bottom: 10px;
            }}
        </style>
        <table>
            <caption>{mode} Scheduling Trace</caption>
            <tr>
                <th>Time</th>
        """
        for t in time_headers:
            html += f"<th>{t}</th>"
        html += "</tr>"

        for label, timeline in process_rows:
            html += f"<tr><td>{label}</td>"
            for cell in timeline:
                html += f"<td>{cell}</td>"
            html += "</tr>"
        html += "</table>"
        return html



    def retry_simulation(self):
        from Input import InputWindow
        self.input_window = InputWindow()
        self.input_window.show()
        self.close()

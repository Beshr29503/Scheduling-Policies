import subprocess
from PyQt6.QtWidgets import (
    QWidget, QLabel, QPushButton, QVBoxLayout, QHBoxLayout,
    QComboBox, QSpinBox, QLineEdit, QTextEdit
)
from PyQt6.QtGui import QFont
from PyQt6.QtCore import Qt
from Trace import TraceWindow
from Stats import StatsWindow


class InputWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Scheduler Input")
        self.showFullScreen()

        # self.setStyleSheet("background-color: black; color: gold;")
        self.setStyleSheet("""
            QWidget {
                background-color: black;
                color: gold;
                font-size: 24px;
                font-family: Arial;
            }
            QLineEdit, QComboBox, QSpinBox, QTextEdit {
                background-color: #111;
                color: gold;
                padding: 30px;
                border: 1px solid gold;
                border-radius: 5px;
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
        
        self.mode_box = QComboBox()
        self.mode_box.addItems(["Trace", "Stats"])

        self.policy_box = QComboBox()
        self.policy_box.addItems([
            "1 — FCFS", "2 — RR", "3 — SPN", "4 — SRT",
            "5 — HRRN", "6 — FB-1", "7 — FB-2i", "8 — Aging"
        ])

        self.sim_end_spin = QSpinBox()
        self.sim_end_spin.setRange(1, 50)

        self.num_proc_spin = QSpinBox()
        self.num_proc_spin.setRange(1, 10)

        self.name_input = QLineEdit()
        self.name_input.setPlaceholderText("Process Name")

        self.start_spin = QSpinBox()
        self.start_spin.setRange(0, 50)

        self.duration_spin = QSpinBox()
        self.duration_spin.setRange(1, 100)

        self.enter_button = QPushButton("Enter Process")

        self.status_box = QTextEdit()
        self.status_box.setReadOnly(True)

        self.output_button = QPushButton("See Output")
        self.output_button.setVisible(False)
        self.output_button.clicked.connect(self.show_output_window)


        self.entered_processes = []
        self.total_required = 0

        self.enter_button.clicked.connect(self.add_process)

        self.build_layout()

    def build_layout(self):
        main_layout = QVBoxLayout()

        header = QLabel("Enter Scheduler Simulation Inputs")
        header.setFont(QFont("Arial", 20, QFont.Weight.Bold))
        header.setAlignment(Qt.AlignmentFlag.AlignCenter)
        main_layout.addWidget(header)

        # Vertical layout for form fields
        form_layout = QVBoxLayout()

        # Each input field in its own row
        row1 = QHBoxLayout()
        row1.addWidget(QLabel("Mode:"))
        row1.addWidget(self.mode_box)
        form_layout.addLayout(row1)

        row2 = QHBoxLayout()
        row2.addWidget(QLabel("Policy:"))
        row2.addWidget(self.policy_box)
        form_layout.addLayout(row2)

        row3 = QHBoxLayout()
        row3.addWidget(QLabel("Simulation End:"))
        row3.addWidget(self.sim_end_spin)
        form_layout.addLayout(row3)

        row4 = QHBoxLayout()
        row4.addWidget(QLabel("Number of Processes:"))
        row4.addWidget(self.num_proc_spin)
        form_layout.addLayout(row4)

        # Horizontal layout for process name, start, duration
        proc_row = QHBoxLayout()
        proc_row.addWidget(QLabel("Name:"))
        proc_row.addWidget(self.name_input)
        proc_row.addWidget(QLabel("Start Time:"))
        proc_row.addWidget(self.start_spin)
        proc_row.addWidget(QLabel("Service Time:"))
        proc_row.addWidget(self.duration_spin)
        proc_row.setSpacing(200)

        form_layout.addLayout(proc_row)

        # Add Enter button
        form_layout.addWidget(self.enter_button)

        # Add form layout to main
        main_layout.addLayout(form_layout)

        # Add the status/output box under the process entry
        main_layout.addWidget(QLabel("Entered Processes:"))
        main_layout.addWidget(self.status_box)

        main_layout.addWidget(self.output_button)

        self.setLayout(main_layout)

    def add_process(self):
        name = self.name_input.text().strip()
        start = self.start_spin.value()
        duration = self.duration_spin.value()

        if not name:
            self.status_box.append("⚠️ Process name is required.")
            return

        self.entered_processes.append(f"{name},{start},{duration}")
        self.status_box.append(f"{len(self.entered_processes)}. Process: {name} , Start Time: {start}, Service Time: {duration}")

        self.name_input.clear()
        self.start_spin.setValue(0)
        self.duration_spin.setValue(1)

        self.total_required = self.num_proc_spin.value()
        if len(self.entered_processes) == self.total_required:
            self.status_box.append("\n✅ All processes entered. Starting simulation...")
            self.run_scheduler()

    def run_scheduler(self):
        mode = self.mode_box.currentText().lower()
        policy = self.policy_box.currentText().split("—")[0].strip()
        sim_end = self.sim_end_spin.value()
        num_procs = self.num_proc_spin.value()

        full_input = f"{mode}\n{policy}\n{sim_end}\n{num_procs}\n" + "\n".join(self.entered_processes) + "\n"

        try:
            process = subprocess.Popen(
                ["./lab6"],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            stdout, stderr = process.communicate(input=full_input)

            self.sim_output = stdout


            if stderr:
                self.status_box.append(f"\n❌ Error:\n{stderr}")
            else:
                self.output_button.setVisible(True)
                self.output_button.clicked.connect(self.show_output_window)

        except Exception as e:
            self.status_box.append(f"Exception: {str(e)}")

    def show_output_window(self):
        mode = self.mode_box.currentText().lower()
        if mode == "trace":
            self.trace_window = TraceWindow(self.sim_output)
            self.trace_window.show()
        else:
            self.stats_window = StatsWindow(self.sim_output)
            self.stats_window.show()
        self.close()

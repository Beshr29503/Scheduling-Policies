import sys
from PyQt6.QtWidgets import QApplication, QWidget, QLabel, QPushButton, QVBoxLayout, QHBoxLayout
from PyQt6.QtCore import Qt
from PyQt6.QtGui import QFont
from Input import InputWindow

class WelcomeWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("CPU Scheduler Simulator")
        self.showFullScreen()

        # Set background color to black
        self.setStyleSheet("background-color: black;")

        # Create welcome label
        welcome_label = QLabel("Welcome to the CPU Scheduler Simulator", self)
        welcome_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        welcome_label.setStyleSheet("color: gold;")
        welcome_label.setFont(QFont("Arial", 24, QFont.Weight.Bold))

        # Create buttons
        self.run_button = QPushButton("Run Program")
        self.exit_button = QPushButton("Exit Program")

        # Style buttons
        button_style = """
            QPushButton {
                background-color: black;
                color: gold;
                padding: 100px;
                font-size: 24px;
                font-weight: bold;
                border: none;
            }
            QPushButton:hover {
                background-color: #222;
            }
        """
        self.run_button.setStyleSheet(button_style)
        self.exit_button.setStyleSheet(button_style)

        # Horizontal layout for buttons
        button_layout = QHBoxLayout()
        button_layout.setAlignment(Qt.AlignmentFlag.AlignCenter)
        button_layout.setSpacing(150)
        button_layout.addWidget(self.run_button)
        button_layout.addWidget(self.exit_button)

        # Vertical layout for everything
        main_layout = QVBoxLayout()
        main_layout.setAlignment(Qt.AlignmentFlag.AlignCenter)
        main_layout.setSpacing(150)
        main_layout.addWidget(welcome_label)
        main_layout.addLayout(button_layout)

        self.setLayout(main_layout)

        # Connect button signals
        self.run_button.clicked.connect(self.open_input_window)
        self.exit_button.clicked.connect(self.close)

    def open_input_window(self):
        self.input_window = InputWindow()
        self.input_window.show()
        self.close()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = WelcomeWindow()
    window.show()
    sys.exit(app.exec())

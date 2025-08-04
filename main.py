import sys
from PyQt6.QtWidgets import QApplication
from Welcome import WelcomeWindow

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = WelcomeWindow()
    window.show()
    sys.exit(app.exec())

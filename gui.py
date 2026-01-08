import sys
import subprocess
import threading
from PySide6.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QHBoxLayout,
    QPushButton, QLineEdit, QTextEdit, QLabel, QGroupBox
)
from PySide6.QtCore import QObject, Signal
from PySide6.QtGui import QFont


# ================= Reader Thread =================
class Reader(QObject):
    output = Signal(str)
    exited = Signal()

    def __init__(self, proc):
        super().__init__()
        self.proc = proc

    def run(self):
        for line in self.proc.stdout:
            self.output.emit(line.rstrip())
        self.exited.emit()


# ================= Main GUI =================
class MemoryGUI(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Memory Management Simulator")
        self.resize(1100, 750)

        self.proc = subprocess.Popen(
            ["./memsim"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1
        )

        self.alive = True
        self.mode = "memory"   # memory | buddy

        self.init_ui()
        self.start_reader()

    # ================= UI =================
    def init_ui(self):
        main = QVBoxLayout(self)

        # ---------- Title ----------
        title = QLabel("Memory Management Simulator")
        title.setStyleSheet("font-size:20px; font-weight:bold;")
        main.addWidget(title)

        self.mode_label = QLabel("Mode: Memory Allocator")
        self.mode_label.setStyleSheet("color:#8be9fd; font-weight:bold;")
        main.addWidget(self.mode_label)

        # ---------- Console ----------
        self.console = QTextEdit()
        self.console.setReadOnly(True)
        self.console.setFont(QFont("Courier New", 10))
        self.console.setStyleSheet("""
            QTextEdit {
                background-color: #0f111a;
                color: #dcdcdc;
                border: 1px solid #444;
                padding: 6px;
            }
        """)
        main.addWidget(self.console, stretch=1)

        # ---------- Allocation Strategy ----------
        alloc_box = QGroupBox("Allocation Strategy (Memory mode only)")
        alloc_layout = QHBoxLayout()

        for name in ["first_fit", "best_fit", "worst_fit"]:
            btn = QPushButton(name.replace("_", " ").title())
            btn.clicked.connect(lambda _, n=name: self.set_allocator(n))
            alloc_layout.addWidget(btn)

        alloc_box.setLayout(alloc_layout)
        main.addWidget(alloc_box)

        # ---------- Commands ----------
        cmd_box = QGroupBox("Commands")
        cmd_layout = QVBoxLayout()

        # Init
        self.init_input = QLineEdit()
        self.init_input.setPlaceholderText("Memory size (bytes)")
        init_row = QHBoxLayout()
        init_row.addWidget(self.init_input)
        init_row.addWidget(self.button("Init Memory", self.init_memory))
        init_row.addWidget(self.button("Init Buddy", self.init_buddy))
        cmd_layout.addLayout(init_row)

        # Malloc
        self.malloc_input = QLineEdit()
        self.malloc_input.setPlaceholderText("Malloc size (bytes)")
        cmd_layout.addLayout(self.row(
            self.malloc_input,
            self.button("Malloc", self.malloc)
        ))

        # Free
        self.free_input = QLineEdit()
        self.free_input.setPlaceholderText("Free block ID")
        cmd_layout.addLayout(self.row(
            self.free_input,
            self.button("Free", self.free)
        ))

        # Stats / Dump / Cache
        misc = QHBoxLayout()
        misc.addWidget(self.button("Dump Memory", self.dump))
        misc.addWidget(self.button("Memory / Buddy Stats", self.stats))
        misc.addWidget(self.button("Cache Stats (L1 + L2)", self.cache))
        misc.addWidget(self.button("Exit", self.exit))
        cmd_layout.addLayout(misc)

        cmd_box.setLayout(cmd_layout)
        main.addWidget(cmd_box)

    # ================= Helpers =================
    def row(self, left, right):
        h = QHBoxLayout()
        h.addWidget(left)
        h.addWidget(right)
        return h

    def button(self, text, fn):
        b = QPushButton(text)
        b.clicked.connect(fn)
        return b

    # ================= Process IO =================
    def start_reader(self):
        self.reader = Reader(self.proc)
        self.reader.output.connect(self.handle_output)
        self.reader.exited.connect(self.on_exit)
        threading.Thread(target=self.reader.run, daemon=True).start()

    def on_exit(self):
        self.alive = False
        self.console.append("[Simulator exited]")

    def styled(self, text, color="#dcdcdc"):
        self.console.append(f"<span style='color:{color};'>{text}</span>")

    # ================= Output =================
    def handle_output(self, line):
        self.styled(line)

    # ================= Commands =================
    def send(self, cmd):
        if not self.alive:
            return
        self.styled(f"> {cmd}", "#6272a4")
        try:
            self.proc.stdin.write(cmd + "\n")
            self.proc.stdin.flush()
        except:
            self.on_exit()

    # ---------- Mode ----------
    def set_allocator(self, name):
        if self.mode != "memory":
            self.styled("Allocator ignored in Buddy mode", "#ffb86c")
            return
        self.send(f"set allocator {name}")

    def init_memory(self):
        s = self.init_input.text().strip()
        if s:
            self.mode = "memory"
            self.mode_label.setText("Mode: Memory Allocator")
            self.send(f"init memory {s}")
            self.init_input.clear()

    def init_buddy(self):
        s = self.init_input.text().strip()
        if s:
            self.mode = "buddy"
            self.mode_label.setText("Mode: Buddy Allocator")
            self.send(f"init buddy {s}")
            self.init_input.clear()

    # ---------- Actions ----------
    def malloc(self):
        s = self.malloc_input.text().strip()
        if s:
            self.send(f"malloc {s}")
            self.malloc_input.clear()

    def free(self):
        s = self.free_input.text().strip()
        if s:
            self.send(f"free {s}")
            self.free_input.clear()

    def dump(self):
        self.send("dump")

    def stats(self):
        self.send("stats")

    def cache(self):
        self.send("cache")

    def exit(self):
        self.send("exit")
        self.close()


# ================= Run =================
if __name__ == "__main__":
    app = QApplication(sys.argv)
    gui = MemoryGUI()
    gui.show()
    sys.exit(app.exec())

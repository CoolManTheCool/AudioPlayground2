import mido
import time
import threading
import tkinter as tk
import math

STEP_DURATION = 0.001
NUM_STEPS = 12
NUM_OCTAVES = 1

BASE_NOTE = 53  # F3

# MIDI → note name
NOTE_NAMES = ["C", "C#", "D", "D#", "E", "F",
              "F#", "G", "G#", "A", "A#", "B"]

# keyboard → semitone offset (F-based layout)
KEY_MAP = {
    "a": 0,    # F
    "w": 1,    # F#
    "s": 2,    # G
    "e": 3,    # G#
    "d": 4,    # A
    "r": 5,    # A#
    "f": 6,    # B
    "g": 7,    # C
    "y": 8,    # C#
    "h": 9,    # D
    "u": 10,   # D#
    "j": 11,   # E
    "k": 12    # F (next octave)
}


def midi_to_name(midi_note):
    name = NOTE_NAMES[midi_note % 12]
    octave = (midi_note // 12) - 1
    return f"{name}{octave}"


class ShepardToneMIDI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Shepard Tone Controller (MIDI)")
        self.geometry("420x260")

        self.playing = False
        self.thread = None

        # Create virtual MIDI output
        self.midi_out = mido.open_output(
            "ShepardTone MIDI", virtual=True
        )

        self.octave_offset = 0

        # Polyphony tracking
        self.held_keys = set()
        self.active_notes = {}

        # Disable key auto-repeat (Wayland-safe)
        try:
            self.tk.call("tk", "cancelrepeat")
        except tk.TclError:
            pass

        self.bind_all("<KeyPress>", self.on_key_down)
        self.bind_all("<KeyRelease>", self.on_key_up)

        # UI
        self.button = tk.Button(
            self,
            text="Play / Stop Shepard Tone",
            width=28,
            height=2,
            command=self.toggle_play
        )
        self.button.pack(pady=15)

        self.label = tk.Label(
            self,
            text="Pressed Notes: None",
            font=("Arial", 14)
        )
        self.label.pack()

    # ----------------- KEYS ----------------- #

    def on_key_down(self, event):
        keycode = event.keycode
        keysym = event.keysym.lower()

        if keycode in self.held_keys:
            return
        self.held_keys.add(keycode)

        # octave shift
        if keysym == "shift_l":
            self.octave_offset -= 12
            return
        if keysym == "shift_r":
            self.octave_offset += 12
            return

        if keysym not in KEY_MAP:
            return

        note = BASE_NOTE + KEY_MAP[keysym] + self.octave_offset
        self.active_notes[keycode] = note

        self.midi_out.send(
            mido.Message("note_on", note=note, velocity=50)
        )

        names = [midi_to_name(n) for n in self.active_notes.values()]
        self.label.config(text="Pressed Notes: " + ", ".join(names))

    def on_key_up(self, event):
        keycode = event.keycode
        keysym = event.keysym.lower()

        self.held_keys.discard(keycode)

        # undo octave shift
        if keysym == "shift_l":
            self.octave_offset += 12
            return
        if keysym == "shift_r":
            self.octave_offset -= 12
            return

        if keycode not in self.active_notes:
            return

        note = self.active_notes.pop(keycode)

        self.midi_out.send(
            mido.Message("note_off", note=note, velocity=0)
        )

        if self.active_notes:
            names = [midi_to_name(n) for n in self.active_notes.values()]
            self.label.config(text="Pressed Notes: " + ", ".join(names))
        else:
            self.label.config(text="Pressed Notes: None")

    # ---------------- SHEPARD LOOP ---------------- #

    def toggle_play(self):
        if self.playing:
            self.playing = False
            if self.thread:
                self.thread.join()
        else:
            self.playing = True
            self.thread = threading.Thread(
                target=self.play_loop, daemon=True
            )
            self.thread.start()

    def play_loop(self):
        idx = 0
        active = []

        while self.playing:
            # Turn off previous notes
            for note in active:
                self.midi_out.send(
                    mido.Message("note_off", note=note, velocity=0)
                )
            active.clear()

            for o in range(NUM_OCTAVES):
                note = BASE_NOTE + (idx % NUM_STEPS) + o * NUM_STEPS

                center = NUM_OCTAVES / 2
                strength = math.exp(
                    -0.5 * ((o - center) / (NUM_OCTAVES / 4)) ** 2
                )
                velocity = int(30 + strength * 70)

                self.midi_out.send(
                    mido.Message(
                        "note_on",
                        note=note,
                        velocity=velocity
                    )
                )
                active.append(note)

            time.sleep(STEP_DURATION)
            idx = (idx + 1) % NUM_STEPS

        # cleanup
        for note in active:
            self.midi_out.send(
                mido.Message("note_off", note=note, velocity=0)
            )


if __name__ == "__main__":
    app = ShepardToneMIDI()
    app.mainloop()

import tkinter as tk
import requests
import threading
import time

DEVICE_IP = "http://192.168.1.7"  # Change this to your NodeMCU IP

def threaded(fn):
    def wrapper(*args, **kwargs):
        threading.Thread(target=fn, args=args, kwargs=kwargs, daemon=True).start()
    return wrapper


@threaded
def toggle_led(index, state):
    try:
        url = f"{DEVICE_IP}/led"
        params = {"index": index, "state": state}
        r = requests.get(url, params=params, timeout=2)
        print(r.text)
    except Exception as e:
        print(f"[LED {index}] Error: {e}")


@threaded
def set_pwm(index, value):
    try:
        url = f"{DEVICE_IP}/pwm"
        payload = {"index": index, "value": int(value)}
        r = requests.post(url, json=payload, timeout=2)
        print(r.text)
    except Exception as e:
        print(f"[PWM {index}] Error: {e}")


@threaded
def update_status(labels, sliders):
    while True:
        try:
            r = requests.get(f"{DEVICE_IP}/status", timeout=2)
            data = r.json()
            for i in range(3):
                state = "ON" if data["states"][i] else "OFF"
                val = data["brightness"][i]
                labels[i].config(text=f"State: {state} | Brightness: {val}")
                sliders[i].set(val)
        except Exception as e:
            print("Status update failed:", e)
        time.sleep(2)


def create_led_control(root, index, label_list, slider_list):
    frame = tk.Frame(root, padx=10, pady=10, relief="groove", borderwidth=2)
    frame.pack(padx=5, pady=5, fill="x")

    tk.Label(frame, text=f"LED {index + 1}").pack()

    btn_on = tk.Button(frame, text="Turn ON", command=lambda: toggle_led(index, "on"))
    btn_on.pack(side="left", padx=5)

    btn_off = tk.Button(frame, text="Turn OFF", command=lambda: toggle_led(index, "off"))
    btn_off.pack(side="left", padx=5)

    status = tk.Label(frame, text="State: Unknown", fg="blue")
    status.pack()
    label_list.append(status)

    slider = tk.Scale(frame, from_=0, to=255, orient="horizontal",
                      label="Brightness", command=lambda val: set_pwm(index, val))
    slider.pack(fill="x")
    slider_list.append(slider)


def main():
    root = tk.Tk()
    root.title("ESP8266 3-LED Controller")

    labels = []
    sliders = []

    for i in range(3):
        create_led_control(root, i, labels, sliders)

    update_status(labels, sliders)

    root.mainloop()


if __name__ == "__main__":
    main()

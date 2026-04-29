import cv2
import torch
import serial
import time
import RPi.GPIO as GPIO

# ── Serial → Arduino Uno ──────────────────
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
time.sleep(2)  # Uno boot হওয়ার জন্য wait

# ── Ultrasonic (Pi GPIO) ──────────────────
TRIG, ECHO = 5, 6
GPIO.setmode(GPIO.BCM)
GPIO.setup(TRIG, GPIO.OUT)
GPIO.setup(ECHO, GPIO.IN)

# ── Serial Helper ─────────────────────────
def send(cmd):
    ser.write(f"{cmd}\n".encode())
    time.sleep(0.05)

# ── Motor Commands ────────────────────────
def move_forward():  send("FW")
def move_backward(): send("BK")
def turn_left():     send("LT")
def turn_right():    send("RT")
def stop_motors():   send("ST")
def pick_trash():    send("PK")

# ── Ultrasonic ────────────────────────────
def ultrasonic_distance():
    GPIO.output(TRIG, True)
    time.sleep(0.00001)
    GPIO.output(TRIG, False)
    start = stop = time.time()
    while GPIO.input(ECHO) == 0:
        start = time.time()
    while GPIO.input(ECHO) == 1:
        stop = time.time()
    return (stop - start) * 34300 / 2

# ── Lane Detection ────────────────────────
def lane_detection(frame):
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    _, thresh = cv2.threshold(gray, 60, 255, cv2.THRESH_BINARY_INV)
    M = cv2.moments(thresh)
    if M["m00"] != 0:
        cx = int(M["m10"] / M["m00"])
        return cx - frame.shape[1] // 2
    return 0

# ── YOLOv5 ───────────────────────────────
model = torch.hub.load('ultralytics/yolov5', 'custom',
                        path='best.pt', force_reload=True)

cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

pickup_count = 0
max_pickups  = 5

# ── Main Loop ─────────────────────────────
try:
    while True:
        ret, frame = cap.read()
        if not ret:
            break

        # ── Obstacle─────
        dist = ultrasonic_distance()
        if dist < 20:
            stop_motors()
            time.sleep(1)
            continue          

        # ── Lane keeping ──────────────────
        err = lane_detection(frame)
        if   err >  50: turn_right()
        elif err < -50: turn_left()
        else:           move_forward()

        # ── Object Detection ──────────────
        results    = model(frame)
        detections = results.xyxy[0]

        for *box, conf, cls in detections:
            label = model.names[int(cls)]
            if conf > 0.5 and label in ["dustbin", "cup"]:
                print(f"Detected: {label} ({conf:.2f})")
                pick_trash()       # Uno arm চালাবে
                time.sleep(5)      # arm complete হওয়ার জন্য wait
                pickup_count += 1
                move_forward()

        # ── Dumpyard ──────────────────────
        if pickup_count >= max_pickups:
            print("Max pickups — dumpyard navigation")
            stop_motors()
            pickup_count = 0

        # ── Display ───────────────────────
        cv2.imshow("Robot View", frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

except KeyboardInterrupt:
    pass

finally:                  # crash বা Ctrl+C — সব সময় cleanup
    cap.release()
    cv2.destroyAllWindows()
    stop_motors()
    ser.close()
    GPIO.cleanup()

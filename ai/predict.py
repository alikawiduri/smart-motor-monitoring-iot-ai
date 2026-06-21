import sys
import joblib
import pandas as pd

model = joblib.load(r"D:\TubesIoT\motor_health_model.pkl")

temp = float(sys.argv[1])
hum = float(sys.argv[2])
vib = float(sys.argv[3])
cur = float(sys.argv[4])

data = pd.DataFrame([{
    "temperature": temp,
    "humidity": hum,
    "vibration": vib,
    "current": cur
}])

prediction = model.predict(data)

print(prediction[0])
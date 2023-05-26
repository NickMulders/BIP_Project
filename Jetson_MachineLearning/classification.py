import numpy as np
import tensorflow as tf
from tensorflow.keras.preprocessing.image import img_to_array
from tensorflow.keras.models import load_model
from tensorflow.keras.applications.mobilenet_v2 import preprocess_input
from PIL import Image, ImageTk, ImageDraw
import cv2
import json
import paho.mqtt.client as mqtt

# Define MQTT parameters
broker_hostname = "test.mosquitto.org"
broker_port = 1883
topic = "BIP/TechConnect"
client = mqtt.Client()

# Variable to count detected frames
counter = 0

# variable to handle publishing
previous_prediction = None

# Load the labels and pre-trained model
model = load_model('model.h5')
labels_file = 'labels.txt'

# Read the labels from the file and store them in a list
with open(labels_file, 'r') as f:
    labels = [line.strip() for line in f.readlines()]

# Initialize the camera
cap = cv2.VideoCapture(0)

# Set the width and height of the camera
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

# Reconnect function, just in case
#def Reconnect(param1, param2, param3):
#    client.connect(broker_hostname, broker_port, keepalive=600)
#    print("RECONNECT")

# MQTT Connect
client.connect(broker_hostname, broker_port, keepalive=600)

#client.on_disconnect = Reconnect

# Load JSON files
with open('./symptoms_json/headache.json', 'r') as f:
    headache = json.load(f)

with open('./symptoms_json/heartache.json', 'r') as f:
    heartache = json.load(f)

with open('./symptoms_json/fracture.json', 'r') as f:
    fracture = json.load(f)

with open('./symptoms_json/fever.json', 'r') as f:
    fever = json.load(f)

with open('./symptoms_json/stomachache.json', 'r') as f:
    stomachache = json.load(f)

with open('./symptoms_json/hand.json', 'r') as f:
    hand = json.load(f)

# Continuously capture frames from the camera and classify them in real-time
while True:

    # Capture a frame from the camera
    ret, frame = cap.read()

    # Convert the frame to an Image object
    image = Image.fromarray(frame)

    # Preprocess the image
    image = image.resize((224, 224))
    image = img_to_array(image)
    image = preprocess_input(image)
    image = np.expand_dims(image, axis=0)

    # Make predictions on the image
    predictions = model.predict(image)
    predicted_label = labels[np.argmax(predictions[0])]

    # Handle detected frames, so we don't publish duplicates
    if predicted_label == previous_prediction:
        counter = counter + 1

    if predicted_label != previous_prediction:
        counter = 0

    # Comparing predictions and waiting 10 frames on each label to be sure
    if counter == 10 and predicted_label== "0 headache":

        # Publish to node-red
        client.publish(topic, json.dumps(headache))
        print("HEAD")

    if counter == 10 and predicted_label == "1 heartache":

        client.publish(topic, json.dumps(heartache))
        print("HEART")

    if counter == 10 and predicted_label == "2 fracture":

        client.publish(topic, json.dumps(fracture))
        print("FRACTURE")

    if counter == 10 and predicted_label == "3 fever":

        client.publish(topic, json.dumps(fever))
        print("FEVER")

    if counter == 10 and predicted_label == "4 stomachache":

        client.publish(topic, json.dumps(stomachache))
        print("STOMACHACHE")

    if counter == 10 and predicted_label == "5 hand":

        client.publish(topic, json.dumps(hand))
        print("HAND")
    
    # Variable to handle publishing
    previous_prediction = predicted_label

    # Display the predicted label on the frame
    image = Image.fromarray(frame)
    draw = ImageDraw.Draw(image)
    draw.text((30, 30), predicted_label, fill=(255, 0, 0))

    # Display the frame on the screen
    cv2.imshow('Real-time Image Classification', np.array(image))

    # Exit the program if the 'q' key is pressed
    if cv2.waitKey(1) == ord('q'):
        break

# Release the camera and close the window
client.disconnect()
cap.release()
cv2.destroyAllWindows()

# export LD_PRELOAD=/usr/lib/aarch64-linux-gnu/libgomp.so.1


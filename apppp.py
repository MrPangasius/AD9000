from flask import Flask, request, jsonify, render_template
import google.generativeai as genai
import io
from PIL import Image
import base64

app = Flask(__name__)

genai.configure(api_key="AIzaSyBJIbNXSKcCHaUlgRVdVdygj-95cUH3GK4")

# Variable to store the latest AI response
last_ai_response = "Awaiting first AI response"

# Route to serve the dashboard HTML
@app.route('/')
def dashboard():
    return render_template('index.html')  # Serves the HTML file located in the templates folder

# Endpoint for receiving images from Arduino
@app.route('/upload', methods=['POST'])
def upload_image():
    global last_ai_response  # Ensure we update the global variable

    if 'file' not in request.files:
        return jsonify({"error": "No file part"}), 400

    # Get the image file from the request
    file = request.files['file']
    image = Image.open(io.BytesIO(file.read()))

    # Process the image with the AI model
    ai_response = gemini_api_func(image)
    last_ai_response = ai_response  # Update the global variable
    return jsonify({"response": ai_response}), 200

# Sends the image to the Gemini API
def gemini_api_func(image):
    temp_file_path = "temp_image.jpg"
    image.save(temp_file_path)

    myfile = genai.upload_file(temp_file_path)
    model = genai.GenerativeModel("gemini-1.5-flash")
    result = model.generate_content(
        [myfile, "\n\n", "Succinctly give an audio description of the image as if you were guiding a blind person figure out what is directly in front of them. If you see anything dangerous/lethal first add the words 'DANGER DANGER DANGER' else don't add those words. Max word limit: 40 words. Here is an audio description of the image"]
    )
    print(result.text)
    return result.text

# Endpoint to provide the latest image and response
@app.route('/get_latest', methods=['GET'])
def get_latest():
    global last_ai_response

    try:
        # Read the last saved image
        with open("temp_image.jpg", "rb") as img_file:
            image_base64 = base64.b64encode(img_file.read()).decode('utf-8')
    except FileNotFoundError:
        return jsonify({"error": "No image available"}), 404

    return jsonify({"image": image_base64, "response": last_ai_response}), 200

if __name__ == "__main__":
    app.run(debug=True, host="192.168.216.31", port=5000)

from flask import Flask, request, jsonify
import google.generativeai as genai
import io
from PIL import Image

app = Flask(__name__)

genai.configure(api_key="AIzaSyBJIbNXSKcCHaUlgRVdVdygj-95cUH3GK4")
# Endpoint for receiving images from Arduino
@app.route('/upload', methods=['POST'])
def upload_image():
    if 'file' not in request.files:
        return jsonify({"error": "No file part"}), 400

    # Gets the image file from the request
    file = request.files['file']
    image = Image.open(io.BytesIO(file.read()))

    ai_response = gemini_api_func(image)
    return ai_response

# Sends the image to the Gemini API
def gemini_api_func(image):
    # Saves the image as a temporary file
    temp_file_path = "temp_image.jpg"
    image.save(temp_file_path)

    # Uploads the image file using the Google Generative AI API
    myfile = genai.upload_file(temp_file_path)

    # prompting brrr
    model = genai.GenerativeModel("gemini-1.5-flash")
    result = model.generate_content(
        [myfile, "\n\n", "Succintly give an audio description of the image as if you were guiding a blind person figure out what is directly in front of them. if you see anything dangerous/ lethal first add the words 'DANGER DANGER DANGER' else don't add those words. max word limit: 40 words"]
    )

    # Prints
    print(result.text)

    return result.text

if __name__ == "__main__":
    app.run(debug=True, host="192.168.1.6", port=5000)

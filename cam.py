import cv2
import requests
from PIL import Image
import io

SERVER_URL = "http://192.168.193.222:5000/upload" 

def send_image_to_server(image):
    # (BGR) to RGB
    image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    
    # Converts the image to a PIL Image 
    pil_image = Image.fromarray(image_rgb)
    
    # Converts PIL image to binary (JPEG) format for sending
    img_byte_arr = io.BytesIO()
    pil_image.save(img_byte_arr, format='JPEG')
    img_byte_arr = img_byte_arr.getvalue()

    try:
        # Sends the image to the server via POST request
        response = requests.post(SERVER_URL, files={'file': ('image.jpg', img_byte_arr, 'image/jpeg')})

        # Prints the server response
        if response.status_code == 200:
            print("Server response:", response.json())
        else:
            print(f"Failed to send image. Status code: {response.status_code}")

    except requests.exceptions.RequestException as e:
        print(f"Error sending image to server: {e}")

def capture_image():
    # Starts the webcam
    cap = cv2.VideoCapture(0)

    if not cap.isOpened():
        print("Error: Could not open webcam.")
        return None

    print("Press 's' to capture an image...")

    while True:
        # Read frame by frame
        ret, frame = cap.read()

        if not ret:
            print("Failed to grab frame")
            break

        # Displays the frame in a window
        cv2.imshow("Press 's' to capture an image", frame)

        # Waits for capture command 
        key = cv2.waitKey(1) & 0xFF
        if key == ord('s'):  # 's' key to capture the image
            print("Image captured!")
            break

    # Closes the webcam and close any OpenCV windows
    cap.release()
    cv2.destroyAllWindows()

    return frame  # Return the captured image

if __name__ == "__main__":
    captured_image = capture_image()
    if captured_image is not None:
        send_image_to_server(captured_image)

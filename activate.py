import time
import requests

# Replace this with an appropriate URL or endpoint from your Codespace
TARGET_URL = "https://www.google.com"  # Example: A server running in your Codespace

def keep_codespace_active():
    while True:
        try:
            # Sending a GET request
            response = requests.get(TARGET_URL)
            print(f"Request sent. Status code: {response.status_code}")
        except Exception as e:
            print(f"Error occurred: {e}")
        
        # Wait for 5 minutes (300 seconds)
        time.sleep(300)

if __name__ == "__main__":
    keep_codespace_active()

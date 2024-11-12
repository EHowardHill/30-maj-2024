import cv2
import os

def process_video(video_path):
    # Create the "screenshots" folder if it doesn't exist
    os.makedirs("screenshots", exist_ok=True)
    
    # Open the video file
    video_capture = cv2.VideoCapture(video_path)
    
    if not video_capture.isOpened():
        print("Error: Could not open video.")
        return
    
    frame_count = 0
    
    while True:
        # Read the next frame
        ret, frame = video_capture.read()
        
        # If no frame is returned, we've reached the end of the video
        if not ret:
            break
        
        # Get frame dimensions
        height, width = frame.shape[:2]
        
        # Resize the cropped frame to 240x160
        resized_frame = cv2.resize(frame, (240, 160), interpolation=cv2.INTER_LINEAR)
        
        # Save the frame as an image
        output_path = f"screenshots/frame_{frame_count:05d}.png"
        cv2.imwrite(output_path, resized_frame)
        
        frame_count += 1
    
    # Release the video capture object
    video_capture.release()
    print(f"Processed {frame_count} frames.")

# Replace 'input_video.mp4' with the path to your MP4 file
process_video("reference.mp4")

from pydub import AudioSegment
import os

# Load the audio file
input_file = "bgmloop.wav"
audio = AudioSegment.from_wav(input_file)

# Define the chunk duration in milliseconds (0.5 seconds)
chunk_duration = 1000  # 500 ms = 0.5 seconds
output_dir = "audio"

# Calculate the number of chunks
num_chunks = len(audio) // chunk_duration

# Create output directory if it doesn't exist
os.makedirs(output_dir, exist_ok=True)

# Split and export each chunk
for i in range(num_chunks):
    start = i * chunk_duration
    end = start + chunk_duration
    chunk = audio[start:end]
    output_file = os.path.join(output_dir, f"bgm{i}.wav")
    chunk.export(output_file, format="wav")
    print(f"Exported {output_file}")

print("Splitting complete.")

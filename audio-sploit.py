from pydub import AudioSegment
import os

# Load the audio file
input_file = "bgmloop.wav"
audio = AudioSegment.from_wav(input_file)

# Define the chunk duration in milliseconds (1 second)
chunk_duration = 1000
output_dir = "audio"
fps = 60  # Frames per second

# Calculate the number of chunks
num_chunks = len(audio) // chunk_duration

# Create output directory if it doesn't exist
os.makedirs(output_dir, exist_ok=True)

# Split and export each chunk
for i in range(num_chunks):
    start = i * chunk_duration
    end = start + chunk_duration
    chunk = audio[start:end]
    frames = int(len(chunk) * fps / 1000)  # Calculate the number of frames
    print(f"Chunk {i}: {len(chunk)} ms ({frames} frames)")
    output_file = os.path.join(output_dir, f"bgm{i}.wav")
    chunk.export(output_file, format="wav")
    print(f"Exported {output_file}")

# Handle the remaining audio, if any
if len(audio) % chunk_duration != 0:
    start = num_chunks * chunk_duration
    chunk = audio[start:]
    frames = int(len(chunk) * fps / 1000)  # Calculate the number of frames
    print(f"Remaining chunk: {len(chunk)} ms ({frames} frames)")
    output_file = os.path.join(output_dir, f"bgm{num_chunks}.wav")
    chunk.export(output_file, format="wav")
    print(f"Exported {output_file} (remaining audio)")

print("Splitting complete.")

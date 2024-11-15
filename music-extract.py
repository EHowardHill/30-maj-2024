import numpy as np
from scipy.io import wavfile

# Replace 'file1.wav' and 'file2.wav' with your actual file paths
file1_path = 'file1.wav'
file2_path = 'file2.wav'
output_path = 'common_music.wav'

# Read the two WAV files
sample_rate1, data1 = wavfile.read(file1_path)
sample_rate2, data2 = wavfile.read(file2_path)

# Ensure the sample rates are the same
if sample_rate1 != sample_rate2:
    raise ValueError('Sample rates of the two files are different.')

# Convert audio data to float32 for processing
data1 = data1.astype(np.float32)
data2 = data2.astype(np.float32)

# Trim both files to the length of the shorter one
min_length = min(len(data1), len(data2))
data1 = data1[:min_length]
data2 = data2[:min_length]

# Compute the absolute difference between the two audio signals
difference = np.abs(data1 - data2)

# Define a threshold to determine where the signals are similar
# You may need to adjust this value based on your specific audio files
threshold = np.mean(difference) + np.std(difference)

# Create a mask where the difference is below the threshold (common music)
mask = difference < threshold

# Optionally, you can smooth the mask to reduce abrupt changes
# Uncomment the following lines if needed
# from scipy.ndimage import gaussian_filter1d
# mask = gaussian_filter1d(mask.astype(float), sigma=5) > 0.5

# Compute the average of the two signals where they are similar
common_music = np.zeros_like(data1)
common_music[mask] = (data1[mask] + data2[mask]) / 2

# Normalize the output to prevent clipping
max_val = np.max(np.abs(common_music))
if max_val > 0:
    common_music = common_music / max_val

# Scale back to int16 range if original data was int16
if data1.dtype == np.int16:
    common_music = (common_music * np.iinfo(np.int16).max).astype(np.int16)
else:
    common_music = common_music.astype(data1.dtype)

# Save the common music as a new WAV file
wavfile.write(output_path, sample_rate1, common_music)

print(f'Common music saved to {output_path}')

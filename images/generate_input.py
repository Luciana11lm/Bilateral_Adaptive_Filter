# Generate a simple grayscale PGM test image

width = 256
height = 256

with open("images/input.pgm", "wb") as f:
  f.write(b"P5\n")
  f.write(f"{width} {height}\n".encode())
  f.write(b"255\n")

  for y in range(height):
    for x in range(width):
      value = (x + y) // 2
      f.write(bytes([value]))
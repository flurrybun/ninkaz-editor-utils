import colorsys
import os.path

initial_color = "8BC81A"
hue_shift = -15

def hex_to_hsv(hex: str) -> tuple:
    r = int(hex[0:2], 16) / 255
    g = int(hex[2:4], 16) / 255
    b = int(hex[4:6], 16) / 255
    return colorsys.rgb_to_hsv(r, g, b)

def hsv_to_hex(hsv: tuple) -> str:
    r, g, b = colorsys.hsv_to_rgb(*hsv)
    r = int(r * 255)
    g = int(g * 255)
    b = int(b * 255)
    return f"{r:02X}{g:02X}{b:02X}"

def shift_hue(hex: str) -> str:
    hsv = hex_to_hsv(hex)
    hue = hsv[0] + hue_shift / 360

    if hue < 0:
        hue += 1
    if hue > 1:
        hue -= 1

    return hsv_to_hex((hue, hsv[1], hsv[2]))

def get_paragraph_colors(hex1: str, hex2: str, steps: int) -> list:
    hsv1 = hex_to_hsv(hex1)
    hsv2 = hex_to_hsv(hex2)
    colors = []

    for i in range(steps):
        ratio = (i + 1) / (steps + 1)

        hue_diff = hsv2[0] - hsv1[0]
        if abs(hue_diff) > 0.5:
            if hue_diff > 0: hue_diff -= 1
            else: hue_diff += 1

        hue = hsv1[0] + hue_diff * ratio
        hue %= 1

        sat = 0.3
        val = 1
        colors.append(hsv_to_hex((hue, sat, val)))

    return colors

def convert_markdown(filename: str):
    header_color = initial_color

    old_file = open(f'{os.path.dirname(__file__)}\\{filename}', 'r')
    new_file = open(f'{os.path.dirname(__file__)}\\..\\{filename}', 'w')

    lines = old_file.readlines()

    paragraphs = []
    headers = []
    paragraph_index = 0

    for line in lines:
        if paragraph_index >= len(paragraphs):
            paragraphs.append('')
        
        if line.startswith('### '):
            headers.append(line)
    
            if paragraphs[paragraph_index] != '':
                paragraph_index += 1
        else:
            paragraphs[paragraph_index] += line

    if filename == 'about.md':
        new_file.write(paragraphs[0])
        paragraphs = paragraphs[1:]

    if len(paragraphs) != len(headers):
        raise Exception('Number of headers and paragraphs do not match')

    for i in range(len(headers)):
        header = headers[i]
        paragraph = paragraphs[i]

        header = f'### <c-{header_color}>' + header[4:].rstrip('\n') + '</c>\n'
        new_file.write(header)
        
        steps = paragraph.count('**') // 2

        if steps > 0:
            colors = get_paragraph_colors(header_color, shift_hue(header_color), steps)
            paragraph_sections = paragraph.split('**')

            for i in range(steps):
                paragraph_sections[i * 2 + 1] = f'<c-{colors[i]}>**' + paragraph_sections[i * 2 + 1] + '**</c>'

            new_file.write(''.join(paragraph_sections))
        else:
            new_file.write(paragraph)

        header_color = shift_hue(header_color)

    old_file.close()
    new_file.close()

def main():
    print('Converting about.md...')
    convert_markdown('about.md')
    print('Converting changelog.md...')
    convert_markdown('changelog.md')
    print('Done!')

if __name__ == "__main__":
    main()
# Oppstar Beamer Theme

A professional, clean Beamer presentation theme designed for corporate and academic presentations. This theme features a modern design with a dark blue color scheme and clean typography.

## Features

- **Professional Design**: Clean, modern layout suitable for business and academic presentations
- **Dark Blue Color Scheme**: Professional color palette with excellent contrast
- **Modular Structure**: Separate files for colors, fonts, inner theme, and outer theme
- **Customizable**: Easy to modify colors, fonts, and layout elements
- **Logo Support**: Built-in support for company/institution logos
- **Multiple Block Types**: Standard, alert, and example blocks with consistent styling
- **Custom Layout Elements**: Positioned design elements including blue square and image placeholders
- **Custom Footer**: Three-section footer with configurable text and page numbers

## Design Elements

The template includes the following positioned elements:

- **Upper Left Corner**: 3cm x 3cm blue square (no spacing from edges)
- **Upper Right Corner**: Placeholder for your logo/image (1cm spacing from edges, anchored from upper right)
- **Upper Middle**: Placeholder next to blue square (1cm spacing from blue square)
- **Bottom Right Corner**: Placeholder for footer logo (no spacing from edges)
- **Bottom Left Corner**: Placeholder for footer logo (no spacing from edges)
- **Custom Footer**:
  - Left: "INTERNAL USE Only"
  - Center: "Transforming Talent. Powering Malaysia's Silicon Future"
  - Right: Page numbers

All placeholders show colored rectangles with labels (UR, UM, BR, BL) until you replace them with actual images.

### Positioning Details

- **Upper Right Image**: Positioned 1cm from both top and right edges, anchored from its upper-right corner for easy size adjustment
- **Upper Middle Image**: Positioned 4cm from left edge (1cm spacing from the 3cm blue square), 0.5cm from top
- **Bottom Images**: Positioned exactly at the corners with no spacing
- **Blue Square**: Fixed 3cm x 3cm positioned at upper-left corner

## Installation

### Option 1: Local Installation
1. Copy all `.sty` files to your presentation directory
2. Use `\usetheme{oppstar}` in your LaTeX document

### Option 2: System Installation
1. Copy the `.sty` files to your local texmf tree:
   ```
   ~/texmf/tex/latex/beamer/themes/theme/
   ~/texmf/tex/latex/beamer/themes/color/
   ~/texmf/tex/latex/beamer/themes/font/
   ~/texmf/tex/latex/beamer/themes/inner/
   ~/texmf/tex/latex/beamer/themes/outer/
   ```
2. Run `texhash` to update the TeX database

## Usage

### Basic Usage

```latex
\documentclass[aspectratio=169]{beamer}
\usetheme{oppstar}

\title{Your Presentation Title}
\subtitle{Your Subtitle}
\author{Your Name}
\institute{Your Institution}
\date{\today}

\begin{document}

\oppstartitlepage

\begin{frame}{Outline}
  \tableofcontents
\end{frame}

% Your content here

\end{document}
```

### Adding Custom Images

The template includes placeholder elements that can be replaced with your own images:

```latex
% Add these after loading the theme to replace placeholders with actual images
\oppstarupperrightimage{path/to/upper-right-logo.png}
\oppstaruppermiddleimage{path/to/upper-middle-logo.png}
\oppstarbottomrightimage{path/to/bottom-right-logo.png}
\oppstarbottomleftimage{path/to/bottom-left-logo.png}
```

### Custom Commands

- `\oppstartitlepage` - Creates a clean title slide
- `\oppstarsectionpage` - Creates section divider slides
- `\oppstarlogo{filename}` - Adds a logo to slides (legacy support)
- `\oppstarupperrightimage{filename}` - Sets upper right corner image (1cm spacing from edges)
- `\oppstaruppermiddleimage{filename}` - Sets upper middle image (next to blue square)
- `\oppstarbottomrightimage{filename}` - Sets bottom right corner image (no spacing)
- `\oppstarbottomleftimage{filename}` - Sets bottom left corner image (no spacing)

## Theme Components

### Colors (`beamercolorthemeoppstar.sty`)
- **oppstarblue**: Main dark blue color (RGB: 25,55,109)
- **oppstarlightblue**: Accent blue (RGB: 65,105,225)
- **oppstarwhite**: Pure white backgrounds
- **oppstargray**: Secondary text color
- **oppstarlightgray**: Light backgrounds for blocks

### Fonts (`beamerfontthemeoppstar.sty`)
- Sans-serif fonts for modern appearance
- Helvetica-like font family
- Consistent sizing hierarchy
- Bold titles and headers

### Layout (`beamerinnerthemeoppstar.sty`)
- Clean title page layout
- Professional frame titles
- Triangle bullet points
- Rounded blocks with subtle styling

### Navigation (`beamerouterthemeoppstar.sty`)
- Minimal header with section navigation
- Clean footer with author, date, and page numbers
- No distracting navigation symbols

## Customization

### Changing Colors

Edit `beamercolorthemeoppstar.sty` and modify the color definitions:

```latex
\definecolor{oppstarblue}{RGB}{25,55,109}      % Change main color
\definecolor{oppstarlightblue}{RGB}{65,105,225} % Change accent color
```

### Modifying Fonts

Edit `beamerfontthemeoppstar.sty` to change font sizes or families:

```latex
\setbeamerfont{title}{size=\LARGE,series=\bfseries}
\setbeamerfont{frametitle}{size=\Large,series=\bfseries}
```

### Layout Adjustments

Modify `beamerinnerthemeoppstar.sty` and `beamerouterthemeoppstar.sty` to change:
- Title page layout
- Frame title positioning
- Footer content and layout
- Block styling

## Example Presentation

See `example-presentation.tex` for a complete example showing:
- Title slide
- Table of contents
- Section slides
- Various block types
- Lists and formatting
- Multi-column layouts

## Compilation

Compile your presentation using:

```bash
pdflatex your-presentation.tex
```

For presentations with references or complex navigation:

```bash
pdflatex your-presentation.tex
pdflatex your-presentation.tex
```

## Requirements

- LaTeX with Beamer class
- Standard packages: `tikz`, `xcolor`, `fontenc`, `lmodern`, `helvet`
- PDF output (pdflatex recommended)

## Troubleshooting

### Theme Not Found
- Ensure all `.sty` files are in the same directory as your `.tex` file
- Check file permissions and names

### Compilation Errors
- Verify all required packages are installed
- Check for typos in theme file names
- Ensure proper LaTeX syntax in custom modifications

### Font Issues
- Install required font packages (`lmodern`, `helvet`)
- Check font encoding settings

## License

This theme is provided as-is for educational and professional use. Feel free to modify and distribute according to your needs.

## Support

For issues or questions:
1. Check the example presentation for proper usage
2. Verify all theme files are properly installed
3. Consult Beamer documentation for advanced customization

## Current State (Latest Updates)

### Visual Design
- **Title Page**: All elements (title, author, institute, date) completely centered using full paper width with white backgrounds
- **Frame Titles**: Completely centered with transparent backgrounds and blue text
- **Headers**: Clean design with no blue header lines on any slides
- **Footer**: Transparent background with three-section layout:
  - Left: "INTERNAL USE Only"
  - Center: "Transforming Talent. Powering Malaysia's Silicon Future"
  - Right: Page numbers
- **Blocks**: Standard, alert, and example blocks with proper styling and centering

### Layout Elements
- **Blue Square**: 1cm × 1cm positioned in upper left corner (no spacing from edges)
- **Upper Right Image**: 1cm spacing from edges, anchored from upper-right corner for easy adjustment
- **Upper Middle Image**: Precisely attached to right side of blue square, scaled to 1cm height to match
- **Bottom Corner Images**: Positioned at exact corners with no spacing
- **Content Margins**: 2.5cm left margin to clear positioned elements, 1cm right margin

### Image System
- **Placeholder System**: Shows UR, UM, BR, BL labels when no custom images specified
- **Image Replacement**: Working correctly - when image paths are provided, they replace placeholders
- **Commands Available**:
  ```latex
  \oppstarupperrightimage{path/to/image.png}
  \oppstaruppermiddleimage{path/to/image.png}
  \oppstarbottomrightimage{path/to/image.png}
  \oppstarbottomleftimage{path/to/image.png}
  ```

### Color Scheme
- **Title Page Elements**: White backgrounds with blue title text, gray secondary text
- **Frame Titles**: Transparent backgrounds with blue text
- **Blocks**: Blue title backgrounds with light gray body backgrounds
- **Footer**: Transparent background with gray text
- **Decorative Elements**: Blue square and colored placeholders as specified

## Version History

- v1.2 (2025-01-02): Complete centering fixes, transparent backgrounds, custom footer, precise image positioning
- v1.1 (2025-01-02): Added positioned decorative elements and image replacement system
- v1.0 (2025-01-01): Initial release with complete theme package


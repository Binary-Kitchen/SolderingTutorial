# BlinkyTree Manual

This directory contains the documentation for the BlinkyTree Christmas tree LED soldering kit, using the Binary Kitchen soldering tutorial template system.

## Files Structure

```
manual/
├── BlinkyTree_en.tex      # English manual source (uses template)
├── BlinkyTree_de.tex      # German manual source (uses template)
├── BlinkyTree_en.pdf      # English manual (generated)
├── BlinkyTree_de.pdf      # German manual (generated)
├── Makefile              # Simple build system
├── README.md             # This file
├── configuration/        # CSV configuration files
│   ├── en_config.csv     # English project configuration
│   ├── en_parts.csv      # English parts list
│   ├── en_steps.csv      # English assembly steps
│   ├── de_config.csv     # German project configuration
│   ├── de_parts.csv      # German parts list
│   └── de_steps.csv      # German assembly steps
├── images/               # Assembly photos and diagrams
│   ├── README.md         # Image requirements guide
│   ├── thumbnail.jpg     # Main project image [NEEDED]
│   ├── step01_*.jpg      # Assembly step photos [NEEDED]
│   └── finished_*.jpg    # Completed project photos [NEEDED]
└── misc/                 # Labels and additional materials
    ├── BlinkyTree_label.pdf     # Kit label [generated]
    └── BlinkyTree_boxlabel.pdf  # Box label [generated]
```

## Binary Kitchen Template System

This manual uses the standardized Binary Kitchen soldering tutorial template system, which:

- **Automatically generates** the complete manual from CSV configuration files
- **Supports multiple languages** (English and German)
- **Provides consistent formatting** across all Binary Kitchen tutorials
- **Includes standard sections** like safety warnings, troubleshooting, etc.

### Configuration Files (CSV Format)

#### `*_config.csv`
Project metadata and settings:
- Title, difficulty level, build time
- Version information and licensing
- Safety warnings and general information

#### `*_parts.csv`  
Component list in tabular format:
- Quantity, component name, description
- Color codes and markings
- Used for automatic parts list generation

#### `*_steps.csv`
Step-by-step assembly instructions:
- Image filename, instruction text
- Detailed assembly procedures with tips
- Used for automatic instruction generation

## Building the Manual

### Prerequisites
- LuaLaTeX (part of modern TeX distributions)
- Binary Kitchen template files in `../LaTeX_config/`

### Build Commands
```bash
# Build both language versions
make

# Build individual versions
make BlinkyTree_en.pdf
make BlinkyTree_de.pdf

# Clean build artifacts
make clean

# Clean everything including PDFs
make distclean
```

## Content Status

### Completed ✅
- CSV configuration framework
- English and German configurations
- Basic LaTeX templates
- Parts list definitions
- Assembly step outlines

### In Progress 🔄
- Assembly instruction refinement
- Image requirements documentation

### Needed 📋
- **Assembly photos** - Step-by-step photography
- **Thumbnail image** - Main project photo for landing page
- **Finished photos** - Completed project documentation
- **Testing and validation** - Verify all instructions

## Template System Features

The Binary Kitchen template automatically provides:

### Standard Sections
- **Title page** with project info and difficulty
- **Safety warnings** and tool requirements
- **Parts list** with component identification
- **Assembly instructions** with step-by-step photos
- **Testing procedures** and troubleshooting
- **Common failures** reference section

### Multilingual Support
- Automatic language selection based on filename
- Shared configuration with language-specific text
- Consistent formatting across languages

### Professional Output
- Standard Binary Kitchen branding
- Consistent typography and layout
- Print-ready PDF generation
- Web-friendly formatting

## Creating Content

### Writing Steps
1. **Update CSV files** with your specific content
2. **Take assembly photos** following the image guidelines
3. **Build and review** the generated PDF
4. **Iterate** until content is complete and accurate

### CSV Format Guidelines
- Use **semicolons (;)** to separate instruction paragraphs
- Use **commas in braces {,}** for literal commas in text  
- Reference parts with **\usePart{component}** macro
- Include **safety warnings** and **tips** in instructions

## Dependencies

Requires the shared Binary Kitchen LaTeX configuration:
- `../LaTeX_config/solderingTut.tex` - Main template
- `../LaTeX_config/BK-solderingtut.sty` - Style definitions
- Various support files for formatting and translations

Ensure the parent repository structure is intact when building.
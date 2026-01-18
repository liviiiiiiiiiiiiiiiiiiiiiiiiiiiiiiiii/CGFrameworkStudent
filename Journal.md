DISCLAIMER: This journal is written by AN AI.

# Development Journal - Computer Graphics Framework
**Date:** January 18, 2026  
**Contributors:** Jorge Camacho & Livia Fernández  
**Session Duration:** ~3.5 hours

---

## Session Overview
This session focused on implementing a complete UI toolbar system with interactive buttons for drawing tools and color selection in our Computer Graphics framework.

---

## Starting Point: Livia's Contribution

### What Livia Implemented (Commit: `ee52c9f`)
Livia created the foundation for our button system:

1. **Created `Button` Class** (`src/framework/button.h` & `button.cpp`)
   - Properties: `Image* image`, `Vector2 position`, `ButtonType type`
   - Enum `ButtonType` with values: Line, Rectangle, Triangle, Colors, Pencil, Eraser, ClearImage, LoadImage, SaveImage
   - Implemented `IsMouseInside()` method for click detection
   - Declared `Draw()` method (but left implementation empty)

2. **Minor Code Formatting**
   - Cleaned up spacing in `application.cpp` and `image.cpp`

---

## Our Development Session

### Phase 1: Understanding the Button System
**Goal:** Learn how buttons work and implement the first functional button

**Steps:**
1. Reviewed Livia's `Button` class structure
2. Understood the pattern: Image + Position + Type + Click Detection
3. Learned the workflow:
   - Declare button in `application.h`
   - Initialize button in `Init()` (load image, set position)
   - Draw button in `InitUI()`
   - Detect clicks in `OnMouseButtonDown()`

---

### Phase 2: Implementing Button Draw Function
**Commit:** `95ae8ff` - "Implemented draw function for button"

**What we did:**
- Completed the missing `Button::Draw()` method in `button.cpp`
- Implementation: `framebuffer.DrawImage(*image, position.x, position.y);`

---

### Phase 3: Creating the UI Toolbar
**Commit:** `431dc62` - "Added all icons to a low gray bar in the UI"

**What we did:**
1. Created `InitUI()` function to manage all UI elements
2. Loaded and displayed 9 tool icons horizontally:
   - Load, Save, Clear (file operations)
   - Pencil, Eraser (drawing tools)
   - Line, Rectangle, Triangle (shape tools)
3. Loaded and displayed 7 color icons:
   - White, Black, Red, Green, Blue, Yellow, Pink (Purple)
4. Positioned icons with proper spacing (10px between tools, 35px between colors)

**Technical Details:**
- Used `static Image` variables to load images only once
- Positioned toolbar at top of screen (y=5)
- Tools start at x=5, colors start at x=120

---

### Phase 4: Line and Rectangle Drawing Logic
**Commit:** `d56bf48` - "Created logic for buttons of drawing lines and drawing rectangles"

**What we implemented:**

#### 4.1 State Management
Added to `application.h`:
- `ButtonType ActiveTool` - Currently selected tool
- `bool isDrawing` - Whether user is in drawing mode
- `Vector2 drawStartPoint` - First click position
- `int borderWidth` - Adjustable border thickness (default: 2)

#### 4.2 Line Tool
- **First click:** Store starting point
- **Second click:** Draw line from start to current position using `DrawLineDDA()`
- Color: White (hardcoded initially)

#### 4.3 Rectangle Tool
- **First click:** Store first corner
- **Second click:** Calculate rectangle bounds and draw
- Used `std::min()` and `std::abs()` to handle any click order
- Parameters: position (x,y), size (w,h), border color, border width, filled, fill color

#### 4.4 Border Width Control
Added keyboard shortcuts:
- **`+` or `=`**: Increase border width (max: 20)
- **`-` or `_`**: Decrease border width (min: 1)
- Displays current width in console

**Technical Challenge:** 
- Initial attempt used `SDLK_PLUS` which doesn't exist in SDL2
- Solution: Used character literals `'+'`, `'='`, `'-'`, `'_'`

---

### Phase 5: Triangle Drawing Logic
**Commit:** `9eb1d21` - "Created triangle button + triangle logic"

**What we implemented:**

#### 5.1 Triangle-Specific State
Added to `application.h`:
- `int triangleClickCount` - Tracks number of clicks (0, 1, or 2)
- `Vector2 trianglePoint1` - First vertex
- `Vector2 trianglePoint2` - Second vertex

#### 5.2 Three-Click System
Unlike lines and rectangles (2 clicks), triangles need 3 points:
- **Click 1:** Store `trianglePoint1`, set counter to 1
- **Click 2:** Store `trianglePoint2`, set counter to 2
- **Click 3:** Use both stored points + current position, draw triangle, reset counter

#### 5.3 Triangle Button
- Loaded `triangle.png` image
- Positioned at x=75, y=5
- Added click detection to activate triangle tool

---

### Phase 6: Color System Implementation
**Not yet committed** - Implemented during session

**What we implemented:**

#### 6.1 Color Selection Infrastructure
Added 8 color buttons:
- White, Black, Red, Green, Blue, Yellow, Purple (pink icon), Cyan
- Each button positioned with 35px spacing
- All buttons use `ButtonType::Colors`

#### 6.2 Color State Management
Added to `application.h`:
- `Color currentColor` - Border/outline color (default: WHITE)
- `Color fillColor` - Fill color (default: BLACK)
- `bool isFilled` - Toggle for filled shapes (default: false)
- `enum ColorMode { BORDER, FILL }` - Which color we're setting
- `ColorMode colorMode` - Current mode (default: BORDER)

#### 6.3 Color Selection Logic
**Mode Toggle:**
- **`C` key:** Switch between BORDER and FILL mode
- Console displays: "Color Mode: BORDER" or "Color Mode: FILL"

**Color Button Behavior:**
- When in BORDER mode: Sets `currentColor`
- When in FILL mode: Sets `fillColor`
- Console shows which was set: "Border Color: RED" or "Fill Color: RED"

#### 6.4 Fill Toggle
- **`F` key:** Toggle `isFilled` on/off
- Console displays: "Fill: ON" or "Fill: OFF"

#### 6.5 Updated Drawing Functions
Modified all shape drawing to use dynamic colors:
- **Lines:** Use `currentColor`
- **Rectangles:** Border uses `currentColor`, fill uses `fillColor` and `isFilled`
- **Triangles:** Border uses `currentColor`, fill uses `fillColor` and `isFilled`

**Technical Challenge:**
- Initially tried using `SDL_SCANCODE_LSHIFT` for Shift key detection
- SDL scancodes weren't available in our setup
- Solution: Simplified to mode toggle with `C` key instead

---

## Complete Feature Set

### Drawing Tools
1. **Line Tool** - Two-click line drawing
2. **Rectangle Tool** - Two-click rectangle (any corner order)
3. **Triangle Tool** - Three-click triangle (any 3 points)

### Color System
- 8 selectable colors (White, Black, Red, Green, Blue, Yellow, Purple, Cyan)
- Separate border and fill color selection
- Fill toggle for shapes

### Keyboard Controls
| Key | Function |
|-----|----------|
| `C` | Toggle color mode (BORDER ↔ FILL) |
| `F` | Toggle fill on/off |
| `+` / `=` | Increase border width |
| `-` / `_` | Decrease border width |

### Mouse Controls
- **Left Click:** 
  - On tool button: Select tool
  - On color button: Set color (based on current mode)
  - On canvas: Draw with selected tool

---

## Code Statistics

### Files Modified
- `src/framework/application.h` - Added 20+ new member variables
- `src/framework/application.cpp` - Added ~200 lines of logic
- `src/framework/button.h` - Created by Livia (29 lines)
- `src/framework/button.cpp` - Completed implementation (29 lines)

### Total Changes Since Livia's Commit
```
4 files changed, 223 insertions(+), 91 deletions(-)
```

---

## Technical Learnings

### 1. Button Pattern
We learned a clean pattern for UI buttons:
```cpp
// Declaration
Button myButton;

// Initialization (in Init())
Image* img = new Image();
img->LoadPNG("path/to/image.png");
myButton = Button(img, x, y, ButtonType::MyType);

// Drawing (in InitUI())
myButton.Draw(framebuffer);

// Interaction (in OnMouseButtonDown())
if (myButton.IsMouseInside(mouse_position)) {
    // Handle click
}
```

### 2. State Management
Learned to manage complex drawing state:
- Tool selection (which tool is active)
- Drawing progress (mid-draw vs ready for new shape)
- Multi-point shapes (storing intermediate points)
- Color modes (which color we're setting)

### 3. SDL2 Keyboard Input
- Character literals (`'+'`, `'-'`) work for keyboard input
- SDL key constants may not always be available
- Case-insensitive input: handle both `'f'` and `'F'`

### 4. Shape Drawing Algorithms
- **Lines:** DDA algorithm with 2 points
- **Rectangles:** Need to normalize corner positions with `std::min()` and `std::abs()`
- **Triangles:** Scanline algorithm with 3 vertices

---

## Future Improvements

### Potential Enhancements
1. **Visual Feedback**
   - Highlight selected tool button
   - Show current border/fill colors in UI
   - Preview shapes while drawing (before final click)

2. **Additional Tools**
   - Pencil (freehand drawing)
   - Eraser
   - Circle tool
   - Fill bucket

3. **File Operations**
   - Implement Save button (export to PNG)
   - Implement Load button (import image)
   - Implement Clear button (reset canvas)

4. **Advanced Features**
   - Undo/Redo system
   - Layer support
   - Brush size for pencil
   - Color picker (custom colors)

---

## Conclusion

This was a highly productive session where we:
1. Built upon Livia's button foundation
2. Implemented a complete drawing tool system
3. Created a flexible color selection system
4. Learned important patterns for UI development

The codebase now has a solid foundation for a paint/drawing application with multiple tools, colors, and customization options.

**Next Steps:** Commit the color system changes and continue with additional tools and features.

# LAB 4: Introduction to the GPU — Documentación Completa

## Índice
1. [Conceptos Fundamentales de la GPU](#1-conceptos-fundamentales-de-la-gpu)
2. [Task 2.1: Creación del Quad](#2-task-21-creación-del-quad)
3. [Task 2.2: Patrones Procedurales (Fórmulas)](#3-task-22-patrones-procedurales-fórmulas)
4. [Task 2.3: Filtros de Imagen](#4-task-23-filtros-de-imagen)
5. [Task 2.4: Transformaciones de Imagen Animadas](#5-task-24-transformaciones-de-imagen-animadas)
6. [Task 2.5: Renderizado de Mallas 3D en GPU](#6-task-25-renderizado-de-mallas-3d-en-gpu)
7. [Interactividad (Controles)](#7-interactividad-controles)
8. [Archivos Modificados / Creados](#8-archivos-modificados--creados)
9. [Pipeline de Renderizado Completo](#9-pipeline-de-renderizado-completo)

---

## 1. Conceptos Fundamentales de la GPU

### ¿Qué es la GPU y por qué la usamos?

En los labs anteriores (Lab 2 y 3), toda la rasterización se hacía en la **CPU**: proyectábamos vértices manualmente, interpolábamos coordenadas baricéntricas, y pintábamos píxel a píxel en un framebuffer (`Image`). Esto es extremadamente lento porque la CPU procesa cada triángulo secuencialmente.

La **GPU** (Graphics Processing Unit) está diseñada para ejecutar miles de operaciones en paralelo. En vez de procesar cada vértice y cada píxel uno por uno, la GPU los procesa todos simultáneamente. Esto hace que el renderizado sea ordenes de magnitud más rápido.

### El Pipeline de Renderizado de OpenGL

El pipeline de renderizado en OpenGL sigue estos pasos:

```
CPU (Application)
    │
    ▼
┌──────────────────────────────────────────────────────────────────┐
│  1. VERTEX SHADER (.vs)                                          │
│     - Recibe cada vértice del mesh                               │
│     - Transforma coordenadas: Local → World → Clip Space         │
│     - Pasa datos interpolados al fragment shader (varying)       │
├──────────────────────────────────────────────────────────────────┤
│  2. RASTERIZACIÓN (automática por la GPU)                        │
│     - Determina qué píxeles cubre cada triángulo                 │
│     - Interpola las variables "varying" baricéntricamente        │
├──────────────────────────────────────────────────────────────────┤
│  3. FRAGMENT SHADER (.fs)                                        │
│     - Se ejecuta una vez POR CADA PÍXEL                          │
│     - Determina el color final de cada píxel                     │
│     - Puede usar texturas, iluminación, etc.                     │
├──────────────────────────────────────────────────────────────────┤
│  4. FRAMEBUFFER (pantalla)                                       │
│     - Depth test (GL_DEPTH_TEST) para oclusiones                 │
│     - El resultado se muestra en pantalla                        │
└──────────────────────────────────────────────────────────────────┘
```

### Conceptos Clave de GLSL (OpenGL Shading Language)

#### Tipos de variables en los shaders:

| Tipo | Descripción | Ejemplo |
|------|-------------|---------|
| **`uniform`** | Variable enviada desde la **CPU al shader**. Tiene el mismo valor para TODOS los vértices/píxeles del draw call. Se sube con `shader->SetUniform1()`, `shader->SetMatrix44()`, `shader->SetTexture()`. | `uniform mat4 u_model;` |
| **`varying`** | Variable que pasa datos del **vertex shader al fragment shader**. La GPU automáticamente interpola su valor por cada píxel según la posición baricéntrica dentro del triángulo. | `varying vec2 v_uv;` |
| **`attribute`** | Datos por vértice que vienen del mesh (posición, normal, UV). En GLSL antiguo se acceden con variables predefinidas. | `gl_Vertex`, `gl_Normal`, `gl_MultiTexCoord0` |

#### Variables predefinidas de GLSL (legacy/compatibility mode):

| Variable | Tipo | Descripción |
|----------|------|-------------|
| `gl_Vertex` | `vec4` | Posición del vértice en espacio local (viene del mesh) |
| `gl_Normal` | `vec3` | Normal del vértice en espacio local |
| `gl_MultiTexCoord0` | `vec4` | Coordenadas de textura UV del vértice |
| `gl_Position` | `vec4` | **Output del vertex shader**: posición del vértice en clip space |
| `gl_FragColor` | `vec4` | **Output del fragment shader**: color RGBA del píxel |

#### Tipos de datos GLSL:

| Tipo | Descripción |
|------|-------------|
| `float` | Número decimal |
| `vec2` | Vector de 2 floats (ej: coordenadas UV) |
| `vec3` | Vector de 3 floats (ej: posición XYZ, color RGB) |
| `vec4` | Vector de 4 floats (ej: posición homogénea XYZW, color RGBA) |
| `mat4` | Matriz 4×4 (ej: model matrix, viewprojection matrix) |
| `sampler2D` | Referencia a una textura 2D en la GPU |

#### Funciones GLSL importantes:

| Función | Descripción |
|---------|-------------|
| `texture2D(sampler, uv)` | Muestrea el color de una textura en las coordenadas UV dadas |
| `mix(a, b, t)` | Interpolación lineal: `a*(1-t) + b*t` |
| `step(edge, x)` | Devuelve 0 si `x < edge`, 1 si `x >= edge` |
| `smoothstep(a, b, x)` | Interpolación suave entre 0 y 1 |
| `floor(x)` | Redondea hacia abajo al entero más cercano |
| `fract(x)` | Parte decimal de x: `x - floor(x)` |
| `mod(x, y)` | Módulo: resto de la división `x/y` |
| `dot(a, b)` | Producto escalar de dos vectores |
| `length(v)` | Longitud (magnitud) de un vector |
| `normalize(v)` | Normaliza un vector (longitud 1) |
| `clamp(x, min, max)` | Restringe x al rango [min, max] |

### Clases C++ del Framework

#### `Shader` (shader.h / shader.cpp)
Encapsula todo el ciclo de vida de un programa de shaders OpenGL:
- **`Shader::Get("vertex.vs", "fragment.fs")`**: Carga, compila y enlaza los dos shaders. Los cachea para no recompilar.
- **`shader->Enable()`**: Activa el shader como programa actual de OpenGL, a partir de aquí los draw calls usan este shader.
- **`shader->Disable()`**: Desactiva el shader.
- **`shader->SetUniform1("nombre", valor)`**: Sube un `int` o `float` al shader.
- **`shader->SetMatrix44("nombre", matrix)`**: Sube una matriz 4x4 al shader.
- **`shader->SetTexture("nombre", textura)`**: Vincula una textura a un uniform `sampler2D`.

#### `Texture` (texture.h / texture.cpp)
Contenedor de imágenes **en la GPU** (a diferencia de `Image` que está en CPU):
- **`Texture::Get("ruta/imagen.tga")`**: Carga una imagen desde disco, la sube a la GPU como textura OpenGL, y la cachea.
- Soporta formatos `.tga` y `.png`.
- La textura tiene un `texture_id` que OpenGL usa internamente para identificarla.

#### `Mesh` (mesh.h / mesh.cpp)
Almacena la geometría (vértices, normales, UVs):
- **`mesh->CreateQuad()`**: Crea un quad (2 triángulos) que cubre el clip space [-1, 1].
- **`mesh->LoadOBJ("ruta.obj")`**: Carga una malla 3D desde un archivo OBJ.
- **`mesh->Render(GL_TRIANGLES)`**: Envía los vértices a la GPU para que sean procesados por el pipeline. Internamente usa `glVertexPointer`, `glNormalPointer`, `glTexCoordPointer` y `glDrawArrays`.

---

## 2. Task 2.1: Creación del Quad

### Objetivo
Crear un quad (rectángulo) que cubra toda la pantalla para poder ejecutar shaders 2D sobre él.

### Implementación

#### En `Application::Init()`:
```cpp
quad_mesh = new Mesh();
quad_mesh->CreateQuad(); // genera 2 triángulos en clip space [-1, 1]

quad_shader = Shader::Get("shaders/quad.vs", "shaders/quad.fs");
quad_texture = Texture::Get("images/fruits.png");
```

- `CreateQuad()` genera 4 vértices en las esquinas del clip space `(-1,-1)`, `(1,-1)`, `(1,1)`, `(-1,1)` formando 2 triángulos.
- Como las posiciones ya están en clip space, el vertex shader simplemente las pasa directamente.

#### Vertex Shader (`quad.vs`):
```glsl
varying vec2 v_uv;

void main()
{
    v_uv = gl_MultiTexCoord0.xy;  // Pasamos las UVs [0,1] al fragment shader
    gl_Position = gl_Vertex;       // Ya está en clip space, no necesitamos transformar
}
```

**¿Por qué `gl_Position = gl_Vertex` directamente?** Porque `CreateQuad()` genera los vértices directamente en clip space `[-1, 1]`. No necesitamos ninguna matriz de modelo ni proyección. Las coordenadas de textura (`gl_MultiTexCoord0`) van de `[0, 1]`, que es el rango estándar de las UVs.

#### En `Application::Render()`:
```cpp
glDisable(GL_DEPTH_TEST);    // No necesitamos depth test para 2D
quad_shader->Enable();        // Activar el shader

// Subir uniforms al shader
quad_shader->SetUniform1("u_mode", subtask_mode);
quad_shader->SetUniform1("u_show_texture", show_image_filters ? 1 : 0);
quad_shader->SetUniform1("u_aspect", window_width / (float)window_height);
quad_shader->SetUniform1("u_time", time);

if (quad_texture)
    quad_shader->SetTexture("u_texture", quad_texture);

quad_mesh->Render(GL_TRIANGLES);  // Dibujar el quad
quad_shader->Disable();            // Desactivar el shader
```

**Flujo de datos CPU → GPU:**
1. La CPU sube varios **uniforms**: `u_mode` (qué subtask mostrar), `u_show_texture` (si es patrón o filtro), `u_aspect` (aspect ratio), `u_time` (tiempo), y `u_texture` (la imagen).
2. Se llama a `mesh->Render()` que envía los vértices del quad a la GPU.
3. El vertex shader procesa cada vértice y pasa las UVs interpoladas al fragment shader.
4. El fragment shader se ejecuta por cada píxel del quad y calcula su color.

---

## 3. Task 2.2: Patrones Procedurales (Fórmulas)

### Concepto General
Los patrones procedurales se generan **matemáticamente** sin usar ninguna imagen. El fragment shader recibe las coordenadas UV interpoladas `v_uv` (rango `[0.0, 1.0]`) y calcula el color de cada píxel usando fórmulas matemáticas.

**Restricción importante:** NO se pueden usar condicionales (`if`) para generar los patrones. Los `if` solo se usan para cambiar entre subtasks.

### Variables comunes del fragment shader (`quad.fs`):
```glsl
vec2 uv = v_uv;               // Coordenadas UV [0, 1]
vec2 p = uv - vec2(0.5);      // Centradas en (0,0), rango [-0.5, 0.5]
p.x *= u_aspect;              // Corrección de aspect ratio para evitar deformación
vec3 color = vec3(0.0);        // Color de salida inicializado a negro
```

La corrección del aspect ratio (`p.x *= u_aspect`) es crucial: sin ella, los patrones circulares se verían ovalados en pantallas no cuadradas.

---

### Subtask a) — Gradiente horizontal bicolor (0.125p)

```glsl
// u_mode == 0
color = vec3(uv.x, 0.0, 1.0 - uv.x);
```

**Explicación detallada:**
- `uv.x` va de `0.0` (izquierda) a `1.0` (derecha).
- Canal **rojo** = `uv.x`: aumenta de 0 a 1 de izquierda a derecha.
- Canal **verde** = `0.0`: siempre apagado.
- Canal **azul** = `1.0 - uv.x`: disminuye de 1 a 0 de izquierda a derecha.
- **Resultado**: Gradiente de azul puro (izquierda) a rojo puro (derecha), pasando por magenta/violeta en el centro.

---

### Subtask b) — Gradiente radial (distancia al centro) (0.125p)

```glsl
// u_mode == 1
float d = length(p);
color = vec3(d);
```

**Explicación detallada:**
- `p` son las coordenadas centradas en `(0, 0)` con corrección de aspect ratio.
- `length(p)` calcula la **distancia euclidiana** al centro: `sqrt(p.x² + p.y²)`.
- Al usar `d` como los tres canales RGB, obtenemos un gradiente de grises.
- **Resultado**: Negro en el centro (d=0), blanco en los bordes (d grande). Forma un gradiente radial circular.

---

### Subtask c) — Barras rojas y azules (0.25p)

```glsl
// u_mode == 2
float red   = step(0.4, abs(fract(uv.x * 8.0) - 0.5));
float blue  = step(0.4, abs(fract(uv.y * 6.0) - 0.5));
color = vec3(red, 0.0, blue);
```

**Explicación detallada paso a paso:**

1. **`uv.x * 8.0`**: Escala la coordenada X para crear 8 repeticiones horizontales.
2. **`fract(...)`**: Extrae la parte decimal, creando un patrón repetitivo que va de 0 a 1 en cada celda.
3. **`fract(...) - 0.5`**: Centra el rango a [-0.5, 0.5].
4. **`abs(...)`**: Toma el valor absoluto, creando un patrón triangular simétrico (0 en los bordes de celda, 0.5 en el centro).
5. **`step(0.4, ...)`**: Devuelve 1 si el valor ≥ 0.4, 0 si es menor. Esto crea bandas discretas (encendido/apagado).

Lo mismo para las barras azules pero en dirección vertical (`uv.y * 6.0` = 6 repeticiones verticales).

- **Resultado**: Barras rojas verticales + barras azules horizontales. Donde se cruzan ambas, se ve magenta.

**¿Por qué no usa condicionales?** Porque `step()` es una función que devuelve 0 o 1 sin usar `if`. Es la forma funcional/matemática de crear una condición binaria.

---

### Subtask d) — Gradiente con celdas cuadradas (0.25p)

```glsl
// u_mode == 3
float cols = 10.0 * u_aspect;
float cellX = floor(uv.x * cols) / cols;
float cellY = floor(uv.y * 10.0) / 10.0;
color = vec3(cellX, cellY, 0.0);
```

**Explicación detallada:**

1. **`cols = 10.0 * u_aspect`**: Número de columnas ajustado al aspect ratio para que las celdas sean cuadradas.
2. **`floor(uv.x * cols)`**: Multiplica la UV por el número de columnas y redondea hacia abajo. Esto "discretiza" la posición X, agrupando todos los píxeles de una misma celda al mismo valor.
3. **`... / cols`**: Divide de vuelta para normalizar al rango [0, 1).
4. Lo mismo para Y con 10 filas.
5. `cellX` → canal rojo, `cellY` → canal verde.

- **Resultado**: Una cuadrícula de celdas donde cada celda tiene un color sólido. El rojo aumenta de izquierda a derecha, el verde de abajo a arriba, creando un gradiente bidimensional discretizado.

---

### Subtask e) — Tablero de ajedrez (checkerboard) (0.25p)

```glsl
// u_mode == 4
float cols = 10.0 * u_aspect;
float x = floor(uv.x * cols);
float y = floor(uv.y * 10.0);
float c = mod(x + y, 2.0);
color = vec3(c);
```

**Explicación detallada:**

1. Se calcula el índice de celda (`x`, `y`) igual que en la subtask d.
2. **`mod(x + y, 2.0)`**: Si la suma de los índices de fila y columna es par, el resultado es 0 (negro). Si es impar, el resultado es 1 (blanco).
3. Este es el clásico patrón de tablero de ajedrez.

- **Resultado**: Patrón de tablero blanco y negro con celdas cuadradas.

**¿Por qué `mod(x+y, 2)` crea un tablero?** Porque en un tablero, las casillas alternan: (0,0)=blanco, (1,0)=negro, (0,1)=negro, (1,1)=blanco... La paridad de la suma de coordenadas determina el color.

---

### Subtask f) — Onda sinusoidal con gradiente (0.5p)

```glsl
// u_mode == 5
float wave = 0.5 + 0.3 * sin(uv.x * 6.28);
float below = step(uv.y, wave);
float intensity = mix((1.0 - uv.y) * 2.0, uv.y * 2.0, below);
intensity = clamp(intensity, 0.0, 1.0);
color = vec3(0.0, intensity, 0.0);
```

**Explicación detallada:**

1. **`wave = 0.5 + 0.3 * sin(uv.x * 6.28)`**: Genera una onda sinusoidal centrada en 0.5 con amplitud 0.3. `6.28 ≈ 2π`, así que completa un ciclo completo a lo largo del ancho de la pantalla.
2. **`below = step(uv.y, wave)`**: Vale 1 si el píxel está por debajo de la onda (`uv.y ≤ wave`), 0 si está por encima.
3. **`mix(a, b, below)`**: Sin condicionales, selecciona entre:
   - Si está **por encima** de la onda (`below=0`): `(1.0 - uv.y) * 2.0` → gradiente que disminuye hacia arriba.
   - Si está **por debajo** de la onda (`below=1`): `uv.y * 2.0` → gradiente que aumenta hacia arriba.
4. **`clamp(..., 0.0, 1.0)`**: Asegura que la intensidad no se salga del rango válido.
5. Solo se usa el canal verde.

- **Resultado**: Una onda sinusoidal verde con gradientes de intensidad a ambos lados, creando un efecto de "colinas" luminosas.

---

## 4. Task 2.3: Filtros de Imagen

### Concepto General
En esta tarea aplicamos filtros de post-procesado sobre una textura (`fruits.png`). El fragment shader **muestrea** la textura con `texture2D()` y modifica los valores de color.

**Restricción:** NO se pueden usar condicionales para crear los filtros, solo para cambiar entre subtasks.

### Cómo funciona una textura en la GPU

1. **CPU** carga la imagen con `Texture::Get("images/fruits.png")` → la sube a la memoria de la GPU.
2. **CPU** vincula la textura al shader con `shader->SetTexture("u_texture", texture)`.
3. **GPU** en el fragment shader, declara `uniform sampler2D u_texture` para recibirla.
4. **GPU** muestrea la textura con `texture2D(u_texture, uv)` usando las coordenadas UV interpoladas.

**Diferencia entre `sampler2D` y `texture2D`:**
- `sampler2D` es un **tipo de variable** que representa una textura con su configuración de muestreo.
- `texture2D(sampler, uv)` es una **función** que lee el color de la textura en las coordenadas UV dadas.

### Código base:
```glsl
vec3 texColor = texture2D(u_texture, uv).rgb;
```
Esto lee el color RGB de la textura en la posición UV actual.

---

### Subtask a) — Escala de grises (Grayscale) (0.125p)

```glsl
float luminance = dot(texColor, vec3(0.299, 0.587, 0.114));
color = vec3(luminance);
```

**Explicación detallada:**
- La **luminancia perceptual** no es el promedio simple de RGB. El ojo humano es más sensible al verde que al rojo, y menos al azul.
- Los pesos `(0.299, 0.587, 0.114)` son del estándar **ITU-R BT.601** (NTSC), que pondera cada canal según la sensibilidad humana.
- **`dot(texColor, pesos)`** calcula: `R×0.299 + G×0.587 + B×0.114`.
- El resultado se asigna a los tres canales RGB para producir un gris uniforme.

---

### Subtask b) — Negativo (Inversión de color) (0.125p)

```glsl
color = 1.0 - texColor;
```

**Explicación detallada:**
- Cada canal se invierte: `R_nuevo = 1.0 - R_original`.
- Colores claros se vuelven oscuros y viceversa.
- Blanco (1,1,1) → Negro (0,0,0), Rojo (1,0,0) → Cian (0,1,1).
- Es una operación por componente gracias a la aritmética vectorial de GLSL.

---

### Subtask c) — Tonalidad amarilla (Sepia/Tint) (0.25p)

```glsl
float luminance = dot(texColor, vec3(0.299, 0.587, 0.114));
vec3 yellow = vec3(250.0, 237.0, 39.0) / 255.0;
color = luminance * yellow;
```

**Explicación detallada:**
1. Primero convertimos a escala de grises con la misma fórmula de luminancia.
2. Definimos un color amarillo en formato normalizado (dividimos los valores RGB 0-255 entre 255).
3. Multiplicamos la luminancia por el color amarillo: esto "tiñe" la imagen de ese color, manteniendo la intensidad original pero cambiando el tono.

- **Resultado**: Imagen en tonos amarillos, similar a un efecto sepia pero con tinte amarillo.

---

### Subtask d) — Umbralización (Binary/Threshold) (0.25p)

```glsl
float luminance = dot(texColor, vec3(0.299, 0.587, 0.114));
float threshold = 0.5;
float binary = step(threshold, luminance);
color = vec3(binary);
```

**Explicación detallada:**
1. Calculamos la luminancia del píxel.
2. Definimos un umbral de `0.5`.
3. **`step(0.5, luminance)`**: Si la luminancia ≥ 0.5, devuelve 1 (blanco). Si es menor, devuelve 0 (negro).
4. No se usa `if`, sino `step()` que es una función matemática.

- **Resultado**: Imagen binaria (solo blanco y negro), como una fotocopia de alto contraste.

---

### Subtask e) — Viñeta (Vignette) (0.25p)

```glsl
float dist = length(uv - vec2(0.5));
float vignette = smoothstep(0.8, 0.2, dist);
color = texColor * vignette;
```

**Explicación detallada:**
1. **`length(uv - vec2(0.5))`**: Distancia de cada píxel al centro de la imagen (0.5, 0.5).
2. **`smoothstep(0.8, 0.2, dist)`**: Función de interpolación suave.
   - Cuando `dist ≥ 0.8`: devuelve 0 (borde, completamente oscuro).
   - Cuando `dist ≤ 0.2`: devuelve 1 (centro, color original).
   - Entre 0.2 y 0.8: transición suave. 
   - **Nota**: Los parámetros están invertidos (`0.8, 0.2` en vez de `0.2, 0.8`), lo que invierte la rampa. Esto hace que sea 1 en el centro y 0 en los bordes.
3. Multiplicamos el color original por el factor de viñeta.

- **Resultado**: La imagen se oscurece gradualmente hacia los bordes, con el centro intacto. Efecto clásico de viñeta fotográfica.

---

### Subtask f) — Desenfoque (Blur) por promediado (Averaging filter) (0.5p)

```glsl
vec3 sum = vec3(0.0);
float offset = 1.0 / 256.0;
for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
        sum += texture2D(u_texture, uv + vec2(float(i), float(j)) * offset).rgb;
    }
}
color = sum / 9.0;
```

**Explicación detallada:**
1. Este es un **filtro de promediado 3×3** (box blur).
2. **`offset = 1.0/256.0`**: Tamaño de un píxel en coordenadas UV (asumiendo textura de 256px).
3. El doble bucle recorre los 9 píxeles vecinos (3×3) alrededor del píxel actual: el propio píxel y sus 8 vecinos.
4. Por cada vecino, muestreamos la textura en `uv + desplazamiento` y acumulamos.
5. Dividimos la suma entre 9 para obtener el promedio.

- **Resultado**: Imagen ligeramente desenfocada. Cada píxel es el promedio de sus vecinos, suavizando los bordes y detalles.

**Nota sobre convolución**: Este es el filtro de convolución más simple. Los filtros Gaussianos usan pesos diferentes para cada vecino (más peso al centro, menos a los bordes), produciendo un desenfoque más natural.

---

## 5. Task 2.4: Transformaciones de Imagen Animadas

### Concepto General
Estas transformaciones modifican las **coordenadas UV** antes de muestrear la textura, y usan la variable `u_time` (tiempo acumulado desde el inicio de la aplicación) para crear animaciones.

La variable `u_time` se sube desde la CPU:
```cpp
// En Application::Update():
this->time += seconds_elapsed;

// En Application::Render():
quad_shader->SetUniform1("u_time", time);
```

---

### Subtask g) — Pixelización Animada (0.5p)

```glsl
// u_mode == 6
float pixels = 20.0 + 100.0 * (0.5 + 0.5 * sin(u_time));
vec2 pixelated_uv = floor(uv * pixels) / pixels;
color = texture2D(u_texture, pixelated_uv).rgb;
```

**Explicación detallada:**

1. **`pixels`**: Número de "bloques" grandes. Oscila entre 20 y 120 gracias a `sin(u_time)`:
   - `sin(u_time)` oscila entre -1 y 1.
   - `0.5 + 0.5 * sin(u_time)` oscila entre 0 y 1.
   - `20.0 + 100.0 * (...)` oscila entre 20 y 120.
   - Cuando `pixels` es bajo (20), la imagen se ve muy pixelada (pocos bloques grandes).
   - Cuando `pixels` es alto (120), la imagen se ve casi normal (muchos bloques pequeños).

2. **`floor(uv * pixels) / pixels`**: "Discretiza" las coordenadas UV.
   - Multiplicar por `pixels` escala las UVs al número de bloques.
   - `floor()` redondea hacia abajo, agrupando múltiples píxeles en el mismo bloque.
   - Dividir por `pixels` devuelve las UVs al rango [0, 1] pero "aplanadas" por bloque.

3. Se muestrea la textura con las UV pixeladas, todos los píxeles de un bloque muestrean el mismo punto.

- **Resultado**: La imagen alterna suavemente entre alta y baja resolución, creando un efecto de pixelización pulsante.

---

### Subtask h) — Aberración Cromática Radial Animada (0.5p)

```glsl
// u_mode == 7
vec2 dir = uv - 0.5;
float dist = length(dir);
float strength = 0.05 * dist * (1.1 + sin(u_time * 3.0));

color.r = texture2D(u_texture, uv - dir * strength).r;
color.g = texture2D(u_texture, uv).g;
color.b = texture2D(u_texture, uv + dir * strength).b;
```

**Explicación detallada:**

1. **`dir = uv - 0.5`**: Dirección desde el centro de la imagen al píxel actual.
2. **`dist = length(dir)`**: Distancia al centro. El efecto es más fuerte en los bordes.
3. **`strength`**: Intensidad del efecto.
   - `0.05` es la base de la fuerza.
   - Se multiplica por `dist` (más fuerte lejos del centro).
   - `(1.1 + sin(u_time * 3.0))` oscila entre 0.1 y 2.1, haciendo que el efecto pulse rápidamente.

4. **Los tres canales se muestrean en posiciones diferentes:**
   - **Rojo** → `uv - dir * strength`: muestreado más cerca del centro.
   - **Verde** → `uv`: posición original (sin cambio).
   - **Azul** → `uv + dir * strength`: muestreado más lejos del centro.

- **Resultado**: Se separan los tres canales de color, creando halos de colores RGB alrededor de los bordes de los objetos, especialmente visibles en los bordes de la imagen. El efecto pulsa con el tiempo. Este es el mismo efecto que se ve en lentes de baja calidad cuando la luz se descompone en sus componentes.

---

## 6. Task 2.5: Renderizado de Mallas 3D en GPU

### Concepto General

En esta tarea pasamos de renderizar un quad 2D a renderizar **mallas 3D** (los modelos del lab anterior) **en la GPU**. La diferencia fundamental con el lab anterior:

| Aspecto | Lab 3 (CPU) | Lab 4/5 (GPU) |
|---------|-------------|---------------|
| **Proyección de vértices** | `camera->ProjectVector()` en C++ | Vertex shader con matrices |
| **Rasterización** | `DrawTriangleInterpolated()` en C++ | GPU automática |
| **Texturizado** | Interpolación baricéntrica manual | `texture2D()` en fragment shader |
| **Depth buffer** | `FloatImage zbuffer` manual | `GL_DEPTH_TEST` de OpenGL |
| **Velocidad** | Lento (secuencial) | Rápido (paralelo) |

### Paso 1: Crear los Shaders (`raster.vs` y `raster.fs`)

#### Vertex Shader — `raster.vs`

```glsl
//Global variables from the CPU
uniform mat4 u_model;
uniform mat4 u_viewprojection;

//Variables to pass to the fragment shader
varying vec2 v_uv;

void main()
{
    //Pass texture coordinates to the fragment shader
    v_uv = gl_MultiTexCoord0.xy;

    //Convert local position to world space
    vec3 world_position = (u_model * vec4( gl_Vertex.xyz, 1.0)).xyz;

    //Project the vertex using the model view projection matrix
    gl_Position = u_viewprojection * vec4(world_position, 1.0);
}
```

**Explicación línea por línea:**

1. **Uniforms**: Dos matrices que la CPU envía al shader:
   - `u_model`: Matriz de modelo (posición, rotación y escala de la entidad en el mundo). Transforma de **espacio local** a **espacio mundo**.
   - `u_viewprojection`: Combinación de la View Matrix (dónde está la cámara) y la Projection Matrix (perspectiva). Transforma de **espacio mundo** a **clip space**.

2. **`v_uv = gl_MultiTexCoord0.xy`**: Pasamos las coordenadas de textura al fragment shader. La GPU las interpolará automáticamente para cada píxel del triángulo.

3. **Transformación del vértice** (Local → World → Clip):
   - `u_model * vec4(gl_Vertex.xyz, 1.0)` → posición en **espacio mundo** (world space).
   - `u_viewprojection * vec4(world_position, 1.0)` → posición en **clip space** (la GPU luego hace la división perspectiva y el viewport transform automáticamente).

4. **`gl_Position`**: Variable de salida obligatoria del vertex shader. La GPU usa este valor para determinar dónde se dibuja el vértice en pantalla.

**¿Por qué usamos `vec4(..., 1.0)` para posición?** En coordenadas homogéneas, un punto tiene `w=1.0` (un vector/dirección tendría `w=0.0`). Esto permite que las traslaciones se apliquen correctamente al multiplicar por la matriz.

**Diferencia con `simple.vs`:** Eliminamos las normales del mundo (`v_world_normal`) y la posición del mundo (`v_world_position`) como varyings porque para texturizado básico no las necesitamos. Solo necesitamos las UVs.

#### Fragment Shader — `raster.fs`

```glsl
//Variables from the vertex shader (interpolated)
varying vec2 v_uv;

//Texture from the CPU
uniform sampler2D u_texture;

void main()
{
    gl_FragColor = texture2D(u_texture, v_uv);
}
```

**Explicación:**
1. **`v_uv`**: Las coordenadas de textura llegan **interpoladas** de los 3 vértices del triángulo. La GPU calcula automáticamente la interpolación baricéntrica (lo que en el Lab 3 hacíamos manualmente).
2. **`u_texture`**: La textura de la malla, subida por la CPU como uniform.
3. **`texture2D(u_texture, v_uv)`**: Muestreamos la textura en las coordenadas UV interpoladas para obtener el color del píxel.
4. **`gl_FragColor`**: Devolvemos el color RGBA directamente de la textura (`texture2D` devuelve un `vec4` con alpha incluido).

### Paso 2: Modificar la clase Entity

#### Nuevos atributos en `entity.h`:

```cpp
// GPU rendering properties ("material")
Shader *shader = nullptr;       // Shader que usa esta entidad
Texture *gpu_texture = nullptr; // Textura GPU para el raster shader
```

El shader simula ser el **material** de la entidad. En un motor de juegos real, el material definiría cómo se renderiza un objeto (qué shader usa, qué texturas tiene, qué parámetros de iluminación, etc.).

Usamos `Texture*` (GPU) en vez del `Image*` (CPU) del lab anterior, porque ahora es la GPU la que lee la textura directamente.

#### Nuevo método `Entity::Render(Camera* camera)` en `entity.cpp`:

```cpp
void Entity::Render(Camera *camera) {
  if (!mesh || !shader || !camera)
    return;

  // 1. Obtener la matriz ViewProjection actualizada
  Matrix44 viewprojection = camera->GetViewProjectionMatrix();

  // 2. Activar el shader
  shader->Enable();

  // 3. Enviar uniforms a la GPU
  shader->SetMatrix44("u_model", model);
  shader->SetMatrix44("u_viewprojection", viewprojection);

  // 4. Vincular la textura
  if (gpu_texture)
    shader->SetTexture("u_texture", gpu_texture);

  // 5. Renderizar la malla
  mesh->Render(GL_TRIANGLES);

  // 6. Desactivar el shader
  shader->Disable();
}
```

**Explicación paso a paso:**

1. **`camera->GetViewProjectionMatrix()`**: Calcula y devuelve la matriz `View × Projection`.
   - La **View Matrix** se calcula a partir de `eye`, `center` y `up` de la cámara.
   - La **Projection Matrix** se calcula a partir de `fov`, `aspect`, `near_plane` y `far_plane`.
   - Internamente llama a `UpdateViewMatrix()`, `UpdateProjectionMatrix()` y `UpdateViewProjectionMatrix()`.

2. **`shader->Enable()`**: Activa el programa de shaders en el pipeline de OpenGL. A partir de aquí, cualquier `glDraw*` usará este shader.

3. **Envío de uniforms**:
   - `u_model`: La matriz de modelo de esta entidad específica. Cada entidad tiene su propia matrix con su posición/rotación/escala.
   - `u_viewprojection`: La combinación de View y Projection. Es la misma para todas las entidades en un frame (depende solo de la cámara).

4. **`shader->SetTexture("u_texture", gpu_texture)`**: Vincula la textura al uniform `sampler2D` del fragment shader. OpenGL la asocia a una unidad de textura (texture unit).

5. **`mesh->Render(GL_TRIANGLES)`**: Envía los vértices, normales y UVs del mesh a la GPU. Internamente:
   ```cpp
   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
   // ... normales y UVs ...
   glDrawArrays(GL_TRIANGLES, 0, vertices.size());
   ```
   La GPU toma estos datos y los procesa por el pipeline: vertex shader → rasterización → fragment shader → framebuffer.

### Paso 3: Configurar en Application

#### En `Init()`:

```cpp
// Cargar el shader de rasterización
raster_shader = Shader::Get("shaders/raster.vs", "shaders/raster.fs");

// Inicializar cámara
camera = new Camera();
camera->LookAt(Vector3(0.f, 0.f, 1.5f), Vector3(0.f, 0.f, 0.f), Vector3::UP);
camera->SetPerspective(45.f, window_width / (float)window_height, 0.01f, 100.f);

// Cargar mesh y textura
shared_mesh = new Mesh();
shared_mesh->LoadOBJ("meshes/lee.obj");
entity_texture = Texture::Get("textures/lee_color_specular.tga");

// Crear entidades animadas con el shader como "material"
for (int i = 0; i < 3; ++i) {
    Entity *e = new Entity();
    e->SetMesh(shared_mesh);
    e->shader = raster_shader;        // Material = raster shader
    e->gpu_texture = entity_texture;   // Textura GPU
    // ... posición, rotación, escala, etc.
    entities.push_back(e);
}
```

**Puntos clave:**
- Todas las entidades comparten el **mismo mesh** y la **misma textura** (optimización de memoria).
- Cada entidad tiene su propia **matriz de modelo** (posición/rotación/escala diferentes).
- Las texturas se cargan UNA VEZ en `Init()`, **nunca en `Render()`** ni `Update()` (eso sería cargar 60 veces por segundo).

#### En `Render()`:

```cpp
if (lab_mode == 5) {
    glEnable(GL_DEPTH_TEST);  // Habilitar test de profundidad

    for (Entity *e : entities) {
        e->Render(camera);     // Cada entidad se renderiza con GPU
    }

    glDisable(GL_DEPTH_TEST);
    return;
}
```

**¿Qué es `GL_DEPTH_TEST` y por qué es necesario?**
- Sin depth test, los triángulos se dibujan en el orden en que llegan, y los últimos siempre "tapan" a los primeros, sin importar si están delante o detrás.
- Con `glEnable(GL_DEPTH_TEST)`, la GPU compara la profundidad (Z) de cada píxel nuevo con la del píxel ya almacenado en el **depth buffer**. Solo se pinta si el nuevo píxel está **más cerca** de la cámara.
- Esto es equivalente al z-buffer que implementamos manualmente en el Lab 3 con `FloatImage zbuffer`, pero ahora lo hace la GPU automáticamente.
- `glClear(GL_DEPTH_BUFFER_BIT)` al inicio del frame resetea el depth buffer.

---

## 7. Interactividad (Controles)

| Tecla | Acción |
|-------|--------|
| **1** | Task 2.2: Patrones procedurales (fórmulas) |
| **2** | Task 2.3: Filtros de imagen |
| **3** | Task 2.4: Transformaciones animadas |
| **a** – **f** | Cambiar subtask dentro de la tarea actual |
| **L** | Toggle entre Lab 4 (quad shader) y Lab 5 (malla 3D GPU) |
| **Click izq + arrastrar** | Orbitar la cámara (en Lab 5) |
| **Click der + arrastrar** | Rotar cámara (yaw) |
| **Rueda ratón** | Zoom |
| **ESC** | Salir |

### Implementación de la interactividad:

```cpp
void Application::OnKeyPressed(SDL_KeyboardEvent event) {
    switch (event.keysym.sym) {
    case '1': formula_mode = 1; show_image_filters = false; break;  // Patrones
    case '2': formula_mode = 2; show_image_filters = true;  break;  // Filtros
    case '3': formula_mode = 3; show_image_filters = true;  break;  // Animaciones
    case 'a': subtask_mode = 0; break;
    case 'b': subtask_mode = 1; break;
    // ... etc
    case 'L': lab_mode = (lab_mode != 5) ? 5 : 2; break;  // Toggle Lab 5
    }
}
```

En el `Render()`, el `formula_mode` y `show_image_filters` se envían al shader como uniforms:
- **`u_mode`**: Determina qué subtask se renderiza dentro del fragment shader.
- **`u_show_texture`**: Diferencia entre patrones (cuando es 0) y filtros de textura (cuando es 1).

Para Task 2.4 (animaciones), `formula_mode == 3` mapea los subtasks a los modos 6 y 7 del shader:
```cpp
if (formula_mode == 3) {
    u_mode_to_send = 6 + (subtask_mode % 2);
}
```

---

## 8. Archivos Modificados / Creados

### Archivos nuevos:
| Archivo | Descripción |
|---------|-------------|
| `res/shaders/raster.vs` | Vertex shader para renderizado 3D con transformación de vértices |
| `res/shaders/raster.fs` | Fragment shader para texturizado de mallas 3D |

### Archivos modificados:
| Archivo | Cambios |
|---------|---------|
| `res/shaders/quad.fs` | Fragment shader con todos los patrones, filtros y transformaciones (Tasks 2.2–2.4) |
| `src/framework/entity.h` | Añadidos atributos `Shader*` y `Texture*`, y nuevo método `Render(Camera*)` |
| `src/framework/entity.cpp` | Implementación del método `Render(Camera*)` para renderizado GPU |
| `src/framework/application.h` | Añadidos punteros `raster_shader` y `entity_texture` para Lab 5 |
| `src/framework/application.cpp` | Inicialización de entidades con shader/textura GPU, renderizado con `GL_DEPTH_TEST` |

---

## 9. Pipeline de Renderizado Completo

### Lab 4 (Tasks 2.1–2.4): Quad 2D

```
CPU (Application::Render)
  │
  ├── Sube uniforms: u_mode, u_aspect, u_time, u_show_texture
  ├── Vincula textura: u_texture (fruits.png)
  │
  ▼
quad.vs (Vertex Shader)
  │ gl_Position = gl_Vertex (ya en clip space)
  │ v_uv = gl_MultiTexCoord0.xy
  │
  ▼
[GPU Rasterización automática]
  │ Interpola v_uv por cada píxel
  │
  ▼
quad.fs (Fragment Shader)
  │ Según u_mode y u_show_texture:
  │   - Patrones procedurales (fórmulas matemáticas)
  │   - Filtros de imagen (texture2D + manipulación)
  │   - Transformaciones animadas (UV modification + u_time)
  │
  ▼
gl_FragColor → Pantalla
```

### Lab 5 (Task 2.5): Malla 3D

```
CPU (Entity::Render)
  │
  ├── Calcula ViewProjection = View × Projection
  ├── Sube uniforms: u_model (por entidad), u_viewprojection (cámara)
  ├── Vincula textura: u_texture (lee_color_specular.tga)
  │
  ▼
raster.vs (Vertex Shader)
  │ world_pos = u_model × gl_Vertex        (Local → World)
  │ gl_Position = u_viewprojection × world_pos  (World → Clip)
  │ v_uv = gl_MultiTexCoord0.xy            (pasar UVs)
  │
  ▼
[GPU Rasterización automática]
  │ Interpola v_uv por cada píxel
  │ GL_DEPTH_TEST compara profundidad Z automáticamente
  │
  ▼
raster.fs (Fragment Shader)
  │ gl_FragColor = texture2D(u_texture, v_uv)
  │ (muestrea la textura en las UVs interpoladas)
  │
  ▼
Depth Buffer → gl_FragColor → Pantalla
```

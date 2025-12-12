# 🎮 Block Breaker - Arkanoid Retro

### Objetivo del Juego
Destruye todos los bloques con la pelota mientras controlas la paleta para evitar que caiga. Supera niveles infinitos, enfrenta al jefe final y alcanza la puntuación más alta. ¡Cada 10 bloques destruidos aumenta la velocidad de la pelota para mayor desafío!

### Controles

- **⬅️ Flecha Izquierda**: Mover paleta a la izquierda
- **➡️ Flecha Derecha**: Mover paleta a la derecha
- **Espacio**: Lanzar pelota al inicio
- **M**: Pausar/Reanudar música
- **R**: Reintentar (en Game Over)
- **ESC**: Volver al menú

### Mecánicas

Sistema de Puntuación
- **Bloques normales**: 10 puntos base
- **Progresión de dificultad**: La pelota acelera cada 10 bloques destruidos
- **Velocidad máxima**: 225% de la velocidad inicial (675 px/s)
- **Boost de paleta**: Al alcanzar 700 px/s de velocidad de pelota, la paleta se vuelve 15% más rápida permanentemente

### Sistema de Vidas
- **Vidas iniciales**: 3 corazones
- **Límite máximo**: 3 vidas


### Boss Fight
- **Activación**: Al destruir todos los bloques
- **Mecánica especial**: El jefe suelta bloques destructibles que caen
- **Puntos de vida**: Sistema de resistencia del jefe
- **Recompensa**: Victoria al derrotarlo

### Bloques Especiales
- **🟪 Bloques Morados**: Sueltan powerups al ser destruidos (20% de probabilidad)
- **🟨 Bloques Amarillos**: Ralentizan la paleta temporalmente al ser destruidos (3% de probabilidad)

## 🎁 PowerUps

| PowerUp | Efecto | Duración |
|---------|--------|----------|
|  **Vida Extra** | +1 vida (máximo 3) | Permanente |
|  **Multi-Ball** | 2 pelotas adicionales | Hasta perderlas |
|  **Paleta Ancha** | Paleta 30% más grande | 7 segundos |
|  **Paleta Corta** | Paleta 30% más pequeña | 7 segundos |
|  **Paleta Rápida** | +15% velocidad de paleta | 7 segundos |
|  **Paleta Lenta** | -15% velocidad de paleta | 7 segundos |

##  Características

- **Sistema de Skins**: 6 skins personalizables para la paleta (Hotdog, Laboratorio, Original, Pan, Sable, Trampolín)
- **Música Dinámica**: Música diferente para menú y gameplay con controles de pausa
- **Efectos de Sonido**: Retroalimentación auditiva completa (colisiones, powerups, victoria, derrota)
- **Modo Infinito**: Progresión sin límite de niveles
- **Efectos Visuales Retro**: Diseño estilo 16-bit con bordes 3D en bloques y menú pixelado
- **Boss Battle**: Enfrentamiento épico contra jefe
- **Combos de PowerUps**: Los efectos se pueden combinar para estrategias avanzadas
- **Sistema de Dificultad Adaptativa**: Velocidad incremental basada en progreso

##  Equipo

- **Líder**: Karol Ernesto Gonzalez Nuño ([@karolgzn](https://github.com/karolgzn))
- **Integrante 2**: Cesar Adrian Fabian Ortega ([@chicharon24310320](https://github.com/chicharon24310320))

##  Tecnologías

- **Framework**: SFML 2.x (Simple and Fast Multimedia Library)
- **Lenguaje**: C++17
- **Compilador**: MinGW-w64 (GCC)
- **Build System**: GNU Make
- **Sistema de Gestión**: Git + GitHub
- **Librerías adicionales**: 
  - SFML Graphics
  - SFML Window
  - SFML Audio
  - SFML System

## 🚀 Compilación e Instalación

### Requisitos previos
- MSYS2/MinGW64
- SFML 2.x
- Make
- pkg-config

### Instrucciones

```bash
# Clonar el repositorio
git clone https://github.com/karolgzn/PROYECTO-BLOCK-BREAKER-320-333-1.git
cd PROYECTO-BLOCK-BREAKER-320-333-1

# Compilar el proyecto
make

# Ejecutar el juego
make run
```

### Ejecutable Precompilado
Si no deseas compilar, descarga el ejecutable desde:
`bin/arkanoid.exe`

**Nota importante**: El ejecutable debe estar en la raíz del proyecto para que los assets se carguen correctamente.

## 📜 Créditos

### Assets de Terceros
- Fuente retro: Sistema de fuentes del sistema
- Paleta de colores: Inspirada en estética 16-bit clásica
- Sonidos: Efectos de audio retro personalizados

### Referencias e Inspiraciones
- **Arkanoid** (1986) - Taito Corporation
- **Breakout** (1976) - Atari
- Estilo visual inspirado en juegos de arcade de los 80s y 90s

### Agradecimientos
- Comunidad de SFML por la excelente documentación
- Compañeros de clase por el feedback durante el desarrollo
- Profesores del curso por la guía y apoyo técnico

---

##  Licencia

Este proyecto fue desarrollado con fines educativos como parte del curso de Programación de Videojuegos del Centro De EnseÑanza Tecnica Industrial (CETI), plantel Colomos.

---

**Desarrollado con ❤️ y mucho código en C++**
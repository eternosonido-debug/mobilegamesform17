# Cross-compile para M17 Linux ARMv7

Não há SDK universal verificado para todas as revisões/firmwares. Use o SDK
Buildroot exportado da imagem em uso, ou o toolchain/sysroot fornecido por seu
mantenedor. Um pacote `gcc-arm-linux-gnueabihf` genérico não garante compatibilidade
com a libc antiga de fábrica. O SDK deve incluir headers/libs do SDL2 e libstdc++
compatíveis; copiar só headers SDL não basta.

## Pré-requisitos

Host Linux compatível com os executáveis do SDK (muitos são x86_64); CMake >=3.16,
make, compilador C++11; SDK ARM com sysroot. Em Mac ARM, use máquina Linux/VM
adequada ao host do SDK. Não há VM/toolchain ARM instalada nesta entrega.

```sh
# Exemplos: substitua os caminhos e o triplet pelos do seu SDK real.
export M17_TOOLCHAIN_PREFIX=/opt/m17-sdk/bin/arm-buildroot-linux-gnueabihf-
export M17_SYSROOT=/opt/m17-sdk/arm-buildroot-linux-gnueabihf/sysroot
"${M17_TOOLCHAIN_PREFIX}g++" --version
"${M17_TOOLCHAIN_PREFIX}g++" -print-sysroot
cmake -S . -B build-m17 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/m17-armhf.cmake \
  -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF \
  -DSDL2_DIR="$M17_SYSROOT/usr/lib/cmake/SDL2"
cmake --build build-m17 -j2
"${M17_TOOLCHAIN_PREFIX}strip" build-m17/fake-mobile-games
```

O SDL2_DIR varia conforme o SDK. Use diretório que contenha SDL2Config.cmake
do alvo, nunca o SDL2 de desktop. O arquivo CMake restringe buscas ao sysroot.
Não rode testes ARM diretamente no PC. Execute os testes de regras em build
separado de host.

## Se o SDK não tem SDL2

Compile SDL2 com o mesmo SDK/toolchain e instale no staging/sysroot, ou habilite
SDL2 na configuração Buildroot dessa imagem. Exemplo para uma árvore SDL2 obtida
do [site oficial](https://www.libsdl.org/release/), com backend escolhido conforme
os drivers existentes na imagem:

```sh
cmake -S /caminho/SDL2 -B build-sdl-m17 \
  -DCMAKE_TOOLCHAIN_FILE="$PWD/cmake/m17-armhf.cmake" \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr \
  -DSDL_SHARED=ON -DSDL_STATIC=OFF -DSDL_TEST=OFF
cmake --build build-sdl-m17 -j2
DESTDIR="$M17_SYSROOT" cmake --install build-sdl-m17
```

Isso instala em seu sysroot de desenvolvimento, não no console. SDL2 precisa de
um backend de vídeo funcional (por exemplo KMSDRM com DRM/GBM/EGL, ou outro backend
específico do firmware). **O renderizador software não resolve falta de backend
de vídeo.** SDL2 upstream não oferece um backend genérico direto para fbcon;
se o firmware só oferece framebuffer, pode ser necessário o SDL modificado do
mantenedor ou uma adaptação de plataforma. Não presuma OpenGL nem X11.

## Conferência do resultado

```sh
file build-m17/fake-mobile-games
"${M17_TOOLCHAIN_PREFIX}readelf" -h -A -l build-m17/fake-mobile-games
"${M17_TOOLCHAIN_PREFIX}readelf" -d build-m17/fake-mobile-games
```

Compare o loader e ABI com um binário funcional do mesmo console. Confira
DT_NEEDED e versões de GLIBC/GLIBCXX requeridas contra o sysroot. Se houver
bibliotecas adicionais, use as correspondentes ao SDK, mantenha suas licenças e
coloque-as em `fake-mobile-games/lib/`. Não substitua a libc/loader do firmware.
SDL2 é zlib; libstdc++ tem suas condições de distribuição e runtime exception.

Só depois copie para o SD. Um build ARM bem-sucedido ainda não comprova que a
exibição, os controles ou o desempenho funcionem no hardware.

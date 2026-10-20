# Boca atlas v2

Unified original sprite artwork generated for this project: blue compact, red sedan,
yellow taxi, ambulance, fire engine, delivery truck, house, apartment building and SUV.
All city vehicles and buildings map to this atlas; no normal-play variants use the
older procedural artwork. Earlier atlas files are no longer shipped.

boca-atlas-v2.png is the source RGBA image (1254 × 1254, 3 × 3 cells of 418 pixels).
boca-atlas-v2.bmp is a lossless BGRA V4 pixel-format conversion for SDL_LoadBMP.
Source bounds and ground anchors are defined in src/boca_sprites.hpp. Horizontal
mirroring aligns sprites with either diagonal street axis. Texture filtering is
linear. Runtime pixel storage is about 6 MiB. FMG_ASSET_DIR can override the asset
folder; normal installation places assets beside bin/. No SDL_image is required.

A procedural fallback exists only for a missing texture, not as mixed variants in
normal play. Install the matching binary AND atlas together.

{
  "targets": [
    {
      "target_name": "node-rpi-si4703",
      "cflags!": ['-fno-exceptions -std=c++11'],
      "cflags_cc!": ['-fno-exceptions -std=c++11'],
      "sources": ["src/node-rpi-si4703.cpp", "src/FMTuner.cpp", "src/FMTuner.h", "src/rpi-si4703/Si4703_Breakout.cpp", "src/rpi-si4703/Si4703_Breakout.h"]
    },
    {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": ["node-rpi-si4703"],
      "copies": [
        {
          "files": ["<(PRODUCT_DIR)/node-rpi-si4703.node"],
          "destination": "node-rpi-si4703"
        }
      ]
    }
  ]
}
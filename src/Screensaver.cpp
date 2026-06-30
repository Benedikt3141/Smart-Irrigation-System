#include <Arduino.h>
#include "pindefinitions.h"
#include "functions.h"
#include <TFT_eSPI.h>
#include <JPEGDEC.h>
#include <SD.h>


extern TFT_eSPI tft;
extern JPEGDEC jpeg;


bool isPlaying;
File videoFile;

#define MJPEG_BUFFER_SIZE (1024 * 40)
uint8_t mjpeg_buf[MJPEG_BUFFER_SIZE];

unsigned long lastFrameTime = 0;
const int fps = 10;
const int frameDelay = 1000 / fps;


int JPEGDraw(JPEGDRAW *pDraw) {
  tft.pushImage(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, pDraw->pPixels);
  return 1;
}

void playVideo(String videoName) {
  if (!mjpeg_buf) {
    Serial.println("MJPEG buffer not allocated!");
    return;
  }

  String path = "/" + videoName;
  Serial.print("Opening video: ");
  Serial.println(path);

  videoFile = SD.open(path);

  if (!videoFile) {
    Serial.println("Could not open video file!");
    return;
  }

  Serial.print("Video size: ");
  Serial.println(videoFile.size());

  isPlaying = true;
  tft.fillScreen(TFT_BLACK);

  int bytesInBuf = 0;
  bytesInBuf = videoFile.read(mjpeg_buf, MJPEG_BUFFER_SIZE);

  while (isPlaying) {
    int frameStartIdx = -1;
    int frameEndIdx = -1;

    for (int i = 0; i < bytesInBuf - 1; i++) {
      if (mjpeg_buf[i] == 0xFF) {
        if (mjpeg_buf[i + 1] == 0xD8) frameStartIdx = i;
        else if (mjpeg_buf[i + 1] == 0xD9) {
          frameEndIdx = i + 1;
          if (frameStartIdx != -1) break;
        }
      }
    }

    if (frameStartIdx != -1 && frameEndIdx != -1) {
      int frameSize = frameEndIdx - frameStartIdx + 1;

      while (millis() - lastFrameTime < frameDelay) {
        // ---------------------------------- later place interrupt --------------------
      }
      lastFrameTime = millis();

      if (jpeg.openRAM(mjpeg_buf + frameStartIdx, frameSize, JPEGDraw)) {
        int x = (tft.width() - jpeg.getWidth()) / 2;
        int y = (tft.height() - jpeg.getHeight()) / 2;

        jpeg.decode(x, y, 0); 
        jpeg.close();
      }

      int remaining = bytesInBuf - (frameEndIdx + 1);
      if (remaining > 0) memmove(mjpeg_buf, mjpeg_buf + frameEndIdx + 1, remaining);

      int toRead = MJPEG_BUFFER_SIZE - remaining;
      if (toRead > 0) {
        int actuallyRead = videoFile.read(mjpeg_buf + remaining, toRead);
        bytesInBuf = remaining + actuallyRead;
        if (actuallyRead == 0 && remaining == 0) isPlaying = false;
      } else {
        bytesInBuf = remaining;
      }

    } else {
      if (bytesInBuf < MJPEG_BUFFER_SIZE) {
        int actuallyRead = videoFile.read(mjpeg_buf + bytesInBuf, MJPEG_BUFFER_SIZE - bytesInBuf);
        if (actuallyRead == 0) isPlaying = false;
        bytesInBuf += actuallyRead;
      } else {
        memmove(mjpeg_buf, mjpeg_buf + 1, MJPEG_BUFFER_SIZE - 1);
        bytesInBuf--;
      }
    }

    // ---------------------------------- later place interrupt --------------------
  }

  videoFile.close();
}
